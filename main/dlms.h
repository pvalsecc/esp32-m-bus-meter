#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dlms_state;
struct Buffer;
typedef void (*dlms_handler)(void *arg, const struct Buffer *buffer);

struct dlms_state *dlms_init(dlms_handler cb, void *arg);
bool dlms_decode(struct dlms_state *state, const struct Buffer *buffer);

#ifdef __cplusplus
}
#endif
