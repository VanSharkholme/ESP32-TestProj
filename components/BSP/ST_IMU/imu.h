#ifndef __IMU_H__
#define __IMU_H__

#include "esp_err.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "I2C.h"
#include "PCA9546.h"
#include "imu_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define IMU_I2C_ADDR 0x6A
#define IMU_I2C_CHANNEL 1
#define IMU_BATCH_SIZE 256

typedef enum
{
    IMU_Accel_ODR_Off = 0,
    IMU_Accel_ODR_1_6Hz = 0b1011,
    IMU_Accel_ODR_12_5Hz = 0b0001,
    IMU_Accel_ODR_26Hz = 0b0010,
    IMU_Accel_ODR_52Hz = 0b0011,
    IMU_Accel_ODR_104Hz = 0b0100,
    IMU_Accel_ODR_208Hz = 0b0101,
    IMU_Accel_ODR_416Hz = 0b0110,
    IMU_Accel_ODR_833Hz = 0b0111,
    IMU_Accel_ODR_1660Hz = 0b1000,
    IMU_Accel_ODR_3330Hz = 0b1001,
    IMU_Accel_ODR_6660Hz = 0b1010,
} IMU_Accel_ODR_t;

typedef enum
{
    IMU_Gyro_ODR_Off = 0,
    IMU_Gyro_ODR_1_6Hz = 0b1011,
    IMU_Gyro_ODR_12_5Hz = 0b0001,
    IMU_Gyro_ODR_26Hz = 0b0010,
    IMU_Gyro_ODR_52Hz = 0b0011,
    IMU_Gyro_ODR_104Hz = 0b0100,
    IMU_Gyro_ODR_208Hz = 0b0101,
    IMU_Gyro_ODR_416Hz = 0b0110,
    IMU_Gyro_ODR_833Hz = 0b0111,
    IMU_Gyro_ODR_1660Hz = 0b1000,
    IMU_Gyro_ODR_3330Hz = 0b1001,
    IMU_Gyro_ODR_6660Hz = 0b1010,
} IMU_Gyro_ODR_t;

typedef struct
{
    float ax, ay, az;
} IMU_Accel_Data_t;

typedef struct
{
    float gx, gy, gz;
} IMU_Gyro_Data_t;

extern QueueHandle_t imu_read_progress_queue;
extern TaskHandle_t imu_task_handle;


void IMU_Init();
void IMU_read_reg(uint8_t reg_addr, uint8_t *data, uint8_t len);
void IMU_set_accel_odr(IMU_Accel_ODR_t odr);
void IMU_set_gyro_odr(IMU_Gyro_ODR_t odr);
void IMU_read_accel(float *ax, float *ay, float *az, uint8_t accel_scale);
void IMU_read_gyro(float *gx, float *gy, float *gz, uint16_t gyro_scale);
uint8_t IMU_get_scale_accel();
uint16_t IMU_get_scale_gyro();
void IMU_read_data_task();
// void IMU_mutex_lock();
// void IMU_mutex_unlock();


#endif