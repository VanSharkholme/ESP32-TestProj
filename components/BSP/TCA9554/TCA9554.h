#ifndef __TCA9554_H__
#define __TCA9554_H__

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "I2C.h"

#define TCA9554_I2C_ADDR 0x38
#define TCA9554_REG_INPUT 0x00
#define TCA9554_REG_OUTPUT 0x01
#define TCA9554_REG_POLARITY_INVERSION 0x02
#define TCA9554_REG_CONFIGURATION 0x03

#define TCA9554_PIN_0 0x01
#define TCA9554_PIN_1 0x02
#define TCA9554_PIN_2 0x04
#define TCA9554_PIN_3 0x08
#define TCA9554_PIN_4 0x10
#define TCA9554_PIN_5 0x20
#define TCA9554_PIN_6 0x40
#define TCA9554_PIN_7 0x80

typedef enum {
    TCA9554_OUTPUT = 0,
    TCA9554_INPUT = 1,
} TCA9554_PIN_MODE_T;

void TCA9554_Init();
void TCA9554_SetPinDir(uint8_t pin, TCA9554_PIN_MODE_T mode);
void TCA9554_SetPinState(uint8_t pin, uint8_t state);
uint8_t TCA9554_GetPinState(uint8_t pin);

#endif