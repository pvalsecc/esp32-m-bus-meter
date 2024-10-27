#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t *bytes;
    int size;
} Information;

Information hdlc_packet_decode(const uint8_t *bytes, int size);

#ifdef __cplusplus
}
#endif
