#include "led.h"
#include <esp_log.h>
#include <led_indicator.h>

static const char *TAG = "led";

static const blink_step_t double_red_blink[] = {
    {LED_BLINK_RGB, SET_RGB(255, 0, 0), 0},
    {LED_BLINK_HOLD, LED_STATE_ON, 500},
    {LED_BLINK_HOLD, LED_STATE_OFF, 500},
    {LED_BLINK_HOLD, LED_STATE_ON, 500},
    {LED_BLINK_RGB, SET_RGB(0, 255 * DEFAULT_LED_INTENSITY / 100, 0), 0},
    {LED_BLINK_STOP, 0, 0},
};

static blink_step_t const *led_mode[] = {
    [BLINK_IDENTIFY] = double_red_blink,
    [BLINK_MAX] = NULL,
};

static led_indicator_handle_t led_handle;

void led_set(RgbColor color, uint16_t intensity) {
    intensity = intensity > 100 ? 100 : intensity;
    ESP_ERROR_CHECK(led_indicator_set_rgb(
        led_handle, SET_RGB(color.red * intensity / 100, color.green * intensity / 100, color.blue * intensity / 100)));
    ESP_LOGI(TAG, "Set LED color to %d/%d/%d i=%d", color.red, color.green, color.blue, intensity);
}

void led_blink(BlinkType type) { ESP_ERROR_CHECK(led_indicator_start(led_handle, type)); }

void led_init(void) {
    led_strip_config_t strip_config = {
        .strip_gpio_num = 8,                      // The GPIO that connected to the LED strip's data line
        .max_leds = 1,                            // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
        .resolution_hz = 0,             // RMT counter clock frequency
        .flags.with_dma = false,        // DMA feature is available on ESP target like ESP32-S3
    };

    led_indicator_strips_config_t strips_config = {
        .led_strip_cfg = strip_config,
        .led_strip_driver = LED_STRIP_RMT,
        .led_strip_rmt_cfg = rmt_config,
    };

    const led_indicator_config_t config = {
        .mode = LED_STRIPS_MODE,
        .led_indicator_strips_config = &strips_config,
        .blink_lists = led_mode,
        .blink_list_num = BLINK_MAX,
    };

    led_handle = led_indicator_create(&config);
    assert(led_handle != NULL);
}
