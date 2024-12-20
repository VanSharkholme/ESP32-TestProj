#include "imu.h"

i2c_master_dev_handle_t IMU_dev_handle = NULL;
static uint16_t read_progress = 0;
QueueHandle_t imu_read_progress_queue = NULL;
TaskHandle_t imu_task_handle = NULL;
static IMU_Accel_Data_t accel_data[IMU_BATCH_SIZE];
static IMU_Gyro_Data_t gyro_data[IMU_BATCH_SIZE];
// static SemaphoreHandle_t IMU_progress_mutex = NULL;

void IMU_Dev_Init(i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == NULL)
    {
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
    }

    i2c_device_config_t IMU_config = {
       .dev_addr_length = I2C_ADDR_BIT_LEN_7,
       .device_address = IMU_I2C_ADDR,
       .scl_speed_hz = 400000,
    };
    PCA9546_SelectChannel(IMU_I2C_CHANNEL);
    if(ESP_OK == i2c_master_probe(bus_handle, IMU_I2C_ADDR, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", IMU_config.device_address);
    }
    PCA9546_DeselectChannel(IMU_I2C_CHANNEL);
    IMU_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &IMU_config, &IMU_dev_handle));
}

void IMU_Init()
{
    if (i2c_bus1_handle == NULL)
    {
        I2C_Init();
    }

    IMU_Dev_Init(i2c_bus1_handle);
    uint8_t IMU_reg_data;
    IMU_read_reg(0x0F, &IMU_reg_data, 1);
    if (IMU_reg_data == 0x6A)
    {
        ESP_LOGI("IMU", "IMU is detected");
        imu_read_progress_queue = xQueueCreate(10, sizeof(uint16_t));
    }
    // IMU_progress_mutex = xSemaphoreCreateRecursiveMutex();
    
}

void IMU_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    PCA9546_SelectChannel(IMU_I2C_CHANNEL);
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit_receive(IMU_dev_handle, &reg_addr, 1, data, len, -1);
    xSemaphoreGive(i2c_bus1_mutex);
    PCA9546_DeselectChannel(IMU_I2C_CHANNEL);
}


void IMU_write_reg(uint8_t reg_addr, uint8_t *data, uint8_t len)
{
    uint8_t tx_data[len+1];
    tx_data[0] = reg_addr;
    for (int i=0; i<len; i++)
    {
        tx_data[i+1] = data[i];
    }
    PCA9546_SelectChannel(IMU_I2C_CHANNEL);
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit(IMU_dev_handle, tx_data, len+1, -1);
    xSemaphoreGive(i2c_bus1_mutex);
    PCA9546_DeselectChannel(IMU_I2C_CHANNEL);
}

void IMU_set_accel_odr(IMU_Accel_ODR_t odr)
{
    uint8_t reg_data;
    IMU_read_reg(IMU_REG_CTRL1_XL, &reg_data, 1);
    reg_data &= (0x0F);
    reg_data |= (odr << 4);
    IMU_write_reg(IMU_REG_CTRL1_XL, &reg_data, 1);
}

void IMU_set_gyro_odr(IMU_Gyro_ODR_t odr)
{
    uint8_t reg_data;
    IMU_read_reg(IMU_REG_CTRL2_G, &reg_data, 1);
    reg_data &= (0x0F);
    reg_data |= (odr << 4);
    IMU_write_reg(IMU_REG_CTRL2_G, &reg_data, 1);
}

void IMU_read_accel_raw(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t reg_data[6];
    IMU_read_reg(IMU_REG_OUTX_L_XL, reg_data, 6);
    *ax = (int16_t)(reg_data[0] | (reg_data[1] << 8));
    *ay = (int16_t)(reg_data[2] | (reg_data[3] << 8));
    *az = (int16_t)(reg_data[4] | (reg_data[5] << 8));
    // ESP_LOGI("IMU", "Accel: x:%d, y:%d, z:%d", *ax, *ay, *az);
}

void IMU_read_gyro_raw(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t reg_data[6];
    IMU_read_reg(IMU_REG_OUTX_L_G, reg_data, 6);
    *gx = (int16_t)(reg_data[0] | (reg_data[1] << 8));
    *gy = (int16_t)(reg_data[2] | (reg_data[3] << 8));
    *gz = (int16_t)(reg_data[4] | (reg_data[5] << 8));
    // ESP_LOGI("IMU", "Gyro: x:%d, y:%d, z:%d", *gx, *gy, *gz);
}

uint8_t IMU_get_scale_accel()
{
    uint8_t reg_data, scale;
    IMU_read_reg(IMU_REG_CTRL1_XL, &reg_data, 1);
    reg_data = (reg_data >> 2) & 0x03;
    switch (reg_data)
    {
    case 0b00:
        scale = 2;
        break;
    case 0b01:
        scale = 16;
        break;
    case 0b10:
        scale = 4;
        break;
    case 0b11:
        scale = 8;
        break;
    default:
        scale = 0;
        break;
    }
    return scale;
}

uint16_t IMU_get_scale_gyro()
{
    uint16_t reg_data, scale;
    IMU_read_reg(IMU_REG_CTRL2_G, &reg_data, 1);
    if (reg_data & 0x02) {
        return 125;
    } else {
        reg_data = (reg_data >> 2) & 0x03;
        switch (reg_data)
        {
        case 0b00:
            scale = 245;
            break;
        case 0b01:
            scale = 500;
            break;
        case 0b10:
            scale = 1000;
            break;
        case 0b11:
            scale = 2000;
            break;        
        default:
            scale = 0;
            break;
        }
        return scale;
    }
    

}

void IMU_read_accel(float *ax, float *ay, float *az, uint8_t scale_accel)
{
    int16_t raw_ax, raw_ay, raw_az;
    IMU_read_accel_raw(&raw_ax, &raw_ay, &raw_az);
    *ax = (float)raw_ax * scale_accel / 32768.0f;
    *ay = (float)raw_ay * scale_accel / 32768.0f;
    *az = (float)raw_az * scale_accel/ 32768.0f;
    ESP_LOGI("IMU", "Accel(g): x:%.2f, y:%.2f, z:%.2f", *ax, *ay, *az);
}

void IMU_read_gyro(float *gx, float *gy, float *gz, uint16_t scale_gyro)
{
    int16_t raw_gx, raw_gy, raw_gz;
    IMU_read_gyro_raw(&raw_gx, &raw_gy, &raw_gz);
    *gx = (float)raw_gx * scale_gyro / 32768.0f;
    *gy = (float)raw_gy * scale_gyro / 32768.0f;
    *gz = (float)raw_gz * scale_gyro / 32768.0f;
    ESP_LOGI("IMU", "Gyro(deg/s): x:%.2f, y:%.2f, z:%.2f", *gx, *gy, *gz);
}

void IMU_read_data_task()
{
    IMU_set_accel_odr(IMU_Accel_ODR_416Hz);
    IMU_set_gyro_odr(IMU_Gyro_ODR_416Hz);
    uint8_t scale_accel = IMU_get_scale_accel();
    uint16_t scale_gyro = IMU_get_scale_gyro();
    read_progress = 0;
    while (read_progress < IMU_BATCH_SIZE)
    {
        IMU_read_accel(&accel_data[read_progress].ax, &accel_data[read_progress].ay, &accel_data[read_progress].az, scale_accel);
        IMU_read_gyro(&gyro_data[read_progress].gx, &gyro_data[read_progress].gy, &gyro_data[read_progress].gz, scale_gyro);
        // xSemaphoreTakeRecursive(IMU_progress_mutex, portMAX_DELAY);
        read_progress++;
        xQueueSend(imu_read_progress_queue, &read_progress, portMAX_DELAY);
        // xSemaphoreGiveRecursive(IMU_progress_mutex);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

// void IMU_mutex_lock()
// {
//     xSemaphoreTakeRecursive(IMU_progress_mutex, portMAX_DELAY);
// }

// void IMU_mutex_unlock()
// {
//     xSemaphoreGiveRecursive(IMU_progress_mutex);
// }





