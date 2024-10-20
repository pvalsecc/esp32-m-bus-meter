#pragma once

#include <esp_zigbee_type.h>
#include <zcl/esp_zigbee_zcl_command.h>

#define LIGHT_ENDPOINT_ID 11

void zigbee_light_create_ep(esp_zb_ep_list_t *epList);
esp_err_t zigbee_light_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message);
