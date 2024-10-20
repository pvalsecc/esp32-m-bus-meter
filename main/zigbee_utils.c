//
// Created by patrick on 20.10.24.
//

#include "zigbee_utils.h"
#include "led.h"
#include <esp_check.h>
#include <esp_zigbee_core.h>
#include <string.h>

#define BASIC_MANUFACTURER_NAME "Patrick Valsecchi"

static const char *TAG = "zigbee_utils";

void add_txt_attr(struct esp_zb_attribute_list_s *cluster, __uint16_t attrId, const char *value) {
    unsigned int len = strlen(value);
    if (len >= 255) {
        ESP_LOGE(TAG, "String too long: %s", value);
    }
    char *encoded = (char *)malloc(len + 1);
    encoded[0] = len;
    memcpy(encoded + 1, value, len);
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(cluster, attrId, encoded));
    free(encoded);
}

void zigbee_create_indentify_cluster(struct esp_zb_cluster_list_s *clusterList) { // identify cluster
    struct esp_zb_attribute_list_s *esp_zb_identify_cluster =
        esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
    __uint16_t identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;
    ESP_ERROR_CHECK(esp_zb_identify_cluster_add_attr(esp_zb_identify_cluster, ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID,
                                                     &identify_time));
    ESP_ERROR_CHECK(
        esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    // identify client cluster
    struct esp_zb_attribute_list_s *esp_zb_identify_client_cluster =
        esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(clusterList, esp_zb_identify_client_cluster,
                                                             ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));
}

void zigbee_create_basic_cluster(struct esp_zb_cluster_list_s *clusterList, const char *modelName) {
    struct esp_zb_attribute_list_s *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);

    __uint8_t zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE;
    ESP_ERROR_CHECK(
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version));

    __uint8_t application_version = ESP_ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID,
                                                  &application_version));

    __uint8_t stack_version = ESP_ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE;
    ESP_ERROR_CHECK(
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, &stack_version));

    __uint8_t hw_version = ESP_ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE;
    ESP_ERROR_CHECK(
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, &hw_version));

    add_txt_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, BASIC_MANUFACTURER_NAME);
    add_txt_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, modelName);

    __uint8_t power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE;
    ESP_ERROR_CHECK(
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &power_source));

    ESP_ERROR_CHECK(
        esp_zb_cluster_list_add_basic_cluster(clusterList, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
}
