#include "hdlc_frame.h"
#include <esp_log.h>
#include <stdlib.h>

static const char *TAG = "hdlc_frame";
typedef enum { WAIT_SYNC, DATA, ESCAPE } State;

typedef struct _hdlc_frame_state {
    State state;
    uint8_t buffer[256];
    int pos;
    hdlc_frame_cb cb;
    void *arg;
} hdlc_frame_state;

hdlc_frame_state *hdlc_frame_init(hdlc_frame_cb cb, void *arg) {
    hdlc_frame_state *state = malloc(sizeof(hdlc_frame_state));
    state->state = WAIT_SYNC;
    state->pos = 0;
    state->cb = cb;
    state->arg = arg;
    return state;
}

void hdlc_handle_byte(struct _hdlc_frame_state *state, uint8_t byte) {
    switch (state->state) {
    case WAIT_SYNC:
        if (byte == 0x7E) {
            // got a start byte
            state->state = DATA;
            state->pos = 0;
        }
        break;
    case DATA:
        if (byte == 0x7E) {
            // got the end
            if (state->pos > 0) {
                state->cb(state->arg, state->buffer, state->pos);
            }
            state->pos = 0;
            state->state = DATA;
        } else if (state->pos > sizeof(state->buffer)) {
            ESP_LOGW(TAG, "Buffer overflow, ignoring the frame");
            state->pos = 0;
            state->state = WAIT_SYNC;
        } else if (byte == 0x7D) {
            state->state = ESCAPE;
        } else {
            state->buffer[state->pos] = byte;
            state->pos++;
        }
        break;
    case ESCAPE:
        state->buffer[state->pos] = byte ^ 0x20;
        state->pos++;
        state->state = DATA;
        break;
    }
}
