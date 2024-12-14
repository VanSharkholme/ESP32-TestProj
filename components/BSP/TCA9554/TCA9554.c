#include "TCA9554.h"

i2c_master_dev_handle_t TCA9554_dev_handle = NULL;

void TCA9554_Dev_Init(i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == NULL)
    {
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
    }

    i2c_device_config_t TCA9554_config = {
       .dev_addr_length = I2C_ADDR_BIT_LEN_7,
       .device_address = TCA9554_I2C_ADDR,
       .scl_speed_hz = 400000,
    };

    if(ESP_OK == i2c_master_probe(bus_handle, TCA9554_I2C_ADDR, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", TCA9554_config.device_address);
    }

    TCA9554_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &TCA9554_config, &TCA9554_dev_handle));
}

void TCA9554_Init()
{
    if (i2c_bus0_handle == NULL)
    {
        I2C_Init();
    }

    TCA9554_Dev_Init(i2c_bus0_handle);

}

void TCA9554_WriteReg(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_data[2] = {reg_addr, data};
    i2c_master_transmit(TCA9554_dev_handle, write_data, 2, -1);
}

void TCA9554_ReadReg(uint8_t reg_addr, uint8_t *data)
{
    i2c_master_transmit_receive(TCA9554_dev_handle, &reg_addr, 1, data, 1, -1);
}

void TCA9554_SetPinDir(uint8_t pin, TCA9554_PIN_MODE_T mode)
{
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    reg_data = pin;
    reg_addr = TCA9554_REG_CONFIGURATION;
    TCA9554_ReadReg(reg_addr, &reg_data);
    if (mode == TCA9554_INPUT)
        reg_data |= pin;
    else
        reg_data &= ~pin;
    TCA9554_WriteReg(reg_addr, reg_data);
}

void TCA9554_SetPinState(uint8_t pin, uint8_t state)
{
    TCA9554_SetPinDir(pin, TCA9554_OUTPUT);
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    reg_addr = TCA9554_REG_OUTPUT;
    TCA9554_ReadReg(reg_addr, &reg_data);
    if (state)
        reg_data |= pin;
    else
        reg_data &= ~pin;
    TCA9554_WriteReg(reg_addr, reg_data);
}

uint8_t TCA9554_GetPinState(uint8_t pin)
{
    TCA9554_SetPinDir(pin, TCA9554_INPUT);
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    reg_addr = TCA9554_REG_INPUT;
    TCA9554_ReadReg(reg_addr, &reg_data);
    return reg_data;
}