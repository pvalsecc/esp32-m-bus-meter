#include "button.h"
#include "led.h"
#include "zigbee.h"
#include <nvs_flash.h>
#include <soc/gpio_num.h>
#include <stdio.h>

static ButtonState buttonState;

static void app_long_press(void *arg) { zigbee_reset_pairing(); }

void app_main(void) {
  ESP_ERROR_CHECK(nvs_flash_init());

  led_init();

  zigbee_init();

  button_init();
  button_add(&buttonState, GPIO_NUM_9, NULL, app_long_press, NULL);
}
