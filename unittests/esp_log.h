#pragma once

#include <stdio.h>

#define ESP_LOGW(TAG, MESSAGE, ...) printf("W " MESSAGE "\n", ##__VA_ARGS__)
#define ESP_LOGI(TAG, MESSAGE, ...) printf("I " MESSAGE "\n", ##__VA_ARGS__)
