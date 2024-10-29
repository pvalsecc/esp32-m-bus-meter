#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Buffer;

typedef struct Stream {
    const struct Buffer *buffer;
    int pos;
} Stream;

void stream_reset(Stream *stream, const struct Buffer *buffer);

bool stream_get(Stream *stream, uint8_t *value, int len);
bool stream_get_buffer(Stream *stream, struct Buffer *value, int len);

bool stream_getu8(Stream *stream, uint8_t *value);
bool stream_get8(Stream *stream, int8_t *value);
bool stream_getu16_be(Stream *stream, uint16_t *value);
bool stream_getu32_be(Stream *stream, uint32_t *value);

bool stream_getu16_le(Stream *stream, uint16_t *value);

bool stream_skip(Stream *stream, int nb);

int stream_remains(const Stream *stream);

#ifdef __cplusplus
}
#endif
