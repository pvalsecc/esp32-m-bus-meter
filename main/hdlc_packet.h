#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Buffer;

bool hdlc_packet_decode(const struct Buffer *packet, struct Buffer *information);

#ifdef __cplusplus
}
#endif
