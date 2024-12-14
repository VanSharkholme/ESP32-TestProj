#ifndef _GT911_H_
#define _GT911_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "I2C.h"
#include "TCA9555.h"


#define GT_Write_ADDR 0xBA
#define GT_Read_ADDR  0xBB
#define GT_ID_ADDR    0x8140
#define GT_CFG_START  0x8047
#define GT_Point_ADDR 0x814E
#define GT_COORD_ADDR 0x8150
#define GT_INT_PIN    6
// #define GT911_COORD_LOG

#define GT_RES_Clr() do{ TCA9555_SetPinState(TCA9555_PIN_TP_RST, 0);}while(0)
#define GT_RES_Set() do{ TCA9555_SetPinState(TCA9555_PIN_TP_RST, 1);}while(0)
#define GT_INT_Clr() do{ gpio_set_level(GT_INT_PIN, 0);}while(0)
#define GT_INT_Set() do{ gpio_set_level(GT_INT_PIN, 1);}while(0)

// 0x804D初始值0000 1101

extern i2c_master_dev_handle_t gt911_dev_handle;
extern uint32_t GT911_CFG_TBL[186];
esp_err_t GT_WR_DATA(uint8_t addr[2],uint8_t data[], uint8_t len);
esp_err_t GT_RD_DATA(uint8_t addr[2],uint8_t len,uint8_t value[]);

void GT_Init();
bool GT911_Read_Coordinate(int16_t *x, int16_t *y);
#ifdef __cplusplus
}
#endif
#endif
