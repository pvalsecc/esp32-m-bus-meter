#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 1024

typedef struct Buffer {
    uint8_t bytes[BUFFER_SIZE];
    int len;
} Buffer;

void buffer_reset(Buffer *buffer);
bool buffer_add_byte(Buffer *buffer, uint8_t byte);
bool buffer_add_bytes(Buffer *buffer, const uint8_t *bytes, int len);
void buffer_dump(const Buffer *buffer);

#ifdef __cplusplus
}
#endif
