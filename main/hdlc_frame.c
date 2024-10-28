#include "hdlc_frame.h"
#include <esp_log.h>
#include <stdlib.h>

static const char *TAG = "hdlc_frame";
typedef enum {
    WAIT_SYNC,
    WAIT_TYPE,
    WAIT_LEN,
    DATA,
#ifdef HANDLE_ESCAPE
    ESCAPE
#endif
} State;
static const int FLAG_BYTE = 0x7E;
#ifdef HANDLE_ESCAPE
static const int ESCAPE_BYTE = 0x7D;
#endif

typedef struct _hdlc_frame_state {
    State state;
    uint8_t buffer[256];
    int pos;
    int expectedLen;
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
            state->expectedLen = 0;
            state->state = WAIT_TYPE;
        }
        break;
    case WAIT_TYPE:
        if (byte == 0x7E) {
            // ignore double start
        } else if ((byte & (uint8_t)0xF0) == 0xA0) {
            // we support only type 3 packets
            state->expectedLen = ((uint16_t)byte & (uint16_t)0x07) << 8;
            state->state = WAIT_LEN;
            state->buffer[state->pos] = byte;
            state->pos++;
        } else {
            // false start
            state->state = WAIT_SYNC;
        }
        break;
    case WAIT_LEN:
        state->expectedLen |= byte;
        state->state = DATA;
        state->buffer[state->pos] = byte;
        state->pos++;
        break;
    case DATA:
        if (byte == FLAG_BYTE && state->pos == state->expectedLen) {
            // got the end
            if (state->pos > 0) {
                state->cb(state->arg, state->buffer, state->pos);
            }
            state->pos = 0;
            state->expectedLen = 0;
            state->state = WAIT_TYPE;
        } else if (state->pos >= state->expectedLen) {
            ESP_LOGW(TAG, "Got more bytes than expected");
            state->pos = 0;
            state->state = WAIT_SYNC;
        } else if (state->pos > sizeof(state->buffer)) {
            ESP_LOGW(TAG, "Buffer overflow, ignoring a frame");
            state->pos = 0;
            state->state = WAIT_SYNC;
#ifdef HANDLE_ESCAPE
        } else if (byte == ESCAPE_BYTE) {
            state->state = ESCAPE;
#endif
        } else {
            state->buffer[state->pos] = byte;
            state->pos++;
        }
        break;
#ifdef HANDLE_ESCAPE
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
#endif
    }
}
