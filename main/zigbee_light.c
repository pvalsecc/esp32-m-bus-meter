#include "zigbee_light.h"
#include "led.h"
#include "zigbee_utils.h"
#include <esp_zigbee_cluster.h>
#include <esp_zigbee_endpoint.h>
#include <zcl/esp_zigbee_zcl_common.h>

static const char *TAG = "zigbee_light";

void zigbee_light_create_ep(esp_zb_ep_list_t *epList) {
    esp_zb_cluster_list_t *clusterList = esp_zb_zcl_cluster_list_create();

    zigbee_create_basic_cluster(clusterList, "TestLight");

    // on-off cluster
    esp_zb_on_off_cluster_cfg_t onOffCfg = {.on_off = false};
    esp_zb_attribute_list_t *onOffCluster = esp_zb_on_off_cluster_create(&onOffCfg);
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_on_off_cluster(clusterList, onOffCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    esp_zb_endpoint_config_t onOffEpConfig = {
        .endpoint = LIGHT_ENDPOINT_ID,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID,
    };
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(epList, clusterList, onOffEpConfig));
}

esp_err_t zigbee_light_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
    switch (message->info.cluster) {
    case ESP_ZB_ZCL_CLUSTER_ID_ON_OFF:
        if (message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID &&
            message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
            bool light_state = message->attribute.data.value ? *(bool *)message->attribute.data.value : false;
            ESP_LOGI(TAG, "Light sets to %s", light_state ? "On" : "Off");
            led_set(light_state ? WHITE : CONNECTED_COLOR, light_state ? 100 : DEFAULT_LED_INTENSITY);
        } else {
            ESP_LOGW(TAG, "Unknown attribute ep=LIGHT cl=ON_OFF attr=0x%x", message->attribute.id);
        }
        return ESP_OK;
    default:
        ESP_LOGW(TAG, "Unknown attribute ep=LIGHT cl=0x%x attr=0x%x", message->info.cluster, message->attribute.id);
        return ESP_FAIL;
    }
}
