#include "dlms.h"
#include <esp_log.h>
#include <malloc.h>
#include <memory.h>

static const char *TAG = "dlms";
#define BUFFER_LENGTH 1024
typedef struct dlms_state {
    uint8_t buffer[BUFFER_LENGTH];
    int len;
    uint8_t nextBlockNumber;
    dlms_handler cb;
    void *arg;
} dlms_state;

dlms_state *dlms_init(dlms_handler cb, void *arg) {
    dlms_state *state = malloc(sizeof(dlms_state));
    memset(state->buffer, 0, BUFFER_LENGTH);
    state->len = 0;
    state->nextBlockNumber = 1;
    state->cb = cb;
    state->arg = arg;
    return state;
}

bool dlms_decode(dlms_state *state, const uint8_t *bytes, int size) {
    if (size < 1) {
        ESP_LOGW(TAG, "Empty DLMS packet");
        return false;
    }

    int pos = 0;

    switch (bytes[pos++]) {
    case 0xE6:
        pos += 3; // 3 bytes of unknown stuff
        break;
    case 0xE0:
        break;
    default:
        ESP_LOGW(TAG, "Unknown byte0: 0x%02x", bytes[0]);
        return false;
    }

    if (pos + 6 > size) {
        ESP_LOGW(TAG, "DLMS packet not big enough");
        return false;
    }

    const uint8_t blockControl = bytes[pos++];
    const bool lastBlock = blockControl & 0x80;

    const uint16_t blockNumber = bytes[pos] << 8 | bytes[pos + 1];
    pos += 2;
    if (blockNumber != state->nextBlockNumber) {
        ESP_LOGW(TAG, "Out of sequence DLMS packet received: %d != %d", blockNumber, state->nextBlockNumber);
        state->len = 0;
        state->nextBlockNumber = 1;
        return false;
    }

    pos += 2; // block number ack (don't care)

    int dataLen = bytes[pos++];

    if (dataLen != size - pos) {
        ESP_LOGW(TAG, "Invalid DLMS data length: %d != %d", dataLen, size - pos);
        return false;
    }

    if (state->len + dataLen > BUFFER_LENGTH) {
        ESP_LOGW(TAG, "DLMS buffer overflow: %d > %d", state->len + dataLen, BUFFER_LENGTH);
        return false;
    }

    memcpy(state->buffer + state->len, bytes + pos, dataLen);
    state->len += dataLen;
    state->nextBlockNumber++;

    if (lastBlock) {
        state->cb(state->arg, state->buffer, state->len);
        state->len = 0;
        state->nextBlockNumber = 1;
    }

    return true;
}
