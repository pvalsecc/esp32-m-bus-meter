#pragma once

#include <stdio.h>

#define ESP_LOGW(TAG, MESSAGE, ...) printf(MESSAGE, ##__VA_ARGS__)
