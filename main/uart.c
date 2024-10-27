#include "uart.h"
#include "hdlc_fields.h"
#include "hdlc_frame.h"
#include "hdlc_packet.h"
#include <driver/gpio.h>
#include <driver/uart.h>
#include <sys/cdefs.h>

static const char *TAG = "uart";
#define UART_PORT UART_NUM_1
#define UART_TX_PIN UART_PIN_NO_CHANGE // 8
#define UART_RX_PIN 10
#define UART_BUF_SIZE 1024
static const uart_config_t uart_config = {
    .baud_rate = 2400,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_ODD,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
};

static void hdlc_frame_received(void *arg, const uint8_t *bytes, int size) {
    Information information = hdlc_packet_decode(bytes, size);
    printf("Got frame:");
    for (int i = 0; i < information.size; ++i) {
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02X", information.bytes[i]);
    }
    printf("\n");
}

_Noreturn void uart_task(void *arg) {
    uint8_t *data = (uint8_t *)malloc(UART_BUF_SIZE);
    struct _hdlc_frame_state *frameState = hdlc_frame_init(hdlc_frame_received, NULL);
    while (true) {
        int len = uart_read_bytes(UART_PORT, data, 1, portMAX_DELAY);
        for (int i = 0; i < len; ++i) {
            hdlc_handle_byte(frameState, data[i]);
        }
    }
}

void uart_init() {
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUF_SIZE * 2, 0, 0, NULL, 0));

    xTaskCreate(uart_task, "uart_task", 4096, NULL, 10, NULL);
}
