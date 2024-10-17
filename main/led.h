#pragma once

#include <inttypes.h>

typedef enum {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  LIGHT_BLUE,
  MAGENTA,
  WHITE,
  NB_COLORS
} Color;

#define DEFAULT_LED_INTENSITY 10

void led_init(void);
void led_set(Color color, uint16_t intensity);
