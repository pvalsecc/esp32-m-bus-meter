#pragma once

#include <esp_zigbee_type.h>
#include <zcl/esp_zigbee_zcl_command.h>

#ifdef __cplusplus
extern "C" {
#endif

#define METERING_ENDPOINT_ID 10
#define ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID 11

void zigbee_meter_create_ep(esp_zb_ep_list_t *epList);
esp_err_t zigbee_meter_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message);

#ifdef __cplusplus
}
#endif
