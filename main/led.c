#include "led.h"
#include <esp_log.h>
#include <led_strip.h>

static const char *TAG = "led";
static led_strip_handle_t s_led_strip;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RgbColor;

static const RgbColor COLORS[] = {
    {
        // RED
        .red = 255,
        .green = 0,
        .blue = 0,
    },
    {
        // GREEN
        .red = 0,
        .green = 255,
        .blue = 0,
    },
    {
        // BLUE
        .red = 0,
        .green = 0,
        .blue = 255,
    },
    {
        // YELLOW
        .red = 255,
        .green = 255,
        .blue = 0,
    },
    {
        // LIGHT_BLUE
        .red = 0,
        .green = 255,
        .blue = 255,
    },
    {
        // MAGENTA
        .red = 255,
        .green = 0,
        .blue = 255,
    },
    {
        // WHITE
        .red = 255,
        .green = 255,
        .blue = 255,
    },
};

void led_set(Color color, uint16_t intensity) {
    if (color >= NB_COLORS) {
        ESP_LOGE(TAG, "Invalid color %d", color);
        return;
    }

    intensity = intensity > 100 ? 100 : intensity;

    const RgbColor *new_color = &COLORS[color];

    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, new_color->red * intensity / 100,
                                        new_color->green * intensity / 100, new_color->blue * intensity / 100));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
    ESP_LOGI(TAG, "Set LED color to %d/%d/%d i=%d", new_color->red, new_color->green, new_color->blue, intensity);
}

void led_init(void) {
    led_strip_config_t led_strip_conf = {
        .max_leds = 1,
        .strip_gpio_num = 8,
    };
    led_strip_rmt_config_t rmt_conf = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_conf, &rmt_conf, &s_led_strip));
    led_set(YELLOW, DEFAULT_LED_INTENSITY);
}
