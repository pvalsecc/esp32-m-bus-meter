#include "iot_button.h"
#include "led.h"
#include "zigbee.h"
#include <esp_log.h>
#include <nvs_flash.h>
#include <soc/gpio_num.h>
#include <stdio.h>

static const char *TAG = "main";
static void long_press(void *arg, void *usr_data) { zigbee_reset_pairing(); }

static void setup_button() {
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config =
            {
                .gpio_num = GPIO_NUM_9,
                .active_level = 0,
            },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    if (NULL == gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }
    iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, long_press, NULL);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());

    led_init();

    zigbee_init();

    setup_button();
}
