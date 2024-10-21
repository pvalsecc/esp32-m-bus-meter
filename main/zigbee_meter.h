#pragma once

#include <esp_zigbee_type.h>
#include <zcl/esp_zigbee_zcl_command.h>

#define METERING_ENDPOINT_ID 10
#define ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID 11

void zigbee_meter_create_ep(esp_zb_ep_list_t *epList);
esp_err_t zigbee_meter_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message);
void zigbee_meter_update_active_power(int16_t powerWatts);
void zigbee_meter_update_rms_current(int phase, uint16_t currentAmps);
void zigbee_meter_update_rms_voltage(int phase, uint16_t voltageVolts);
void zigbee_meter_update_summation_received(uint64_t energy);
void zigbee_meter_update_summation_delivered(uint64_t energy);
