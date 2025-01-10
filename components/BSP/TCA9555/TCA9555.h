#ifndef TCA9555_H_
#define TCA9555_H_

#include "I2C.h"
#include "PCA9546.h"
#include "bluetooth.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#define TCA9555_I2C_CHANNEL 0


// #define TCA9555_I2C_ADDR 0x20
// 
// #define TCA9555_REG_INPUT_PORT0 0x00
// #define TCA9555_REG_INPUT_PORT1 0x01
// #define TCA9555_REG_OUTPUT_PORT0 0x02
// #define TCA9555_REG_OUTPUT_PORT1 0x03
// #define TCA9555_REG_POLARITY_INVERSION_PORT0 0x04
// #define TCA9555_REG_POLARITY_INVERSION_PORT1 0x05
// #define TCA9555_REG_CONFIGURATION_PORT0 0x06
// #define TCA9555_REG_CONFIGURATION_PORT1 0x07
// 
// #define TCA9555_PIN_0 0x01
// #define TCA9555_PIN_1 0x02
// #define TCA9555_PIN_2 0x04
// #define TCA9555_PIN_3 0x08
// #define TCA9555_PIN_4 0x10
// #define TCA9555_PIN_5 0x20
// #define TCA9555_PIN_6 0x40
// #define TCA9555_PIN_7 0x80

typedef enum {
    TCA9555_PORT0 = 0,
    TCA9555_PORT1 = 1,
} TCA9555_PORT_T;

typedef enum {
    TCA9555_OUTPUT = 0,
    TCA9555_INPUT = 1,
} TCA9555_PIN_MODE_T;
//

#define TCA9555_I2C_ADDR 0x20
#define TCA9555_REG_INPUT_PORT0 0x00
#define TCA9555_REG_INPUT_PORT1 0x01
#define TCA9555_REG_OUTPUT_PORT0 0x02
#define TCA9555_REG_OUTPUT_PORT1 0x03
#define TCA9555_REG_POLARITY_INVERSION_PORT0 0x04
#define TCA9555_REG_POLARITY_INVERSION_PORT1 0x05
#define TCA9555_REG_CONFIGURATION_PORT0 0x06
#define TCA9555_REG_CONFIGURATION_PORT1 0x07

#define TCA9555_PIN_TP_RST 0x00
#define TCA9555_PIN_LCD_RST 0x01
#define TCA9555_PIN_BL_EN 0x02       //使能背光
#define TCA9555_PIN_GREEN 0x03        //绿灯
#define TCA9555_PIN_YELLOW 0x04        //黄灯
#define TCA9555_PIN_5 0x05
#define TCA9555_PIN_ADC_ADDR0 0x06
#define TCA9555_PIN_ADC_ADDR1 0x07
#define TCA9555_PIN_CHR_STA_IND 0x08 //充电输入检测  0 有接入充电口
#define TCA9555_PIN_5V_PWR 0x09      //暖芯迦片子上电
#define TCA9555_PIN_ADC_EN 0x0A      //刺激电压采集EN
#define TCA9555_PIN_BL_ADJ 0x0B      //输入 ，按键短按检测，控制屏开关
#define TCA9555_PIN_14 0x0C
#define TCA9555_PIN_15 0x0D
#define TCA9555_PIN_16 0x0E
#define TCA9555_PIN_17 0x0F

typedef struct 
{
    uint16_t pin_tp_rst:        1;
    uint16_t pin_lcd_rst:       1;
    uint16_t pin_bl_en:         1;
    uint16_t pin_led3:          1;
    uint16_t pin_led4:          1;
    uint16_t pin_05:            1;
    uint16_t pin_adc_addr0:     1;
    uint16_t pin_adc_addr1:     1;
    uint16_t pin_chr_sta_ind:   1;
    uint16_t pin_5v_pwr:        1;
    uint16_t pin_adc_en:        1;
    uint16_t pin_bl_adj:        1;
    uint16_t pin_14:            1;
    uint16_t pin_15:            1;
    uint16_t pin_16:            1;
    uint16_t pin_17:            1;

}PIN_BIT;

typedef struct 
{
    uint8_t StRun:        1;  
    uint8_t StChrgOK:     1;
    uint8_t StChrg:       1;
    uint8_t StPowerLow:   1;
    uint8_t StOn:         1;
    uint8_t St_05:        1;
    uint8_t St_06:        1;
    uint8_t St_07:        1;
}StateBin;

typedef union STW
{
    StateBin st_bit;
    uint8_t st_data;

}StateWork;

extern StateWork statework;


typedef union PIO
{
    PIN_BIT pin_bit;
    uint16_t pin_data;

}PIN_IO;

enum sLedType
{
    LedGreenOn = 0x00,
    LedGreenFlashSlow = 0x01,
    LedGreenFlashFast = 0x02,
    LedYellowOn = 0x03,
    LedYellowFlashSlow = 0x04,
    LedYellowFlashFast = 0x05,

};
enum ledstate
{
    cLedON,
    cLedOFF,
    
};

#define CTRL_I2C_START_NUM  4
#define CTRL_I2C_START_SEL  (1ULL<<CTRL_I2C_START_NUM) 
//
void TCA9555_Init();

void TCA9555_SetPinDir(uint8_t pin, TCA9555_PIN_MODE_T mode);
bool TCA9555_GetPinState(uint8_t pin);
void TCA9555_SetPinState(uint8_t pin, uint8_t state);

void ledTask(StateWork statew);
void LedCtrl(uint16_t uwLedType,uint16_t uwBLinkTime,uint16_t uwExeCycle);
void sLedStatusSet(uint16_t wLedType,uint16_t wLedStatus);
void checkIO(void);
void tca9555_task(void *arg);

#endif // !TCA9555_H_
