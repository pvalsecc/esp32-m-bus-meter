#pragma once

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef HANDLE_ESCAPE

struct _hdlc_frame_state;

typedef void (*hdlc_frame_cb)(void *arg, const uint8_t *bytes, int size);

struct _hdlc_frame_state *hdlc_frame_init(hdlc_frame_cb cb, void *arg);

void hdlc_handle_byte(struct _hdlc_frame_state *state, uint8_t byte);

#ifdef __cplusplus
}
#endif
