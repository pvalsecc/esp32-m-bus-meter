#pragma once

#include <esp_zigbee_type.h>

#define TEMPERATURE_ENDPOINT_ID 30

void zigbee_temperature_create_ep(esp_zb_ep_list_t *epList);
