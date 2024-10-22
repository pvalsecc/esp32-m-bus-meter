#include "uart.h"
#include "hdlc_frame.h"
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>
#include <strings.h>
#include <sys/cdefs.h>

static const char *TAG = "uart";
#define UART_PORT UART_NUM_1
#define UART_TX_PIN UART_PIN_NO_CHANGE // 8
#define UART_RX_PIN 22
#define UART_BUF_SIZE 1024
static const uart_config_t uart_config = {
    .baud_rate = 2400,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_ODD,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
};

static void hdlc_frame_received(void *arg, uint8_t *bytes, int size) {
    printf("Got HDLC frame:");
    for (int i = 0; i < size; ++i) {
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02X", bytes[i]);
    }
    printf("\n");
}

_Noreturn void uart_task(void *arg) {
    QueueHandle_t queue = arg;
    uint8_t *data = (uint8_t *)malloc(UART_BUF_SIZE);
    uart_event_t event;
    struct _hdlc_frame_state *frameState = hdlc_frame_init(hdlc_frame_received, NULL);
    while (true) {
        // Waiting for UART event.
        if (xQueueReceive(queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            bzero(data, UART_BUF_SIZE);
            switch (event.type) {
            case UART_DATA: {
                int len = uart_read_bytes(UART_PORT, data, event.size, portMAX_DELAY);
                if (len > 0) {
                    printf("Got data:");
                    for (int i = 0; i < len; ++i) {
                        if (i % 4 == 0) {
                            printf(" ");
                        }
                        printf("%02X", data[i]);
                        hdlc_handle_byte(frameState, data[i]);
                    }
                    printf("\n");
                }
                break;
            }
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
}

void uart_init() {
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // ESP_ERROR_CHECK(gpio_set_pull_mode(UART_RX_PIN, GPIO_PULLUP_ONLY));

    QueueHandle_t queue;
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 20, &queue, 0));

    xTaskCreate(uart_task, "uart_task", 4096, queue, 10, NULL);
}
