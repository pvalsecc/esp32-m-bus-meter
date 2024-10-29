#include "uart.h"
#include "buffer.h"
#include "dlms.h"
#include "hdlc_frame.h"
#include "hdlc_packet.h"
#include "pdu.h"
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_log.h>

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

static void hdlc_frame_received(void *arg, const Buffer *buffer) {
    struct dlms_state *dlmsState = (struct dlms_state *)arg;
    Buffer information;
    if (!hdlc_packet_decode(buffer, &information)) {
        ESP_LOGW(TAG, "Failed parsing this HDLC frame:");
        buffer_dump(buffer);
        return;
    }
    if (!dlms_decode(dlmsState, &information)) {
        ESP_LOGW(TAG, "Failed parsing this HDLC information:");
        buffer_dump(&information);
    }
}

static void dlms_frame_received(void *arg, const Buffer *buffer) {
    if (!pdu_decode(buffer)) {
        ESP_LOGW(TAG, "Failed parsing DLMS packet:");
        buffer_dump(buffer);
    }
}

_Noreturn void uart_task(void *arg) {
    uint8_t *data = (uint8_t *)malloc(UART_BUF_SIZE);
    struct dlms_state *dlmsState = dlms_init(dlms_frame_received, NULL);
    struct hdlc_frame_state *frameState = hdlc_frame_init(hdlc_frame_received, dlmsState);
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

    xTaskCreate(uart_task, "uart_task", 8192, NULL, 10, NULL);
}
