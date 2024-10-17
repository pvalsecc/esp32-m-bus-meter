#include "button.h"
#include "led.h"
#include "zigbee.h"
#include <nvs_flash.h>
#include <stdio.h>

void app_main(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  led_init();
  zigbee_init();
  button_init();
}
