#include "pdu.h"
#include "zigbee_meter_values.h"
#include <esp_log.h>
#include <memory.h>

// 0F 004B10AB 0C 07E80A1B0707232DFF800000 020D010D 02041200 28090600 08190900 FF0F0212 00000204 12002809 06000819
// 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 00000204 12000309 06010001 0700FF0F 02120000 02041200 03090601
// 00020700 FF0F0212 0000 02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 00FF0F02 12000002 04120003
// 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 04120003 09060100 470700FF 0F021200 00020412
// 00030906 01002007 00FF0F02 120000 02 04120003 09060100 340700FF 0F021200 00020412 00030906 01004807 00FF0F02 12000009
// 06000819 0900FF09 08363636 38313431 30060000 015A0600 00000006 003D1B04 06000000 00120074 12000E12 00231200 EB1200EA
// 1200EA

// <DataNotification>
//  <LongInvokeIdAndPriority Value="004B10AB" />
//  <!--2024-10-27 07:35:45-->
//  <DateTime Value="07E80A1B0707232DFF800000" />
//  <NotificationBody>
//    <DataValue>
//      <Structure Qty="0D" >
//        <Array Qty="0D" >
//          <Structure Qty="04" >
//            <UInt16 Value="0028" />
//            <!--0.8.25.9.0.255-->
//            <OctetString Value="0008190900FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0028" />
//            <!--0.8.25.9.0.255-->
//            <OctetString Value="0008190900FF" />
//            <Int8 Value="01" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0001" />
//            <!--0.0.96.1.0.255 N° série fabricant -->
//            <OctetString Value="0000600100FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.1.7.0.255 Puissance active import-->
//            <OctetString Value="0100010700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.2.7.0.255 Puissance active export-->
//            <OctetString Value="0100020700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.1.1.8.0.255 Energie active import totale-->
//            <OctetString Value="0101010800FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.1.2.8.0.255 Energie active export totale -->
//            <OctetString Value="0101020800FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.31.7.0.255 Courant instantané L1-->
//            <OctetString Value="01001F0700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.51.7.0.255 Courant instantané L2-->
//            <OctetString Value="0100330700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.71.7.0.255 Courant instantané L3-->
//            <OctetString Value="0100470700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.32.7.0.255 tension instantanée L1-->
//            <OctetString Value="0100200700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.52.7.0.255 tension instantanée L2-->
//            <OctetString Value="0100340700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.72.7.0.255 tension instantanée L3-->
//            <OctetString Value="0100480700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//        </Array>
//        <!--0.8.25.9.0.255-->
//        <OctetString Value="0008190900FF" />
//        <!--66681410-->
//        <OctetString Value="3636363831343130" /> <!-- N° série fabricant -->
//        <UInt32 Value="0000015A" /> <!-- Puissance active import -->
//        <UInt32 Value="00000000" /> <!-- Puissance active export -->
//        <UInt32 Value="003D1B04" /> <!-- Energie active import totale -->
//        <UInt32 Value="00000000" /> <!-- Energie active export totale -->
//        <UInt16 Value="0074" /> <!-- courant instantané L1 -->
//        <UInt16 Value="000E" /> <!-- courant instantané L2 -->
//        <UInt16 Value="0023" /> <!-- courant instantané L3 -->
//        <UInt16 Value="00EB" /> <!-- tension instantanée L1 -->
//        <UInt16 Value="00EA" /> <!-- tension instantanée L2 -->
//        <UInt16 Value="00EA" /> <!-- tension instantanée L3 -->
//      </Structure>
//    </DataValue>
//  </NotificationBody>
//</DataNotification>

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

static const uint8_t UNKNOWN_FIELD[] = {0, 8, 25, 9, 0, 255};
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

static bool decode_item(const uint8_t *bytes, int size, int *pos, StateValues *stateValues) {
    if (*pos + 1 > size) {
        return false;
    }
    uint8_t type = bytes[*pos];
    ++*pos;
    switch (type) {
    case STRUCTURE_TYPE: {
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t quantity = bytes[*pos];
        ++*pos;
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
            if (!decode_item(bytes, size, pos, stateValues)) {
                return false;
            }
        }
        stateValues->state = backState;
        return true;
    }
    case ARRAY_TYPE: {
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t quantity = bytes[*pos];
        ++*pos;
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
            if (!decode_item(bytes, size, pos, stateValues)) {
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
        if (*pos + 4 > size) {
            return false;
        }
        const uint32_t value = (bytes[*pos] << 24) | (bytes[*pos + 1] << 16) | (bytes[*pos + 2] << 8) | bytes[*pos + 3];
        *pos += 4;
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
        if (*pos + 2 > size) {
            return false;
        }
        const uint16_t value = bytes[*pos] << 8 | bytes[*pos + 1];
        *pos += 2;
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
        if (*pos + 1 > size) {
            return false;
        }
        const int8_t value = (int8_t)bytes[*pos];
        ++*pos;
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
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t stringSize = bytes[*pos];
        ++*pos;
        if (*pos + stringSize > size) {
            return false;
        }
        switch (stateValues->state) {
        case FIELD_DEF2:
            memcpy(stateValues->fields[stateValues->curDefField].field2, bytes + *pos, 6);
            break;
        case DATAS:
            if (curFieldIs(stateValues, UNKNOWN_FIELD) || curFieldIs(stateValues, SERIAL_FIELD)) {
                // ignored fields
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

        *pos += stringSize;
        return true;
    }
    default:
        ESP_LOGW(TAG, "Unknown type: 0x%02x", type);
        return false;
    }
}

bool pdu_decode(const uint8_t *bytes, int size) {
    int pos = 0;
    pos += 1;  // unknown crap
    pos += 4;  // LongInvokeIdAndPriority
    pos += 1;  // unknown crap
    pos += 12; // DateTime

    StateValues stateValues = {};
    if (!decode_item(bytes, size, &pos, &stateValues)) {
        return false;
    }
    return pos == size;
}
