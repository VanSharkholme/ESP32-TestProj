#include "FuelGauge.h"

i2c_master_dev_handle_t FuelGauge_dev_handle = NULL;

void FuelGauge_Dev_Init(i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == NULL)
    {
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
    }

    i2c_device_config_t FuelGauge_config = {
       .dev_addr_length = I2C_ADDR_BIT_LEN_7,
       .device_address = FUEL_GAUGE_I2C_ADDR,
       .scl_speed_hz = 100000,
    };
    PCA9546_SelectChannel(FUEL_GAUGE_I2C_CHANNEL);
    if(ESP_OK == i2c_master_probe(bus_handle, FUEL_GAUGE_I2C_ADDR, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", FuelGauge_config.device_address);
    }
    PCA9546_DeselectChannel(FUEL_GAUGE_I2C_CHANNEL);
    FuelGauge_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &FuelGauge_config, &FuelGauge_dev_handle));
    if (FuelGauge_dev_handle)
    {
        uint16_t device_type = FuelGauge_Device_Type();
        // ESP_LOGI("I2C", "Gauge Device type: 0x%04X", device_type);
    }
    
}

void FuelGauge_Init(void)
{
    if(i2c_bus1_handle == NULL)
    {
        I2C_Init();
    }

    FuelGauge_Dev_Init(i2c_bus1_handle);
}

void FuelGauge_WriteReg(uint8_t reg_addr[2], uint8_t* data)
{
    uint8_t tx_data[10] = {reg_addr[0], reg_addr[1], data[0], data[1]};
    PCA9546_SelectChannel(FUEL_GAUGE_I2C_CHANNEL);
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit(FuelGauge_dev_handle, tx_data, 4, -1);
    xSemaphoreGive(i2c_bus1_mutex);
    PCA9546_DeselectChannel(FUEL_GAUGE_I2C_CHANNEL);
}

void FuelGauge_ReadReg(uint8_t reg_addr[2], uint8_t *data)
{
    PCA9546_SelectChannel(FUEL_GAUGE_I2C_CHANNEL);
    i2c_master_transmit_receive(FuelGauge_dev_handle, reg_addr, 1, data, 2, -1);
    PCA9546_DeselectChannel(FUEL_GAUGE_I2C_CHANNEL);
}

void FuelGauge_WriteCtrlReg(uint8_t sub_cmd[2], uint8_t* data)
{
    uint8_t tx_data[10] = {0x00, 0x01, sub_cmd[0], sub_cmd[1], data[0], data[1]};
    PCA9546_SelectChannel(FUEL_GAUGE_I2C_CHANNEL);
    i2c_master_transmit(FuelGauge_dev_handle, tx_data, 6, -1);
    PCA9546_DeselectChannel(FUEL_GAUGE_I2C_CHANNEL);
}

void FuelGauge_ReadCtrlReg(uint8_t sub_cmd[2], uint8_t *data)
{
    uint8_t tx_data[10] = {0x00, sub_cmd[0], sub_cmd[0]};
    PCA9546_SelectChannel(FUEL_GAUGE_I2C_CHANNEL);
    i2c_master_transmit(FuelGauge_dev_handle, (uint8_t[2]){0x00, sub_cmd[1]}, 2, -1);
    i2c_master_transmit(FuelGauge_dev_handle, (uint8_t[2]){0x01, sub_cmd[0]}, 2, -1);
    i2c_master_transmit_receive(FuelGauge_dev_handle, tx_data, 1, data, 2, -1);
    PCA9546_DeselectChannel(FUEL_GAUGE_I2C_CHANNEL);
    // i2c_master_receive(FuelGauge_dev_handle, data, 2, -1);
    // i2c_master_transmit_receive(FuelGauge_dev_handle, tx_data, 3, data, 2, -1);
}

uint16_t FuelGauge_Device_Type()
{
    uint8_t data[2] = {0x00, 0x01};
    FuelGauge_ReadCtrlReg(data, data);
    ESP_LOGI("I2C", "Gauge Device type: 0x%02X 0x%02X", data[0], data[1]);

    return (data[0] << 8) | data[1];
}

uint8_t FuelGauge_Get_SOC()
{
    uint8_t data[2] = {0x2C, 0x2D};
    FuelGauge_ReadReg(data, data);
    vTaskDelay(pdMS_TO_TICKS(500));
    return data[0];
}

uint8_t FuelGauge_Get_Safety_Status()
{
    uint8_t data[2] = {0x1A, 0x1B};
    FuelGauge_ReadReg(data, data);
    vTaskDelay(pdMS_TO_TICKS(500));
    return data[0];
}

bool FuelGauge_is_Charging()
{
    uint8_t data[2] = {0x0A, 0x0B};
    FuelGauge_ReadReg(data, data);
    vTaskDelay(pdMS_TO_TICKS(500));
    return ~(data[0] & 0x01);
}
