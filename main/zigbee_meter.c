#include "zigbee_meter.h"
#include "led.h"
#include "zigbee_utils.h"
#include <esp_zigbee_attribute.h>
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_endpoint.h>
#include <zcl/esp_zigbee_zcl_common.h>

static const char *TAG = "zigbee_meter";

// Table 4-28. From
// https://zigbeealliance.org/wp-content/uploads/2019/12/07-5123-06-zigbee-cluster-library-specification.pdf
typedef enum {
    ACTIVE_MEASUREMENT,
    REACTIVE_MEASUREMENT,
    APPARENT_MEASUREMENT,
    PHASE_A_MEASUREMENT,
    PHASE_B_MEASUREMENT,
    PHASE_C_MEASUREMENT,
    DC_MEASUREMENT,
    HARMONICS_MEASUREMENT,
    POWER_QUALITY__MEASUREMENT
} MeasurementType;

void zigbee_meter_create_ep(esp_zb_ep_list_t *epList) {
    // Home assistant doesn't support phb and phc attributes => one end point per phase
    for (int16_t phase = 0; phase < 3; ++phase) {
        esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();

        // electrical measurement cluster
        esp_zb_attribute_list_t *esp_zb_electrical_measurement_cluster =
            esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);

        uint32_t measurement_type = (1 << PHASE_A_MEASUREMENT);
        ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
            esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID,
            &measurement_type));
        int16_t active_power_a = 42 + phase; // Watts
        ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(esp_zb_electrical_measurement_cluster,
                                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID,
                                                                &active_power_a));

        ESP_ERROR_CHECK(esp_zb_cluster_list_add_electrical_meas_cluster(
            clusterList, esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

        esp_zb_endpoint_config_t onOffEpConfig = {
            .endpoint = METER_ENDPOINT_ID + phase,
            .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
            .app_device_id = ESP_ZB_HA_METER_INTERFACE_DEVICE_ID,
        };
        ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(epList, clusterList, onOffEpConfig));
    }
}

esp_err_t zigbee_meter_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
    ESP_LOGW(TAG, "Unknown attribute ep=METER cl=0x%x attr=0x%x", message->info.cluster, message->attribute.id);
    return ESP_FAIL;
}
