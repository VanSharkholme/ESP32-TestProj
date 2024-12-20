#include "PCA9546.h"

i2c_master_dev_handle_t PCA9546_dev_handle = NULL;

void PCA9546_Dev_Init(i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == NULL)
    {
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
    }

    i2c_device_config_t PCA9546_config = {
       .dev_addr_length = I2C_ADDR_BIT_LEN_7,
       .device_address = PCA9546_I2C_ADDR,
       .scl_speed_hz = 400000,
    };

    if(ESP_OK == i2c_master_probe(bus_handle, PCA9546_I2C_ADDR, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", PCA9546_config.device_address);
    }

    PCA9546_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &PCA9546_config, &PCA9546_dev_handle));
}

void PCA9546_Init()
{
    if (i2c_bus1_handle == NULL)
    {
        I2C_Init();
    }

    PCA9546_Dev_Init(i2c_bus1_handle);

}

void PCA9546_SelectChannel(uint8_t channel)
{
    uint8_t data;
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_receive(PCA9546_dev_handle, &data, 1, -1);
    data |= 1 << channel;
    i2c_master_transmit(PCA9546_dev_handle, &data, 1, -1);
    xSemaphoreGive(i2c_bus1_mutex);
}

void PCA9546_DeselectChannel(uint8_t channel)
{
    uint8_t data = 0;
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_receive(PCA9546_dev_handle, &data, 1, -1);
    data &= ~(1 << channel);
    i2c_master_transmit(PCA9546_dev_handle, &data, 1, -1);
    xSemaphoreGive(i2c_bus1_mutex);
}

void PCA9546_EnableAllChannels()
{
    uint8_t data = 0xFF;
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit(PCA9546_dev_handle, &data, 1, -1);
    xSemaphoreGive(i2c_bus1_mutex);
}
