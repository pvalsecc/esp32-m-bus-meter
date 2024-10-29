#include "hdlc_packet.h"
#include "buffer.h"
#include "hdlc_fields.h"
#include "stream.h"
#include <esp_log.h>
#include <stddef.h>

static const char *TAG = "hdlc_packet";

bool hdlc_packet_decode(const Buffer *packet, Buffer *information) {
    if (!packet || !information) {
        return false;
    }

    Stream stream;
    stream_reset(&stream, packet);

    uint8_t type;
    bool segmentation;
    uint16_t hdlcSize;
    if (!hdlc_decode_type_length(&stream, &type, &segmentation, &hdlcSize)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC type/length");
        return false;
    }
    if (hdlcSize != packet->len) {
        ESP_LOGW(TAG, "Invalid HDLC length: %d != %d", hdlcSize, packet->len);
        return false;
    }

    if (!hdlc_decode_address(&stream, NULL)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC destination address");
        return false;
    }

    if (!hdlc_decode_address(&stream, NULL)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC source address");
        return false;
    }

    ControlField controlField;
    if (!hdlc_decode_control(&stream, &controlField)) {
        ESP_LOGW(TAG, "Failed decoding the HDLC control field");
        return false;
    }
    if (controlField.type != UNNUMBERED_COMMAND) {
        ESP_LOGW(TAG, "The control field has a wrong type: 0x%02x", controlField.type);
        return false;
    }

    if (!hdlc_decode_crc16(&stream)) {
        ESP_LOGW(TAG, "Wrong HDLC header CRC");
        return false;
    }

    buffer_reset(information);
    if (!stream_get_buffer(&stream, information, stream_remains(&stream) - 2)) {
        return false;
    }

    if (!hdlc_decode_crc16(&stream)) {
        ESP_LOGW(TAG, "Wrong HDLC packet CRC");
        return false;
    }

    return true;
}
