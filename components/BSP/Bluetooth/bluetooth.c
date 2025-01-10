#include "bluetooth.h"

bool lvgl_lock(void);
void lvgl_unlock(void);
void set_bluetooth_status(bool is_connected);

#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)
#define GATTS_HANDLE_NUM 12

#define SERVICE_UUID {0x40, 0xE3, 0x4A, 0x1D, 0xC2, 0x5F, 0xB0, 0x9C, 0xB7, 0x47, 0xE6, 0x43, 0x0A, 0x00, 0x53, 0x86}
#define CHARACTERISTIC_UUID_RX {0x40, 0xE3, 0x4A, 0x1D, 0xC2, 0x5F, 0xB0, 0x9C, 0xB7, 0x47, 0xE6, 0x43, 0x0C, 0x00, 0x53, 0x86}
#define CHARACTERISTIC_UUID_TX {0x40, 0xE3, 0x4A, 0x1D, 0xC2, 0x5F, 0xB0, 0x9C, 0xB7, 0x47, 0xE6, 0x43, 0x0B, 0x00, 0x53, 0x86}

uint8_t adv_config_done = 0;
uint8_t bluetooth_connected = 0;
uint16_t BLE_MTUSIZE = 23;

static esp_gatt_if_t g_gatts_if;
static uint16_t conn_id = 0;
static esp_bd_addr_t remote_device_address;
static uint16_t gatts_service_handle_table[GATTS_HANDLE_NUM];

esp_attr_control_t gatts_attr_control = {
    .auto_rsp = ESP_GATT_AUTO_RSP, // 设置为自动响应
};

static uint8_t char_value_b[20] = {0x11, 0x22, 0x33, 0x44};

static esp_attr_value_t gatts_char_val_b = {
    .attr_max_len = 60, // 特征值的最大长度
    .attr_len = sizeof(char_value_b),     // 当前特征值长度
    .attr_value = char_value_b,           // 特征值的默认数据
};

static uint8_t adv_service_uuid128[16] = {
    // 16字节UUID，可以自定义
    0xFF, 0xFF, 0x0d, 0x00, 0x07, 0x21, 
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:{
            adv_config_done &= ~ADV_CONFIG_FLAG;
            if (adv_config_done == 0 && statework.st_bit.StChrg) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        }
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT: {
            adv_config_done &= ~SCAN_RSP_CONFIG_FLAG;
            if (adv_config_done == 0 && statework.st_bit.StChrg) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        }
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:{
            // advertising start complete event to indicate advertising start successfully or failed
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BT_TAG, "Advertising start failed");
            } else {
                ESP_LOGI(BT_TAG, "Advertising start successfully");
            }
            break;
        }
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: {
            // advertising stop complete event to indicate advertising stop successfully or failed
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BT_TAG, "Advertising stop failed");
            } else {
                ESP_LOGI(BT_TAG, "Advertising stop successfully");
            }
            break;
        }
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:{
            ESP_LOGI(BT_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        }
        default:
            break;

    }

}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:{


            g_gatts_if = gatts_if;
            esp_gatt_srvc_id_t service_id = {
                .is_primary = true,
                .id.inst_id = 0x00,
                .id.uuid.len = ESP_UUID_LEN_128,
            };
            memcpy(service_id.id.uuid.uuid.uuid128, (uint8_t[])SERVICE_UUID, 16);
            esp_ble_gatts_create_service(gatts_if, &service_id, GATTS_HANDLE_NUM);
        }
        break;
        case ESP_GATTS_CREATE_EVT: {
            ESP_LOGI(BT_TAG, "Service created, status %d,  service_handle %d", param->create.status, param->create.service_handle);
            gatts_service_handle_table[0] = param->create.service_handle;
            char str[256] = "";
            uint8_t *service_uuid = param->create.service_id.id.uuid.uuid.uuid128;
            sprintf(str, "Service UUID: %02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                service_uuid[15], service_uuid[14], service_uuid[13], service_uuid[12], 
                service_uuid[11], service_uuid[10], service_uuid[9], service_uuid[8], 
                service_uuid[7], service_uuid[6], service_uuid[5], service_uuid[4], 
                service_uuid[3], service_uuid[2], service_uuid[1], service_uuid[0]
            );
            ESP_LOGI(BT_TAG, "Service UUID: %s", str);
            
            // Tx Characteristic UUID
            esp_bt_uuid_t char_uuid_tx = {
                .len = ESP_UUID_LEN_128,
            };
            memcpy(char_uuid_tx.uuid.uuid128, (uint8_t[])CHARACTERISTIC_UUID_TX, 16);

            // Tx Characteristic Properties
            esp_gatt_char_prop_t char_prop_tx = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_ble_gatts_add_char(gatts_service_handle_table[0], &char_uuid_tx, ESP_GATT_PERM_READ, char_prop_tx, &gatts_char_val_b, &gatts_attr_control);
            
            // CCCD Descriptor
            uint8_t cccd_value[2] = {0x00, 0x00};
            esp_attr_value_t cccd_attr_value = {
                .attr_max_len = sizeof(cccd_value),
               .attr_len = sizeof(cccd_value),
               .attr_value = cccd_value,
            };

            esp_bt_uuid_t descr_uuid = {
                .len = ESP_UUID_LEN_16,
                .uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
            };

            // Add CCCD Descriptor
            esp_gatt_status_t add_descr_ret = esp_ble_gatts_add_char_descr(
                gatts_service_handle_table[0], &descr_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                &cccd_attr_value, &gatts_attr_control);
            if (add_descr_ret) {
                ESP_LOGE(BT_TAG, "add char descr failed, error code = %x", add_descr_ret);
            }

            // Rx Characteristic UUID
            esp_bt_uuid_t char_uuid_rx = {
                .len = ESP_UUID_LEN_128,
            };
            memcpy(char_uuid_rx.uuid.uuid128, (uint8_t[])CHARACTERISTIC_UUID_RX, 16);

            // Rx Characteristic Properties
            esp_gatt_char_prop_t char_prop_rx = ESP_GATT_CHAR_PROP_BIT_WRITE;
            esp_ble_gatts_add_char(gatts_service_handle_table[0], &char_uuid_rx, ESP_GATT_PERM_WRITE, char_prop_rx, &gatts_char_val_b, &gatts_attr_control);
        }
        break;
        case ESP_GATTS_ADD_CHAR_EVT:{
            ESP_LOGI(BT_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d", param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
            if(param->add_char.char_uuid.len == ESP_UUID_LEN_128)
            {
                if(memcmp(param->add_char.char_uuid.uuid.uuid128, (uint8_t[])CHARACTERISTIC_UUID_TX, 16) == 0)
                {
                    gatts_service_handle_table[2] = param->add_char.attr_handle;
                    ESP_LOGI(BT_TAG, "TX char handle %d saved", gatts_service_handle_table[2]);
                } else if(memcmp(param->add_char.char_uuid.uuid.uuid128, (uint8_t[])CHARACTERISTIC_UUID_RX, 16) == 0)
                {
                    gatts_service_handle_table[1] = param->add_char.attr_handle;
                    ESP_LOGI(BT_TAG, "RX char handle %d saved", gatts_service_handle_table[1]);
                }
            }

            if(gatts_service_handle_table[1] && gatts_service_handle_table[2])
            {
                esp_err_t srvc_start_ret = esp_ble_gatts_start_service(gatts_service_handle_table[0]);
                if (srvc_start_ret != ESP_OK)
                {
                    ESP_LOGE(BT_TAG, "start_service failed, error code = %x", srvc_start_ret);
                }
                else
                {
                    ESP_LOGI(BT_TAG, "service start successfully");
                }
            }
        }
        break;
        case ESP_GATTS_CONNECT_EVT: {
            esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(256);
            if (local_mtu_ret) {
                ESP_LOGE(BT_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
            } else {
                ESP_LOGI(BT_TAG, "set local  MTU successful");
            }
            ESP_LOGI(BT_TAG, "Device connected, conn_id %d", param->connect.conn_id);
            conn_id = param->connect.conn_id;
            memcpy(remote_device_address, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            bluetooth_connected = true;
            if (lvgl_lock())
            {
                set_bluetooth_status(true);
                lvgl_unlock();
            }
            
        }
        break;
        case ESP_GATTS_DISCONNECT_EVT: {
            ESP_LOGI(BT_TAG, "Device disconnected, reason code %d", param->disconnect.reason);
            conn_id = 0;
            memset(remote_device_address, 0, sizeof(esp_bd_addr_t));
            bluetooth_connected = false;
            if (statework.st_bit.StChrg)
                esp_ble_gap_start_advertising(&adv_params);
            if (lvgl_lock())
            {
                set_bluetooth_status(false);
                lvgl_unlock();
            }
        }
        break;
        case ESP_GATTS_WRITE_EVT: {
            ESP_LOGI(BT_TAG, "WRITE_EVT, conn_id %d, len %d, handle %d", param->write.conn_id, param->write.len, param->write.handle);
            if (param->write.handle == gatts_service_handle_table[1]) {
                char rx_data[512] = "";
                for (int i = 0; i < param->write.len; i++) {
                    sprintf(rx_data + i * 3, "%02x ", param->write.value[i]);
                }
                ESP_LOGI(BT_TAG, "Received data: %s", rx_data);
            }
            frame_t *frame = parse_frame(param->write.value);
            if (frame) {
                ESP_LOGI(BT_TAG, "Received valid frame, cmd: %d, cmd_type: %d, data_len: %d", frame->cmd, frame->cmd_type, frame->data_len);
                cmd_handler(frame);
                
            }
        }
        break;
        case ESP_GATTS_MTU_EVT:{
            ESP_LOGI(BT_TAG, "MTU exchange, MTU %d", param->mtu.mtu);
            BLE_MTUSIZE = param->mtu.mtu;
        }
        break;
        default:
            break;
    }
}

void bluetooth_init()
{
    esp_err_t ret;

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(BT_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(BT_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(BT_TAG, "%s initialize bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(BT_TAG, "%s enable bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(BT_TAG, "gap register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(BT_TAG, "gattc register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_app_register(BT_APP_ID);
    if (ret) {
        ESP_LOGE(BT_TAG, "gatts app register failed: %s", esp_err_to_name(ret));
        return;
    }

    char *dev_name = (char *)nvs_get_bluetooth_name();
    esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(dev_name);
    if (set_dev_name_ret) {
        ESP_LOGE(BT_TAG, "set device name failed, error code = %x", set_dev_name_ret);
    } else {
        ESP_LOGI(BT_TAG, "set device name successful");
    }
    
    esp_err_t set_adv_data_ret = esp_ble_gap_config_adv_data(&adv_data);
    if (set_adv_data_ret) {
        ESP_LOGE(BT_TAG, "config adv data failed, error code = %x", set_adv_data_ret);
    } else {
        ESP_LOGI(BT_TAG, "config adv data successful");
    }
    adv_config_done |= ADV_CONFIG_FLAG;

    esp_err_t set_scan_rsp_data_ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (set_scan_rsp_data_ret) {
        ESP_LOGE(BT_TAG, "config scan rsp data failed, error code = %x", set_scan_rsp_data_ret);
    } else {
        ESP_LOGI(BT_TAG, "config scan rsp data successful");
    }
    adv_config_done |= SCAN_RSP_CONFIG_FLAG;

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret) {
        ESP_LOGE(BT_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

}

void ble_send_data(uint8_t *data, uint16_t length)
{
    if (bluetooth_connected)
    {
        printf("ble_send_data\n");
        // // 发送数据到BLE设备
        // esp_ble_gatts_send_indicate(gatts_if, conn_id, gatts_service_handle_table[1],
        //                             length, data, false);
        int mtu = BLE_MTUSIZE - 3; // 获取当前 MTU，减去 ATT 协议头的 3 字节
        int offset = 0;
        while (length > 0)
        {
            int send_len = (length > mtu) ? mtu : length;
            esp_ble_gatts_send_indicate(g_gatts_if, conn_id, gatts_service_handle_table[2], send_len, data + offset, false);
            offset += send_len;
            length -= send_len;
            vTaskDelay(pdMS_TO_TICKS(10)); // 避免发送过快
        }
    }
    else
    {
        printf("ble_send_data fail\n");
    }
}

void ble_disconnect()
{
    if (bluetooth_connected)
    {
        esp_ble_gatts_close(g_gatts_if, conn_id);
        esp_ble_gap_disconnect(remote_device_address);
    }
}

void ble_start_adv()
{
    esp_ble_gap_start_advertising(&adv_params);
}

void ble_stop_adv()
{
    esp_ble_gap_stop_advertising();
}