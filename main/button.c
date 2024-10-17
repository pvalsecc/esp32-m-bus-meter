#include "button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "zigbee.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <sys/cdefs.h>

static uint32_t PRESSED = 1;
static uint32_t RELEASED = 0;
static uint32_t GPIO = GPIO_NUM_9;
static const char *TAG = "button";
static QueueHandle_t evtQueue = NULL;
static int64_t pressedTimeUs = 0L;
#define LONG_PRESS_US (1000000L)

static void button_isr_handler(void *arg) {
  gpio_get_level(GPIO_NUM_9);
  xQueueSendFromISR(evtQueue, gpio_get_level(GPIO_NUM_9) ? &RELEASED : &PRESSED, NULL);
}

_Noreturn static void button_task(void *arg) {
  uint32_t state;
  while (true) {
    if (xQueueReceive(evtQueue, &state, portMAX_DELAY)) {
      bool pressed = state == PRESSED;
      if (pressed) {
        ESP_LOGI(TAG, "Pressed");
        pressedTimeUs = esp_timer_get_time();
      } else {
        if ((esp_timer_get_time() - pressedTimeUs) > LONG_PRESS_US) {
          ESP_LOGI(TAG, "Long click");
          zigbee_reset_pairing();
        } else {
          ESP_LOGI(TAG, "Short click");
        }
      }
    }
  }
}

void button_init(void) {
  gpio_config_t io_conf = {.pin_bit_mask = (1 << GPIO),
                           .intr_type = GPIO_INTR_ANYEDGE,
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = GPIO_PULLUP_ENABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE};
  ESP_ERROR_CHECK(gpio_config(&io_conf));

  evtQueue = xQueueCreate(10, sizeof(uint32_t));
  xTaskCreate(button_task, "gpio_task", 4096, NULL, 10, NULL);

  // install gpio isr service
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  // hook isr handler for specific gpio pin
  ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO, button_isr_handler, NULL));
}
