#include "pdu.h"
#include "buffer.h"
#include "stream.h"
#include "zigbee_meter_values.h"
#include <esp_log.h>
#include <memory.h>

static const char *TAG = "pdu";

typedef enum {
    NULL_TYPE = 0x00,
    ARRAY_TYPE = 0x01,
    STRUCTURE_TYPE = 0x02,
    BOOLEAN_TYPE = 0x03,
    BIT_STRING_TYPE = 0x04,
    INT32_TYPE = 0x05,
    UINT32_TYPE = 0x06,
    OCTET_STRING_TYPE = 0x09,
    VISIBLE_STRING_TYPE = 0x0A,
    BCD_TYPE = 0x0D,
    INT8_TYPE = 0x0F,
    INT16_TYPE = 0x10,
    UINT8_TYPE = 0x11,
    UINT16_TYPE = 0x12,
    COMPACT_ARRAY_TYPE = 0x13,
    LONG64_TYPE = 0x14,
    UNSIGNED_LONG64_TYPE = 0x15,
    ENUM_TYPE = 0x16,
    FLOAT32_TYPE = 0x17,
    FLOAT64_TYPE = 0x18,
    DATE_TIME_TYPE = 0x19,
    DATE_TYPE = 0x1A,
    TIME_TYPE = 0x1B
} Types;

static const uint8_t UNKNOWN_FIELD1[] = {0, 8, 25, 9, 0, 255};
static const uint8_t SERIAL_FIELD[] = {0, 0, 96, 1, 0, 255};
static const uint8_t IMPORTED_ACTIVE_POWER_FIELD[] = {1, 0, 1, 7, 0, 255};
static const uint8_t EXPORTED_ACTIVE_POWER_FIELD[] = {1, 0, 2, 7, 0, 255};
static const uint8_t IMPORTED_ACTIVE_ENERGY_FIELD[] = {1, 1, 1, 8, 0, 255};
static const uint8_t EXPORTED_ACTIVE_ENERGY_FIELD[] = {1, 1, 2, 8, 0, 255};
static const uint8_t CURRENT_P1_FIELD[] = {1, 0, 31, 7, 0, 255};
static const uint8_t CURRENT_P2_FIELD[] = {1, 0, 51, 7, 0, 255};
static const uint8_t CURRENT_P3_FIELD[] = {1, 0, 71, 7, 0, 255};
static const uint8_t VOLTAGE_P1_FIELD[] = {1, 0, 32, 7, 0, 255};
static const uint8_t VOLTAGE_P2_FIELD[] = {1, 0, 52, 7, 0, 255};
static const uint8_t VOLTAGE_P3_FIELD[] = {1, 0, 72, 7, 0, 255};

// stuff from a second kind of packet, sent every minutes
static const uint8_t UNKNOWN_FIELD2[] = {0, 9, 25, 9, 0, 255};
static const uint8_t IMPORTED_ACTIVE_ENERGY_RATE1_FIELD[] = {1, 1, 1, 8, 1, 255};
static const uint8_t IMPORTED_ACTIVE_ENERGY_RATE2_FIELD[] = {1, 1, 1, 8, 2, 255};
static const uint8_t EXPORTED_ACTIVE_ENERGY_RATE1_FIELD[] = {1, 1, 2, 8, 1, 255};
static const uint8_t EXPORTED_ACTIVE_ENERGY_RATE2_FIELD[] = {1, 1, 2, 8, 2, 255};

typedef enum { ROOT, OUTER_STRUCT, FIRST_ARRAY, FIELD_DEF1, FIELD_DEF2, FIELD_DEF3, FIELD_DEF4, DATAS } State;

typedef struct {
    uint16_t field1;
    uint8_t field2[6];
    int8_t field3;
    uint16_t field4;
} FieldDef;

#define MAX_FIELDS 20

typedef struct {
    State state;
    FieldDef fields[MAX_FIELDS];
    int curDefField;
    int curUseField;
} StateValues;

static bool curFieldIs(const StateValues *statueValues, const uint8_t field[6]) {
    return memcmp(statueValues->fields[statueValues->curUseField].field2, field, 6) == 0;
}

static bool decode_item(Stream *stream, StateValues *stateValues) {
    uint8_t type;
    if (!stream_getu8(stream, &type)) {
        return false;
    }
    switch (type) {
    case STRUCTURE_TYPE: {
        uint8_t quantity;
        if (!stream_getu8(stream, &quantity)) {
            return false;
        }
        State backState = stateValues->state;
        bool def = false;
        switch (stateValues->state) {
        case ROOT:
            stateValues->state = OUTER_STRUCT;
            break;
        case FIRST_ARRAY:
            if (quantity != 4) {
                ESP_LOGW(TAG, "Struct for a field def with an invalid number of fields: %d != 4", quantity);
                return false;
            }
            def = true;
            break;
        default:
            ESP_LOGW(TAG, "Received a struct while being in a weird state: %d", stateValues->state);
            return false;
        }
        for (uint8_t i = 0; i < quantity; ++i) {
            if (def) {
                stateValues->state = FIELD_DEF1 + i;
            }
            if (!decode_item(stream, stateValues)) {
                return false;
            }
        }
        stateValues->state = backState;
        return true;
    }
    case ARRAY_TYPE: {
        uint8_t quantity;
        if (!stream_getu8(stream, &quantity)) {
            return false;
        }
        State backState = stateValues->state;
        bool fieldDefs = false;
        switch (stateValues->state) {
        case OUTER_STRUCT:
            stateValues->state = FIRST_ARRAY;
            fieldDefs = true;
            break;
        default:
            ESP_LOGW(TAG, "Received an array while being in a weird state: %d", stateValues->state);
            return false;
        }
        for (uint8_t i = 0; i < quantity; ++i) {
            if (fieldDefs && stateValues->curDefField >= MAX_FIELDS) {
                ESP_LOGW(TAG, "Too many fields");
                return false;
            }
            if (!decode_item(stream, stateValues)) {
                return false;
            }
            if (fieldDefs) {
                if (stateValues->fields[stateValues->curDefField].field3 == 2) {
                    stateValues->curDefField++;
                } else {
                    memset(&stateValues->fields[stateValues->curDefField], 0, sizeof(FieldDef));
                }
            }
        }
        if (fieldDefs) {
            stateValues->state = DATAS;
        } else {
            stateValues->state = backState;
        }
        return true;
    }
    case UINT32_TYPE: {
        uint32_t value;
        if (!stream_getu32_be(stream, &value)) {
            return false;
        }
        switch (stateValues->state) {
        case FIELD_DEF1:
            stateValues->fields[stateValues->curDefField].field1 = value;
            break;
        case FIELD_DEF4:
            stateValues->fields[stateValues->curDefField].field4 = value;
            break;
        case DATAS:
            if (curFieldIs(stateValues, IMPORTED_ACTIVE_POWER_FIELD)) {
                zigbee_meter_update_active_power(value);
            } else if (curFieldIs(stateValues, EXPORTED_ACTIVE_POWER_FIELD)) {
                if (value > 0) {
                    zigbee_meter_update_active_power(-value);
                }
            } else if (curFieldIs(stateValues, IMPORTED_ACTIVE_ENERGY_FIELD)) {
                zigbee_meter_update_summation_received(value);
            } else if (curFieldIs(stateValues, EXPORTED_ACTIVE_ENERGY_FIELD)) {
                zigbee_meter_update_summation_delivered(value);
            } else if (curFieldIs(stateValues, IMPORTED_ACTIVE_ENERGY_RATE1_FIELD) ||
                       curFieldIs(stateValues, IMPORTED_ACTIVE_ENERGY_RATE2_FIELD) ||
                       curFieldIs(stateValues, EXPORTED_ACTIVE_ENERGY_RATE1_FIELD) ||
                       curFieldIs(stateValues, EXPORTED_ACTIVE_ENERGY_RATE2_FIELD)) {
                // ignored field
            } else {
                const FieldDef *curField = stateValues->fields + stateValues->curUseField;
                ESP_LOGW(TAG, "Wrong uint32 type for field: %d.%d.%d.%d.%d.%d", curField->field2[0],
                         curField->field2[1], curField->field2[2], curField->field2[3], curField->field2[4],
                         curField->field2[5]);
                return false;
            }
            stateValues->curUseField++;
            break;
        default:
            ESP_LOGW(TAG, "Received an uint32 while being in a weird state: %d", stateValues->state);
            return false;
        }
        return true;
    }
    case UINT16_TYPE: {
        uint16_t value;
        if (!stream_getu16_be(stream, &value)) {
            return false;
        }
        switch (stateValues->state) {
        case FIELD_DEF1:
            stateValues->fields[stateValues->curDefField].field1 = value;
            break;
        case FIELD_DEF4:
            stateValues->fields[stateValues->curDefField].field4 = value;
            break;
        case DATAS:
            if (curFieldIs(stateValues, CURRENT_P1_FIELD)) {
                zigbee_meter_update_rms_current(0, value);
            } else if (curFieldIs(stateValues, CURRENT_P2_FIELD)) {
                zigbee_meter_update_rms_current(1, value);
            } else if (curFieldIs(stateValues, CURRENT_P3_FIELD)) {
                zigbee_meter_update_rms_current(2, value);
            } else if (curFieldIs(stateValues, VOLTAGE_P1_FIELD)) {
                zigbee_meter_update_rms_voltage(0, value);
            } else if (curFieldIs(stateValues, VOLTAGE_P2_FIELD)) {
                zigbee_meter_update_rms_voltage(1, value);
            } else if (curFieldIs(stateValues, VOLTAGE_P3_FIELD)) {
                zigbee_meter_update_rms_voltage(2, value);
            } else {
                const FieldDef *curField = stateValues->fields + stateValues->curUseField;
                ESP_LOGW(TAG, "Wrong uint16 type for field: %d.%d.%d.%d.%d.%d", curField->field2[0],
                         curField->field2[1], curField->field2[2], curField->field2[3], curField->field2[4],
                         curField->field2[5]);
                return false;
            }
            stateValues->curUseField++;
            break;
        default:
            ESP_LOGW(TAG, "Received an uint16 while being in a weird state: %d", stateValues->state);
            return false;
        }
        return true;
    }
    case INT8_TYPE: {
        int8_t value;
        if (!stream_get8(stream, &value)) {
            return false;
        }
        switch (stateValues->state) {
        case FIELD_DEF3:
            stateValues->fields[stateValues->curDefField].field3 = value;
            break;
        default:
            ESP_LOGW(TAG, "Received an uint8 while being in a weird state: %d", stateValues->state);
            return false;
        }
        return true;
    }
    case OCTET_STRING_TYPE: {
        uint8_t stringSize;
        if (!stream_getu8(stream, &stringSize)) {
            return false;
        }
        switch (stateValues->state) {
        case FIELD_DEF2:
            if (stringSize != 6) {
                return false;
            }
            if (!stream_get(stream, stateValues->fields[stateValues->curDefField].field2, stringSize)) {
                return false;
            }
            break;
        case DATAS:
            if (curFieldIs(stateValues, UNKNOWN_FIELD1) || curFieldIs(stateValues, SERIAL_FIELD) ||
                curFieldIs(stateValues, UNKNOWN_FIELD2)) {
                // ignored fields
                stream_skip(stream, stringSize);
            } else {
                const FieldDef *curField = stateValues->fields + stateValues->curUseField;
                ESP_LOGW(TAG, "Wrong octetString type for field: %d.%d.%d.%d.%d.%d", curField->field2[0],
                         curField->field2[1], curField->field2[2], curField->field2[3], curField->field2[4],
                         curField->field2[5]);
                return false;
            }
            stateValues->curUseField++;
            break;
        default:
            ESP_LOGW(TAG, "Received an uint16 while being in a weird state: %d", stateValues->state);
            return false;
        }

        return true;
    }
    default:
        ESP_LOGW(TAG, "Unknown type: 0x%02x", type);
        return false;
    }
}

bool pdu_decode(const Buffer *buffer) {
    Stream stream;
    stream_reset(&stream, buffer);
    stream_skip(&stream, 1);  // unknown crap
    stream_skip(&stream, 4);  // LongInvokeIdAndPriority
    stream_skip(&stream, 1);  // unknown crap
    stream_skip(&stream, 12); // DateTime

    StateValues stateValues = {};
    if (!decode_item(&stream, &stateValues)) {
        return false;
    }
    return stream_remains(&stream) == 0;
}
