#ifndef FUELGAUGE_H_
#define FUELGAUGE_H_

#include "I2C.h"
#include "PCA9546.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define FUEL_GAUGE_I2C_ADDR 0x55
#define FUEL_GAUGE_I2C_CHANNEL 2

#define FUEL_GAUGE_SAFETY_STATUS_INV_PROT_CHKSUM_MASK 0b10000000
#define FUEL_GAUGE_SAFETY_STATUS_ISD_MASK 0b00100000
#define FUEL_GAUGE_SAFETY_STATUS_TDD_MASK 0b00010000
#define FUEL_GAUGE_SAFETY_STATUS_OTC_MASK 0b00001000
#define FUEL_GAUGE_SAFETY_STATUS_OTD_MASK 0b00000100
#define FUEL_GAUGE_SAFETY_STATUS_OVP_MASK 0b00000010
#define FUEL_GAUGE_SAFETY_STATUS_UVP_MASK 0b00000001

extern uint8_t g_battery_soc;

void FuelGauge_Init(void);
void FuelGauge_WriteReg(uint8_t reg_addr[2], uint8_t* data);
void FuelGauge_ReadReg(uint8_t reg_addr[2], uint8_t *data);
void FuelGauge_WriteCtrlReg(uint8_t sub_cmd[2], uint8_t* data);
void FuelGauge_ReadCtrlReg(uint8_t sub_cmd[2], uint8_t *data);
uint16_t FuelGauge_Device_Type();
void FuelGauge_Get_SOC();
uint8_t FuelGauge_Get_Safety_Status();
bool FuelGauge_is_Charging();


#endif /* FUELGAUGE_H_ */