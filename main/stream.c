#include "stream.h"
#include "buffer.h"
#include <esp_log.h>
#include <memory.h>

static const char *TAG = "stream";

void stream_reset(Stream *stream, const Buffer *buffer) {
    stream->buffer = buffer;
    stream->pos = 0;
}

bool stream_get(Stream *stream, uint8_t *value, int len) {
    if (stream == NULL || value == NULL || stream->pos + len > stream->buffer->len) {
        return false;
    }
    memcpy(value, stream->buffer->bytes + stream->pos, len);
    stream->pos += len;
    return true;
}

bool stream_get_buffer(Stream *stream, Buffer *value, int len) {
    if (stream == NULL || value == NULL || stream->pos + len > stream->buffer->len) {
        return false;
    }
    buffer_add_bytes(value, stream->buffer->bytes + stream->pos, len);
    stream->pos += len;
    return true;
}

bool stream_getu8(Stream *stream, uint8_t *value) {
    if (stream == NULL || value == NULL || stream->pos + 1 > stream->buffer->len) {
        return false;
    }
    *value = stream->buffer->bytes[stream->pos++];
    return true;
}

bool stream_get8(Stream *stream, int8_t *value) { return stream_getu8(stream, (uint8_t *)value); }

bool stream_getu16_be(Stream *stream, uint16_t *value) {
    if (stream == NULL || value == NULL || stream->pos + 2 > stream->buffer->len) {
        return false;
    }
    *value = (stream->buffer->bytes[stream->pos] << 8) | stream->buffer->bytes[stream->pos + 1];
    stream->pos += 2;
    return true;
}

bool stream_getu32_be(Stream *stream, uint32_t *value) {
    if (stream == NULL || value == NULL || stream->pos + 4 > stream->buffer->len) {
        return false;
    }
    *value = (stream->buffer->bytes[stream->pos] << 24) | (stream->buffer->bytes[stream->pos + 1] << 16) |
             (stream->buffer->bytes[stream->pos + 2] << 8) | stream->buffer->bytes[stream->pos + 3];
    stream->pos += 4;
    return true;
}

bool stream_getu16_le(Stream *stream, uint16_t *value) {
    if (stream == NULL || value == NULL || stream->pos + 2 > stream->buffer->len) {
        return false;
    }
    *value = (stream->buffer->bytes[stream->pos + 1] << 8) | stream->buffer->bytes[stream->pos];
    stream->pos += 2;
    return true;
}

bool stream_skip(Stream *stream, int nb) {
    if (stream == NULL || stream->pos + nb > stream->buffer->len) {
        return false;
    }
    stream->pos += nb;
    return true;
}

int stream_remains(const Stream *stream) { return stream->buffer->len - stream->pos; }
