#include "button.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/task.h>

static const char *TAG = "button";
#define LONG_PRESS_US (1000000L)

typedef struct {
  bool pressed;
  int64_t timestampUs;
  ButtonState *buttonState;
} Message;

QueueHandle_t evtQueue;

static void button_isr_handler(void *arg) {
  ButtonState *buttonState = (ButtonState *)arg;
  gpio_get_level(GPIO_NUM_9);
  Message message = {
      .pressed = !gpio_get_level(GPIO_NUM_9), .timestampUs = esp_timer_get_time(), .buttonState = buttonState};
  xQueueSendFromISR(evtQueue, &message, NULL);
}

_Noreturn static void button_task(void *arg) {
  Message message;
  while (true) {
    if (xQueueReceive(evtQueue, &message, portMAX_DELAY)) {
      if (message.pressed) {
        message.buttonState->pressedTimeUs = message.timestampUs;
      } else {
        if ((message.timestampUs - message.buttonState->pressedTimeUs) > LONG_PRESS_US) {
          ESP_LOGI(TAG, "Long click");
          if (message.buttonState->longCb) {
            message.buttonState->longCb(message.buttonState->arg);
          }
        } else {
          ESP_LOGI(TAG, "Short click");
          if (message.buttonState->shortCb) {
            message.buttonState->shortCb(message.buttonState->arg);
          }
        }
      }
    }
  }
}

void button_init() {
  evtQueue = xQueueCreate(10, sizeof(Message));
  xTaskCreate(button_task, "button_task", 4096, NULL, 10, NULL);
  // install gpio isr service
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
}

void button_add(ButtonState *buttonState, uint32_t gpio, ButtonCb shortCb, ButtonCb longCb, void *arg) {
  buttonState->pressedTimeUs = 0L;
  buttonState->shortCb = shortCb;
  buttonState->longCb = longCb;
  buttonState->arg = arg;

  gpio_config_t io_conf = {.pin_bit_mask = (1 << gpio),
                           .intr_type = GPIO_INTR_ANYEDGE,
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = GPIO_PULLUP_ENABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE};
  ESP_ERROR_CHECK(gpio_config(&io_conf));

  // hook isr handler for specific gpio pin
  ESP_ERROR_CHECK(gpio_isr_handler_add(gpio, button_isr_handler, buttonState));
}
