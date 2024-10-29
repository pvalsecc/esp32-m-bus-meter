#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Buffer;

bool pdu_decode(const struct Buffer *buffer);

#ifdef __cplusplus
}
#endif
