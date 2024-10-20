#pragma once

#include <inttypes.h>

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RgbColor;

#define RGB(R, G, B) ((RgbColor){.red = (R), .green = (G), .blue = (B)})
#define RED RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)
#define BLUE RGB(0, 0, 255)
#define YELLOW RGB(255, 255, 0)
#define LIGHT_BLUE RGB(0, 255, 255)
#define MAGENTA RGB(255, 0, 255)
#define WHITE RGB(255, 255, 255)

#define CONNECTING_COLOR BLUE
#define CONNECTED_COLOR GREEN
#define IDENTIFY_COLOR RED

#define DEFAULT_LED_INTENSITY 2

void led_init(void);
void led_set(RgbColor color, uint16_t intensity);
