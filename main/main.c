#include "iot_button.h"
#include "led.h"
#include "uart.h"
#include "zigbee.h"
#include "zigbee_meter.h"
#include <esp_log.h>
#include <hal/efuse_hal.h>
#include <nvs_flash.h>
#include <soc/gpio_num.h>
#include <stdio.h>

static const char *TAG = "main";

static void long_press(void *arg, void *usr_data) { zigbee_reset_pairing(); }

static int16_t curValue = 0;

static void single_click(void *arg, void *usr_data) {
    static int values[7] = {};
    int newValue = ++values[curValue];
    if (curValue == 0) {
        ESP_LOGI(TAG, "Set power to %d", newValue);
        zigbee_meter_update_active_power(newValue);
    } else if (curValue >= 1 && curValue <= 3) {
        zigbee_meter_update_rms_current(curValue - 1, newValue);
    } else if (curValue >= 4 && curValue <= 6) {
        zigbee_meter_update_rms_voltage(curValue - 4, newValue);
    } else if (curValue == 7) {
        zigbee_meter_update_summation_received(newValue);
    } else if (curValue == 8) {
        zigbee_meter_update_summation_delivered(newValue);
    }
}

static void double_click(void *arg, void *usr_data) {
    curValue = (curValue + 1) % 9;
    ESP_LOGI(TAG, "Switch to value %d", curValue + 1);
    single_click(arg, usr_data);
}

static void setup_button() {
    button_config_t gpio_btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config =
            {
                .gpio_num = GPIO_NUM_9,
            },
    };
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    if (NULL == gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }
    iot_button_register_cb(gpio_btn, BUTTON_LONG_PRESS_START, long_press, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, single_click, NULL);
    iot_button_register_cb(gpio_btn, BUTTON_DOUBLE_CLICK, double_click, NULL);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());

    printf("Chip revision: %lu.%lu\n", efuse_hal_get_major_chip_version(), efuse_hal_get_minor_chip_version());

    led_init();

    zigbee_init();

    uart_init();

    setup_button();
}
