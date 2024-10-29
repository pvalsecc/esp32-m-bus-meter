#pragma once

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define METERING_ENDPOINT_ID 10
#define ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID 11

void zigbee_meter_update_active_power(int16_t powerWatts);
void zigbee_meter_update_rms_current(int phase, uint16_t currentAmps);
void zigbee_meter_update_rms_voltage(int phase, uint16_t voltageVolts);
void zigbee_meter_update_summation_received(uint64_t energy);
void zigbee_meter_update_summation_delivered(uint64_t energy);
void zigbee_meter_update_tier_summation_received(int tier, uint64_t energy);
void zigbee_meter_update_tier_summation_delivered(int tier, uint64_t energy);

#ifdef __cplusplus
}
#endif
