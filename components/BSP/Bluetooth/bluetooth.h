#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "nvs_flash.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "nvs_op.h"
#include "protocal.h"
#include "TCA9555.h"

#define BT_TAG "BT"
#define BT_APP_ID 0x11

void bluetooth_init();
void ble_send_data(uint8_t *data, uint16_t length);
void ble_disconnect();
void ble_start_adv();
void ble_stop_adv();

#endif // !BLUETOOTH_H