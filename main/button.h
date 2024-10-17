#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <inttypes.h>

typedef void (*ButtonCb)(void *);

typedef struct {
    int64_t pressedTimeUs;
    ButtonCb shortCb;
    ButtonCb longCb;
    void *arg;
} ButtonState;

void button_init();
void button_add(ButtonState *buttonState, uint32_t gpio, ButtonCb shortCb, ButtonCb longCb, void *arg);
