#include "uart_ens.h"

void ens_uart_init(void)
{
    const uart_port_t uart_num = ENS_UART_NUM;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(ENS_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ENS_UART_NUM, ENS_TXD_PIN, ENS_RXD_PIN, -1, -1));
    ESP_ERROR_CHECK(uart_driver_install(ENS_UART_NUM, ENS_UART_BUF_SIZE, ENS_UART_BUF_SIZE, 0, NULL, 0));
}

void ens_uart_send(uint8_t *data, uint16_t len)
{
    uart_write_bytes(ENS_UART_NUM, (const char*)data, len);
}

int ens_uart_recv(uint8_t *data, uint16_t len)
{
    int len_read = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(ENS_UART_NUM, (size_t*)&len_read));
    if (len_read > 0)
    {
        len_read = uart_read_bytes(ENS_UART_NUM, data, len, 100);
        ESP_LOGI("ENS_UART", "Received %d bytes", len_read);
        if (data[0] == 0x5A && data[1] == 0xA5)
        {
            ESP_LOGI("ENS_UART", "Received from ENS: %02X %02X %02X %02X %02X %02X", data[0], data[1], data[2], data[3], data[4], data[5]);
        }
    }
    return len_read;
    
}

uint8_t ens_uart_checksum(uint8_t cmd, uint8_t cmd_type, uint8_t *data, uint16_t len)
{
    uint8_t checksum = 0;
    checksum += len + 2;
    checksum += cmd;
    checksum += cmd_type;
    for (int i = 0; i < len; i++)
    {
        checksum += data[i];
    }
    return checksum;
}

void ens_uart_send_cmd(uint8_t cmd, uint8_t cmd_type, uint8_t *data, uint16_t len)
{
    uint8_t send_data[100];
    uint8_t checksum = len + 2 + cmd + cmd_type;
    uint8_t i = 0;
    send_data[i++] = 0x5A;
    send_data[i++] = 0xA5;
    send_data[i++] = len + 2;
    send_data[i++] = cmd;
    send_data[i++] = cmd_type;
    for (uint8_t j = 0; j < len; i++,j++)
    {
        send_data[i] = data[j];
        checksum += data[j];
    }
    send_data[i++] = checksum;
    ens_uart_send(send_data, i);
}

void ens_uart_send_channel_base(Plan *plan, uint8_t channel_id)
{
    uint8_t data[5];
    // data[0] = plan->id;
    data[0] = 1; // 方案编号全部设置为1
    data[1] = plan->scheme_type;
    data[2] = 1 << channel_id;
    data[3] = 0;
    data[4] = 0;
    ens_uart_send_cmd(0x20, 0x11, data, 5);
}

void ens_uart_send_plan(Plan *plan, uint8_t channel_id)
{
    uint8_t data[21];
    plan_to_data(plan, channel_id, data);
    ens_uart_send_cmd(0x20, 0x12, data, 21);
}

void ens_start_channel_plan(Plan *pPlan, uint8_t index)
{
    ens_uart_send_channel_base(pPlan, index);
    ens_uart_send_plan(pPlan, index);
    uint8_t start_stim_data = 0;
    // start_stim_data |= pPlan->id << 4;
    start_stim_data |= 1 << 4; // 方案编号全部设置为1
    start_stim_data |= 1 << index;
    ens_uart_send_cmd(0x30, 0x10, &start_stim_data, 1);
}

void ens_stop_channel_plan(Plan *pPlan, uint8_t index)
{
    // uint8_t stop_stim_data = 0;
    // stop_stim_data |= pPlan->id << 4;
    // stop_stim_data |= index;
    // ens_uart_send_cmd(0x31, 0x10, &stop_stim_data, 1);
    // TODO: Implement stop single channel plan command
    uint8_t ch_sel = 0;
    ch_sel |= 1 << 4; // 方案编号全部设置为1
    ch_sel |= 1 << index;
    ens_uart_send_cmd(0x31, 0x10, &ch_sel, 1);
}
