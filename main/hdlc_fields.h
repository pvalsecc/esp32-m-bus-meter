#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool hdlc_decode_type_length(uint8_t *bytes, int len, int *pos, uint8_t *outType, bool *outSegmentation,
                             uint16_t *outLength);

#ifdef __cplusplus
}
#endif
