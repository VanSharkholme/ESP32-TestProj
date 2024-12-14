#ifndef __PROTOCAL_H__
#define __PROTOCAL_H__

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "string.h"
#include "nvs_op.h"
#include "FuelGauge.h"
#include "bluetooth.h"
#include "uart_ens.h"
#include "custom_types.h"

#define PROTOCAL_CMD_FIRMWARE_UPGRADE 0x01
#define PROTOCAL_CMD_FIRMWARE_DATA 0x02
#define PROTOCAL_CMD_FIRMWARE_UPGRADE_END 0x03
#define PROTOCAL_CMD_GET_FIRMARE_VERSION 0x10
#define PROTOCAL_CMD_GET_BATTERY_STATUS 0x11
#define PROTOCAL_CMD_SET_DEVICE_BT_NAME 0x12
#define PROTOCAL_CMD_WRITE_PREDEFINED_PLAN 0x20
#define PROTOCAL_CMD_READ_PREDEFINED_PLAN 0x21
#define PROTOCAL_CMD_START_STIMULATION 0x30
#define PROTOCAL_CMD_STOP_STIMULATION 0x31

#define PROTOCAL_CMD_TYPE_WRITE_PLAN_NAME 0x10
#define PROTOCAL_CMD_TYPE_WRITE_PLAN_BASE_SETTING 0x11
#define PROTOCAL_CMD_TYPE_WRITE_PLAN_CHANNEL_SETTING 0x12
#define PROTOCAL_CMD_TYPE_READ_PLAN_BASE_SETTING 0x11
#define PROTOCAL_CMD_TYPE_READ_PLAN_CHANNEL_SETTING 0x12

typedef struct {
    uint8_t cmd;
    uint8_t cmd_type;
    uint8_t data_len;
    uint8_t *data;
    uint8_t checksum;
    uint8_t *raw_data;
    uint16_t raw_data_len;
} frame_t;

frame_t *parse_frame(uint8_t *frame);
void construct_frame(frame_t *f, uint8_t *frame);
void cmd_handler(frame_t *f);
void no_data_respond(frame_t *f);

#endif /* __PROTOCAL_H__ */