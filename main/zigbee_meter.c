#include "zigbee_meter.h"
#include "esp_zigbee_core.h"
#include "zigbee_meter_values.h"
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

static void create_electrical_measurement_ep(esp_zb_ep_list_t *epList, int16_t phase) {
    esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();

    // electrical measurement cluster
    esp_zb_attribute_list_t *electricalMeasurementCluster =
        esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);

    uint32_t measurement_type = (1 << PHASE_A_MEASUREMENT);
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
        electricalMeasurementCluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, &measurement_type));

    if (phase == 0) {
        int16_t active_power_a = 0; // Watts
        ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
            electricalMeasurementCluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, &active_power_a));
    }

    uint16_t rmsVoltage = 235;
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
        electricalMeasurementCluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, &rmsVoltage));

    uint16_t rmsCurrent = 0;
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
        electricalMeasurementCluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, &rmsCurrent));

    // The current appears to be in centi amps
    uint16_t currentDivisor = 100;
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(
        electricalMeasurementCluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID, &currentDivisor));

    ESP_ERROR_CHECK(esp_zb_cluster_list_add_electrical_meas_cluster(clusterList, electricalMeasurementCluster,
                                                                    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    esp_zb_endpoint_config_t onOffEpConfig = {
        .endpoint = ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID + phase,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_METER_INTERFACE_DEVICE_ID,
    };
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(epList, clusterList, onOffEpConfig));
}

static void create_metering_ep(esp_zb_ep_list_t *epList) {
    esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();

    zigbee_create_basic_cluster(clusterList, "SmartMeter");
    zigbee_create_indentify_cluster(clusterList);

    esp_zb_attribute_list_t *meteringCluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_METERING);

    uint64_t currentSummationDelivered = 0;
    ESP_ERROR_CHECK(esp_zb_cluster_add_attr(
        meteringCluster, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U48, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        &currentSummationDelivered));

    uint64_t currentSummationReceived = 0;
    ESP_ERROR_CHECK(esp_zb_cluster_add_attr(
        meteringCluster, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U48, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING,
        &currentSummationReceived));

    ESP_ERROR_CHECK(
        esp_zb_cluster_list_add_metering_cluster(clusterList, meteringCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    esp_zb_endpoint_config_t onOffEpConfig = {
        .endpoint = METERING_ENDPOINT_ID,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_METER_INTERFACE_DEVICE_ID,
    };
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(epList, clusterList, onOffEpConfig));
}

void zigbee_meter_create_ep(esp_zb_ep_list_t *epList) {
    create_metering_ep(epList);

    // Home assistant doesn't support phb and phc attributes => one end point per phase
    for (int16_t phase = 0; phase < 3; ++phase) {
        create_electrical_measurement_ep(epList, phase);
    }
}

esp_err_t zigbee_meter_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
    ESP_LOGW(TAG, "Unknown attribute ep=METER cl=0x%x attr=0x%x", message->info.cluster, message->attribute.id);
    return ESP_FAIL;
}

void zigbee_meter_update_active_power(int16_t powerWatts) {
    ESP_LOGI(TAG, "Update the active power: %dW", powerWatts);
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_report_attr_cmd_t cmdReq = {.zcl_basic_cmd.src_endpoint = ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID,
                                           .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
                                           .clusterID = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                                           .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                                           .attributeID = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID};

    esp_zb_zcl_status_t state =
        esp_zb_zcl_set_attribute_val(cmdReq.zcl_basic_cmd.src_endpoint, cmdReq.clusterID, cmdReq.cluster_role,
                                     cmdReq.attributeID, &powerWatts, false);

    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        esp_zb_lock_release();
        ESP_LOGE(TAG, "Setting active power attribute failed: %d", state);
        return;
    }

    state = esp_zb_zcl_report_attr_cmd_req(&cmdReq);
    esp_zb_lock_release();

    /* Check for error */
    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Sending active power attribute report command failed: %d", state);
        return;
    }
    ESP_LOGI(TAG, "Done updating the active power");
}

static void updateElectricalMeasurementUint16Attr(uint8_t endpoint, uint16_t attrId, uint16_t value) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_report_attr_cmd_t cmdReq = {.zcl_basic_cmd.src_endpoint = endpoint,
                                           .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
                                           .clusterID = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                                           .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                                           .attributeID = attrId};

    esp_zb_zcl_status_t state = esp_zb_zcl_set_attribute_val(cmdReq.zcl_basic_cmd.src_endpoint, cmdReq.clusterID,
                                                             cmdReq.cluster_role, cmdReq.attributeID, &value, false);

    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        esp_zb_lock_release();
        ESP_LOGE(TAG, "Setting %04x electrical measurement attribute failed: %d", attrId, state);
        return;
    }

    state = esp_zb_zcl_report_attr_cmd_req(&cmdReq);
    esp_zb_lock_release();

    /* Check for error */
    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Sending %04x electrical measurement attribute report command failed: %d", attrId, state);
        return;
    }
}

void zigbee_meter_update_rms_current(int phase, uint16_t currentAmps) {
    // TODO: do we get 1/100 Amps from the meter?
    ESP_LOGI(TAG, "Update the RMS current on phase %d: %dA", phase + 1, currentAmps);
    updateElectricalMeasurementUint16Attr(ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID + phase,
                                          ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, currentAmps);
    ESP_LOGI(TAG, "Done updating the RMS current on phase %d", phase + 1);
}

void zigbee_meter_update_rms_voltage(int phase, uint16_t voltageVolts) {
    ESP_LOGI(TAG, "Update the RMS voltage on phase %d: %dV", phase + 1, voltageVolts);
    updateElectricalMeasurementUint16Attr(ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID + phase,
                                          ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, voltageVolts);
    ESP_LOGI(TAG, "Done updating the RMS voltage on phase %d", phase + 1);
}

static void updateMeteringUint64Attr(uint8_t endpoint, uint16_t attrId, uint64_t value) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_report_attr_cmd_t cmdReq = {.zcl_basic_cmd.src_endpoint = endpoint,
                                           .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
                                           .clusterID = ESP_ZB_ZCL_CLUSTER_ID_METERING,
                                           .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                                           .attributeID = attrId};

    esp_zb_zcl_status_t state = esp_zb_zcl_set_attribute_val(cmdReq.zcl_basic_cmd.src_endpoint, cmdReq.clusterID,
                                                             cmdReq.cluster_role, cmdReq.attributeID, &value, false);

    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        esp_zb_lock_release();
        ESP_LOGE(TAG, "Setting %04x metering attribute failed: %d", attrId, state);
        return;
    }

    state = esp_zb_zcl_report_attr_cmd_req(&cmdReq);
    esp_zb_lock_release();

    /* Check for error */
    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Sending %04x metering attribute report command failed: %d", attrId, state);
        return;
    }
}

void zigbee_meter_update_summation_received(uint64_t energy) {
    ESP_LOGI(TAG, "Update the summation received: %lluW/h", energy);
    updateMeteringUint64Attr(METERING_ENDPOINT_ID, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID, energy);
    ESP_LOGI(TAG, "Done updating the summation received");
}

void zigbee_meter_update_summation_delivered(uint64_t energy) {
    ESP_LOGI(TAG, "Update the summation delivered: %lluW/h", energy);
    updateMeteringUint64Attr(METERING_ENDPOINT_ID, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, energy);
    ESP_LOGI(TAG, "Done updating the summation delivered");
}
