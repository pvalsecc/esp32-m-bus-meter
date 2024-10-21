#include "zigbee_temperature.h"
#include "esp_zigbee_core.h"
#include <driver/temperature_sensor.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_endpoint.h>
#include <zcl/esp_zigbee_zcl_common.h>

static const char *TAG = "zigbee_temperature";

static temperature_sensor_handle_t temp_sensor = NULL;

static void zigbee_temperature_update() {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_report_attr_cmd_t cmdReq = {.zcl_basic_cmd.src_endpoint = TEMPERATURE_ENDPOINT_ID,
                                           .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
                                           .clusterID = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                                           .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                                           .attributeID = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID};

    float temperature;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &temperature));
    ESP_LOGI(TAG, "Got temperature: %.02f°C", temperature);
    int16_t temperatureInt = temperature * 100.0f;

    esp_zb_zcl_status_t state =
        esp_zb_zcl_set_attribute_val(cmdReq.zcl_basic_cmd.src_endpoint, cmdReq.clusterID, cmdReq.cluster_role,
                                     cmdReq.attributeID, &temperatureInt, false);

    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        esp_zb_lock_release();
        ESP_LOGE(TAG, "Setting temperature attribute failed: 0x%03x", state);
        return;
    }

    state = esp_zb_zcl_report_attr_cmd_req(&cmdReq);
    esp_zb_lock_release();

    /* Check for error */
    if (state != ESP_ZB_ZCL_STATUS_SUCCESS) {
        ESP_LOGE(TAG, "Sending temperature attribute report command failed: 0x%03x", state);
        return;
    }
}

static void temperature_cb(void *arg) { zigbee_temperature_update(); }

void zigbee_temperature_create_ep(esp_zb_ep_list_t *epList) {
    esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();

    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    float tsens_value;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
    ESP_LOGI(TAG, "Got temperature: %.02f°C", tsens_value);

    esp_zb_temperature_meas_cluster_cfg_t config = {.measured_value = tsens_value * 100.0f,
                                                    .min_value = temp_sensor_config.range_min * 100,
                                                    .max_value = temp_sensor_config.range_max * 100};
    esp_zb_attribute_list_t *cluster = esp_zb_temperature_meas_cluster_create(&config);
    ESP_ERROR_CHECK(
        esp_zb_cluster_list_add_temperature_meas_cluster(clusterList, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    esp_zb_endpoint_config_t epConfig = {
        .endpoint = TEMPERATURE_ENDPOINT_ID,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID,
    };
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(epList, clusterList, epConfig));

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &temperature_cb,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic"};

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 60 * 1000 * 1000));
}
