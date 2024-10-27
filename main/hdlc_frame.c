#include "hdlc_frame.h"
#include <esp_log.h>
#include <stdlib.h>

static const char *TAG = "hdlc_frame";
typedef enum { WAIT_SYNC, DATA, ESCAPE } State;
static const int FLAG_BYTE = 0x7E;
static const int ESCAPE_BYTE = 0x7D;

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
        if (byte == FLAG_BYTE) {
            // got a start byte
            state->pos = 0;
            state->state = DATA;
        }
        break;
    case DATA:
        if (byte == FLAG_BYTE) {
            // got the end
            if (state->pos > 0) {
                state->cb(state->arg, state->buffer, state->pos);
            }
            state->pos = 0;
            state->state = DATA;
        } else if (state->pos > sizeof(state->buffer)) {
            ESP_LOGW(TAG, "Buffer overflow, ignoring a frame");
            state->pos = 0;
            state->state = WAIT_SYNC;
        } else if (byte == ESCAPE_BYTE) {
            state->state = ESCAPE;
        } else {
            state->buffer[state->pos] = byte;
            state->pos++;
        }
        break;
    case ESCAPE: {
        uint8_t actual = byte ^ 0x20; // bit 5 inverted
        if (actual == FLAG_BYTE || actual == ESCAPE_BYTE) {
            state->buffer[state->pos] = actual;
            state->pos++;
        } else {
            // Landis&Gyr are too stupid to respect the norm...
            state->buffer[state->pos] = ESCAPE_BYTE;
            state->pos++;
            if (state->pos > sizeof(state->buffer)) {
                ESP_LOGW(TAG, "Buffer overflow, ignoring a frame");
                state->pos = 0;
                state->state = WAIT_SYNC;
            } else {
                state->buffer[state->pos] = byte;
                state->pos++;
            }
        }
        state->state = DATA;
        break;
    }
    }
}
