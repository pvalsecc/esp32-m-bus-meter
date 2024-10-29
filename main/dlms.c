#include "dlms.h"
#include "buffer.h"
#include "stream.h"
#include <esp_log.h>
#include <malloc.h>

static const char *TAG = "dlms";
typedef struct dlms_state {
    Buffer buffer;
    uint8_t nextBlockNumber;
    dlms_handler cb;
    void *arg;
} dlms_state;

dlms_state *dlms_init(dlms_handler cb, void *arg) {
    dlms_state *state = malloc(sizeof(dlms_state));
    buffer_reset(&state->buffer);
    state->nextBlockNumber = 1;
    state->cb = cb;
    state->arg = arg;
    return state;
}

bool dlms_decode(dlms_state *state, const Buffer *buffer) {
    Stream stream;
    stream_reset(&stream, buffer);
    uint8_t type;
    if (!stream_getu8(&stream, &type)) {
        ESP_LOGW(TAG, "Empty DLMS packet");
        return false;
    }

    switch (type) {
    case 0xE6:
        stream_skip(&stream, 3); // 3 bytes of unknown stuff
        break;
    case 0xE0:
        break;
    default:
        ESP_LOGW(TAG, "Unknown type: 0x%02x", type);
        return false;
    }

    uint8_t blockControl;
    if (!stream_getu8(&stream, &blockControl)) {
        return false;
    }
    const bool lastBlock = blockControl & 0x80;

    uint16_t blockNumber;
    if (!stream_getu16_be(&stream, &blockNumber)) {
        return false;
    }
    if (blockNumber != state->nextBlockNumber) {
        ESP_LOGW(TAG, "Out of sequence DLMS packet received: %d != %d", blockNumber, state->nextBlockNumber);
        buffer_reset(&state->buffer);
        state->nextBlockNumber = 1;
        return false;
    }

    // block number ack (don't care)
    if (!stream_skip(&stream, 2)) {
        return false;
    }

    uint8_t dataLen;
    if (!stream_getu8(&stream, &dataLen)) {
        return false;
    }

    if (dataLen != stream_remains(&stream)) {
        ESP_LOGW(TAG, "Invalid DLMS data length: %d != %d", dataLen, stream_remains(&stream));
        return false;
    }

    if (!stream_get_buffer(&stream, &state->buffer, dataLen)) {
        return false;
    }

    state->nextBlockNumber++;

    if (lastBlock) {
        state->cb(state->arg, &state->buffer);
        buffer_reset(&state->buffer);
        state->nextBlockNumber = 1;
    }

    return true;
}
