#include "hdlc_packet.h"
#include "hdlc_fields.h"
#include <esp_log.h>
#include <stddef.h>

static const char *TAG = "hdlc_packet";

Information hdlc_packet_decode(const uint8_t *bytes, int size) {
    int pos = 0;

    uint8_t type;
    bool segmentation;
    uint16_t hdlcSize;
    if (!hdlc_decode_type_length(bytes, size, &pos, &type, &segmentation, &hdlcSize)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC type/length");
        return (Information){};
    }
    if (hdlcSize != size) {
        ESP_LOGW(TAG, "Invalid HDLC length: %d != %d", hdlcSize, size);
        return (Information){};
    }

    if (!hdlc_decode_address(bytes, size, &pos, NULL)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC destination address");
        return (Information){};
    }

    if (!hdlc_decode_address(bytes, size, &pos, NULL)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC source address");
        return (Information){};
    }

    ControlField controlField;
    if (!hdlc_decode_control(bytes, size, &pos, &controlField)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC control field");
        return (Information){};
    }
    if (controlField.type != UNNUMBERED_COMMAND) {
        ESP_LOGW(TAG, "The control field has a wrong type: 0x%02x", controlField.type);
        return (Information){};
    }

    if (!hdlc_decode_crc16(bytes, size, &pos)) {
        ESP_LOGW(TAG, "Wrong HDLC header CRC");
        return (Information){};
    }

    const uint8_t *information = bytes + pos;
    const int informationLen = size - pos - 2;
    pos += informationLen;

    if (!hdlc_decode_crc16(bytes, size, &pos)) {
        ESP_LOGW(TAG, "Wrong HDLC packet CRC");
        return (Information){};
    }

    return (Information){.size = informationLen, .bytes = information};
}
