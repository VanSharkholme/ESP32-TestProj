#include "protocal.h"

frame_t *parse_frame(uint8_t *frame)
{
    if (frame[0] == 0x5A && frame[1] == 0xA5)
    {
        frame_t *f = (frame_t *)malloc(sizeof(frame_t));
        f->cmd = frame[3];
        f->cmd_type = frame[4];
        f->data_len = frame[2] - 2;
        if (f->data_len > 0)
        {
            f->data = (uint8_t *)malloc(f->data_len);
            memcpy(f->data, &frame[5], f->data_len);
        }
        else
            f->data = NULL;
        f->checksum = frame[f->data_len + 5];
        uint8_t checksum = 0;
        for (int i = 2; i < f->data_len + 3 + 2; i++)
            checksum += frame[i];
        if (checksum != f->checksum)
        {
            free(f->data);
            f->data = NULL;
            free(f);
            return NULL;
        }
        f->raw_data = frame;
        f->raw_data_len = f->data_len + 6;
        return f;
    }
    else
        return NULL;    
}

void construct_frame(frame_t *f, uint8_t *frame)
{
    frame[0] = 0x5A;
    frame[1] = 0xA5;
    frame[2] = f->data_len + 2;
    frame[3] = f->cmd;
    frame[4] = f->cmd_type;
    memcpy(&frame[5], f->data, f->data_len);
    uint8_t checksum = 0;
    for (int i = 2; i < f->data_len + 5; i++)
        checksum += frame[i];
    frame[f->data_len + 5] = checksum;
}

void cmd_handler(frame_t *f)
{
    switch (f->cmd)
    {
        case PROTOCAL_CMD_FIRMWARE_UPGRADE:
        {
            break;
        }
        case PROTOCAL_CMD_FIRMWARE_DATA:
        {
            break;
        }
        case PROTOCAL_CMD_FIRMWARE_UPGRADE_END:
        {
            break;
        }
        case PROTOCAL_CMD_GET_FIRMARE_VERSION:
        {
            break;
        }
        case PROTOCAL_CMD_GET_BATTERY_STATUS:
        {
            uint8_t is_charging = FuelGauge_is_Charging() ? 0x01 : 0x00;
            uint8_t soc = FuelGauge_Get_SOC();
            f->data_len = 3;
            f->data = (uint8_t *)malloc(f->data_len);
            f->data[0] = is_charging;
            f->data[1] = soc;
            f->data[2] = 0x00;
            uint8_t tx_data[9];
            construct_frame(f, tx_data);
            ble_send_data(tx_data, 9);
            break;
        }
        case PROTOCAL_CMD_SET_DEVICE_BT_NAME:
        {
            uint8_t name_len = f->data[0];
            char *name = (char *)malloc(name_len + 1);
            memcpy(name, &f->data[1], name_len);
            name[name_len] = '\0';
            nvs_save_bluetooth_name(name);
            free(name);
            no_data_respond(f);
            break;
        }
        case PROTOCAL_CMD_WRITE_PREDEFINED_PLAN:
        {
            // if (f->cmd_type == PROTOCAL_CMD_TYPE_WRITE_PLAN_NAME)
            // {
            //     uint8_t plan_id = f->data[0];
            //     uint8_t name_len = f->data[1];
            //     uint8_t seg_num = f->data[2];
            //     saved_plans[plan_id].id = plan_id;
            //     saved_plans[plan_id].name_len = name_len;
            //     memcpy(saved_plans[plan_id].name, &f->data[3], name_len);
            //     // nvs_save_plans(saved_plans);
            //     // no_data_respond(f);
            // }
            // else if (f->cmd_type == PROTOCAL_CMD_TYPE_WRITE_PLAN_BASE_SETTING)
            // {
            //     uint8_t plan_id = f->data[0];
            //     saved_plans[plan_id].id = plan_id;
            //     schemeType_t scheme_type = (schemeType_t)f->data[1];
            //     saved_plans[plan_id].scheme_type = scheme_type;
            //     // nvs_save_plans(saved_plans);
            //     // ens_uart_send(f->raw_data, f->raw_data_len);
            //     // no_data_respond(f);
            // }
            // else if (f->cmd_type == PROTOCAL_CMD_TYPE_WRITE_PLAN_CHANNEL_SETTING)
            // {
            //     uint8_t plan_id = f->data[0] >> 4;
            //     saved_plans[plan_id].id = plan_id;
            //     saved_plans[plan_id].wave_type = f->data[1];
            //     saved_plans[plan_id].current_mA = f->data[2];
            //     saved_plans[plan_id].pulse_width = (uint16_t)(f->data[4] << 8 | f->data[5]);
            //     saved_plans[plan_id].pulse_interval = f->data[6];
            //     saved_plans[plan_id].freq_type = f->data[7];
            //     saved_plans[plan_id].freq_min = (uint16_t)(f->data[8] << 8 | f->data[9]);
            //     saved_plans[plan_id].freq_max = (uint16_t)(f->data[10] << 8 | f->data[11]);
            //     saved_plans[plan_id].wave_rise = (uint16_t)(f->data[12] << 8 | f->data[13]);
            //     saved_plans[plan_id].wave_fall = (uint16_t)(f->data[14] << 8 | f->data[15]);
            //     saved_plans[plan_id].work_time = f->data[16];
            //     saved_plans[plan_id].break_time = f->data[18];
            //     saved_plans[plan_id].total_time_min = f->data[20];
            //     // nvs_save_plans(saved_plans);
            // }
            no_data_respond(f);
            ens_uart_send(f->raw_data, f->raw_data_len);
            break;
        }
        case PROTOCAL_CMD_READ_PREDEFINED_PLAN:
        {
            if (f->cmd_type == PROTOCAL_CMD_TYPE_READ_PLAN_BASE_SETTING)
            {
                uint8_t plan_id = f->data[0];
                uint8_t *tx_data = (uint8_t *)malloc(5);
                tx_data[0] = f->data[0];
                tx_data[1] = saved_plans[plan_id].scheme_type;
                tx_data[2] = 0;
                tx_data[3] = 0;
                tx_data[4] = 0;
                f->data = tx_data;
                f->data_len = 5;
                uint8_t respond_frame[11] = {0};
                construct_frame(f, respond_frame);
                ble_send_data(respond_frame, 11);
            }
            else if (f->cmd_type == PROTOCAL_CMD_TYPE_READ_PLAN_CHANNEL_SETTING)
            {
                uint8_t plan_id = f->data[0] >> 4;
                uint8_t *tx_data = (uint8_t *)malloc(21);
                tx_data[0] = f->data[0] & 0xF0;
                tx_data[1] = saved_plans[plan_id].wave_type;
                tx_data[2] = saved_plans[plan_id].current_mA;
                tx_data[3] = 0;
                tx_data[4] = saved_plans[plan_id].pulse_width >> 8;
                tx_data[5] = saved_plans[plan_id].pulse_width & 0xFF;
                tx_data[6] = saved_plans[plan_id].pulse_interval;
                tx_data[7] = saved_plans[plan_id].freq_type;
                tx_data[8] = saved_plans[plan_id].freq_min >> 8;
                tx_data[9] = saved_plans[plan_id].freq_min & 0xFF;
                tx_data[10] = saved_plans[plan_id].freq_max >> 8;
                tx_data[11] = saved_plans[plan_id].freq_max & 0xFF;
                tx_data[12] = saved_plans[plan_id].wave_rise >> 8;
                tx_data[13] = saved_plans[plan_id].wave_rise & 0xFF;
                tx_data[14] = saved_plans[plan_id].wave_fall >> 8;
                tx_data[15] = saved_plans[plan_id].wave_fall & 0xFF;
                tx_data[16] = saved_plans[plan_id].work_time;
                tx_data[17] = 0;
                tx_data[18] = saved_plans[plan_id].break_time;
                tx_data[19] = 0;
                tx_data[20] = saved_plans[plan_id].total_time_min;
                f->data = tx_data;
                f->data_len = 21;
                uint8_t respond_frame[27] = {0};
                construct_frame(f, respond_frame);
                ble_send_data(respond_frame, 27);
            }
            break;
        }
        case PROTOCAL_CMD_START_STIMULATION:
        {
            ens_uart_send(f->raw_data, f->raw_data_len);
            no_data_respond(f);
            break;
        }
        case PROTOCAL_CMD_STOP_STIMULATION:
        {
            ESP_LOGI("cmd_handler", "Stop Stimulation");
            ens_uart_send(f->raw_data, f->raw_data_len);
            no_data_respond(f);
            break;
        }
    }
    free(f->data);
    f->data = NULL;
    free(f);
}

void no_data_respond(frame_t *f)
{
    uint8_t respond_frame[6] = {0};
    f->data_len = 0;
    construct_frame(f, respond_frame);
    ble_send_data(respond_frame, 6);
}
