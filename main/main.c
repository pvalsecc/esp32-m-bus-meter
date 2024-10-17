#include "led.h"
#include "zigbee.h"
#include <nvs_flash.h>
#include <stdio.h>

void app_main(void)
{
  printf("Hello world\n");
  ESP_ERROR_CHECK(nvs_flash_init());
  led_init();
  zigbee_init();
}
