#include "hdlc_frame.h"
#include "buffer.h"
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

typedef struct hdlc_frame_state {
    State state;
    Buffer buffer;
    int expectedLen;
    hdlc_frame_cb cb;
    void *arg;
} hdlc_frame_state;

hdlc_frame_state *hdlc_frame_init(hdlc_frame_cb cb, void *arg) {
    hdlc_frame_state *state = malloc(sizeof(hdlc_frame_state));
    state->state = WAIT_SYNC;
    buffer_reset(&state->buffer);
    state->cb = cb;
    state->arg = arg;
    return state;
}

void hdlc_handle_byte(hdlc_frame_state *state, uint8_t byte) {
    switch (state->state) {
    case WAIT_SYNC:
        if (byte == FLAG_BYTE) {
            // got a start byte
            buffer_reset(&state->buffer);
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
            buffer_add_byte(&state->buffer, byte);
        } else {
            // false start
            state->state = WAIT_SYNC;
        }
        break;
    case WAIT_LEN:
        state->expectedLen |= byte;
        state->state = DATA;
        buffer_add_byte(&state->buffer, byte);
        break;
    case DATA:
        if (byte == FLAG_BYTE && state->buffer.len == state->expectedLen) {
            // got the end
            if (state->buffer.len > 0) {
                state->cb(state->arg, &state->buffer);
            }
            buffer_reset(&state->buffer);
            state->expectedLen = 0;
            state->state = WAIT_TYPE;
        } else if (state->buffer.len >= state->expectedLen) {
            ESP_LOGW(TAG, "Got more bytes than expected");
            buffer_reset(&state->buffer);
            state->state = WAIT_SYNC;
#ifdef HANDLE_ESCAPE
        } else if (byte == ESCAPE_BYTE) {
            state->state = ESCAPE;
#endif
        } else {
            if (!buffer_add_byte(&state->buffer, byte)) {
                ESP_LOGW(TAG, "Buffer overflow, ignoring a frame");
                buffer_reset(&state->buffer);
                state->state = WAIT_SYNC;
            }
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
                buffer_reset(&state->buffer);
                state->state = WAIT_SYNC;
            } else {
                buffer_add_byte(&state->buffer, byte);
            }
        }
        state->state = DATA;
        break;
    }
#endif
    }
}
