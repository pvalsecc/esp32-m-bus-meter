#include "led.h"
#include <esp_log.h>
#include <led_strip.h>

static const char *TAG = "led";
static led_strip_handle_t s_led_strip;

void led_set(RgbColor color, uint16_t intensity) {
    intensity = intensity > 100 ? 100 : intensity;

    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, color.red * intensity / 100, color.green * intensity / 100,
                                        color.blue * intensity / 100));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
    ESP_LOGI(TAG, "Set LED color to %d/%d/%d i=%d", color.red, color.green, color.blue, intensity);
}

void led_init(void) {
    led_strip_config_t led_strip_conf = {
        .max_leds = 1,
        .strip_gpio_num = 8,
    };
    led_strip_rmt_config_t rmt_conf = {};
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_conf, &rmt_conf, &s_led_strip));
}
