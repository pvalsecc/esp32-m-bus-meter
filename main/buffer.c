#include "buffer.h"
#include <esp_log.h>
#include <memory.h>
#include <stdio.h>

static const char *TAG = "buffer";

void buffer_reset(Buffer *buffer) { buffer->len = 0; }

bool buffer_add_byte(Buffer *buffer, uint8_t byte) {
    if (buffer->len >= BUFFER_SIZE) {
        ESP_LOGW(TAG, "Overflow: %d > %d", buffer->len + 1, BUFFER_SIZE);
        return false;
    }
    buffer->bytes[buffer->len++] = byte;
    return true;
}

bool buffer_add_bytes(Buffer *buffer, const uint8_t *bytes, int len) {
    if (buffer->len + len > BUFFER_SIZE) {
        ESP_LOGW(TAG, "Overflow: %d > %d", buffer->len + len, BUFFER_SIZE);
        return false;
    }
    memcpy(buffer->bytes + buffer->len, bytes, len);
    buffer->len += len;
    return true;
}

void buffer_dump(const Buffer *buffer) {
    for (int i = 0; i < buffer->len; ++i) {
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02X", buffer->bytes[i]);
    }
    printf("\n");
}
