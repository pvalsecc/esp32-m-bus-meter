#include "zigbee.h"
#include "led.h"
#include "zigbee_light.h"
#include "zigbee_meter.h"
#include "zigbee_utils.h"
#include <esp_check.h>
#include <esp_zigbee_core.h>
#include <string.h>
#include <zb_config_platform.h>

static const char *TAG = "zigbee";

#define ED_AGING_TIMEOUT ESP_ZB_ED_AGING_TIMEOUT_64MIN
#define ED_KEEP_ALIVE 3000              /* 3000 millisecond */
#define INSTALLCODE_POLICY_ENABLE false /* enable the install code policy for security */

static esp_err_t deferred_driver_init(void) {
    led_set(CONNECTING_COLOR, DEFAULT_LED_INTENSITY);
    return ESP_OK;
}

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask) {
    ESP_RETURN_ON_FALSE(esp_zb_bdb_start_top_level_commissioning(mode_mask) == ESP_OK, , TAG,
                        "Failed to start Zigbee commissioning");
}

__attribute__((unused)) void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct) {
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;
    switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Initialize Zigbee stack");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK) {
            ESP_LOGI(TAG, "Deferred driver initialization %s", deferred_driver_init() ? "failed" : "successful");
            ESP_LOGI(TAG, "Device started up in %s factory-reset mode", esp_zb_bdb_is_factory_new() ? "" : "non");
            if (esp_zb_bdb_is_factory_new()) {
                ESP_LOGI(TAG, "Start network steering");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            } else {
                ESP_LOGI(TAG, "Device rebooted");
                led_set(CONNECTED_COLOR, DEFAULT_LED_INTENSITY);
            }
        } else {
            /* commissioning failed */
            ESP_LOGW(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (err_status == ESP_OK) {
            esp_zb_ieee_addr_t extended_pan_id;
            esp_zb_get_extended_pan_id(extended_pan_id);
            ESP_LOGI(TAG,
                     "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: "
                     "0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4], extended_pan_id[3],
                     extended_pan_id[2], extended_pan_id[1], extended_pan_id[0], esp_zb_get_pan_id(),
                     esp_zb_get_current_channel(), esp_zb_get_short_address());
            led_set(CONNECTED_COLOR, DEFAULT_LED_INTENSITY);
        } else {
            ESP_LOGI(TAG, "Network steering was not successful (status: %s)", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                   ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
        }
        break;
    case ESP_ZB_ZDO_SIGNAL_LEAVE:
        ESP_LOGI(TAG, "Re-start network steering");
        led_set(CONNECTING_COLOR, DEFAULT_LED_INTENSITY);
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        break;
    default:
        ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                 esp_err_to_name(err_status));
        break;
    }
}

static esp_err_t zigbee_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
    ESP_RETURN_ON_FALSE(message, ESP_FAIL, TAG, "Empty message");
    ESP_RETURN_ON_FALSE(message->info.status == ESP_ZB_ZCL_STATUS_SUCCESS, ESP_ERR_INVALID_ARG, TAG,
                        "Received message: error status(%d)", message->info.status);
    ESP_LOGI(TAG,
             "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), "
             "data size(%d), type(0x%x)",
             message->info.dst_endpoint, message->info.cluster, message->attribute.id, message->attribute.data.size,
             message->attribute.data.type);

    if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY) {
        led_blink(BLINK_IDENTIFY);
        return ESP_OK;
    }

    switch (message->info.dst_endpoint) {
    case ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID:
    case ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID + 1:
    case ELECTRICAL_MEASUREMENT_ENDPOINT_FIRST_ID + 2:
    case METERING_ENDPOINT_ID:
        return zigbee_meter_attribute_handler(message);
    case LIGHT_ENDPOINT_ID:
        return zigbee_light_attribute_handler(message);
    default:
        ESP_LOGW(TAG, "Unknown attribute ep=%d cl=0x%x attr=0x%x", message->info.dst_endpoint, message->info.cluster,
                 message->attribute.id);
        return ESP_FAIL;
    }
}

static esp_err_t zigbee_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
    esp_err_t ret = ESP_OK;
    switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
        ret = zigbee_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
        break;
    default:
        ESP_LOGW(TAG, "Receive Zigbee action(0x%x) callback", callback_id);
        break;
    }
    return ret;
}

static esp_zb_ep_list_t *zigbee_create_ep_list() {
    esp_zb_ep_list_t *epList = esp_zb_ep_list_create();
    zigbee_meter_create_ep(epList);
    zigbee_light_create_ep(epList);
    return epList;
}

static void zigbee_task(void *pvParameters) {
    (void)pvParameters;

    /* initialize Zigbee stack */
    esp_zb_cfg_t networkConfig = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ROUTER,
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,
        .nwk_cfg.zed_cfg =
            {
                .ed_timeout = ED_AGING_TIMEOUT,
                .keep_alive = ED_KEEP_ALIVE,
            },
    };
    esp_zb_init(&networkConfig);

    esp_zb_ep_list_t *epList = zigbee_create_ep_list();
    esp_zb_device_register(epList);

    esp_zb_core_action_handler_register(zigbee_action_handler);
    esp_zb_set_primary_network_channel_set(ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK);

    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

void zigbee_init(void) {
    esp_zb_platform_config_t config = {
        .radio_config =
            {
                .radio_mode = ZB_RADIO_MODE_NATIVE,
            },
        .host_config =
            {
                .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,
            },
    };
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
    xTaskCreate(zigbee_task, "Zigbee_main", 4096, NULL, 5, NULL);
}

void zigbee_reset_pairing(void) {
    ESP_LOGI(TAG, "Reset network pairing");
    led_set(CONNECTING_COLOR, DEFAULT_LED_INTENSITY);
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_bdb_reset_via_local_action();
    esp_zb_lock_release();
}
