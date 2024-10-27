#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool pdu_decode(const uint8_t *bytes, int size);

#ifdef __cplusplus
}
#endif
