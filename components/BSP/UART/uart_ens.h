#ifndef __UART_H__
#define __UART_H__

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "datastructures.h"

#define ENS_UART_NUM UART_NUM_0
#define ENS_UART_BUF_SIZE 2048
#define ENS_TXD_PIN 43
#define ENS_RXD_PIN 44

void ens_uart_init(void);
void ens_uart_send(uint8_t *data, uint16_t len);
int ens_uart_recv(uint8_t *data, uint16_t len);
void ens_uart_send_cmd(uint8_t cmd, uint8_t cmd_type, uint8_t *data, uint16_t len);
void ens_start_channel_plan(Plan *pPlan, uint8_t index);
void ens_stop_channel_plan(Plan *pPlan, uint8_t index);
// void ens_uart_send_channel_base(Plan *plan, uint8_t channel_id);
// void ens_uart_send_plan(Plan *plan, uint8_t channel_id);



#endif