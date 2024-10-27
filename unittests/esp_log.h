#pragma once

#include <stdio.h>

#define ESP_LOGW(TAG, MESSAGE, ...) printf(MESSAGE "\n", ##__VA_ARGS__)
