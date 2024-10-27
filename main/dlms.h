#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dlms_state;
typedef void (*dlms_handler)(void *arg, const uint8_t *bytes, int size);

struct dlms_state *dlms_init(dlms_handler cb, void *arg);
bool dlms_decode(struct dlms_state *state, const uint8_t *bytes, int size);

#ifdef __cplusplus
}
#endif
