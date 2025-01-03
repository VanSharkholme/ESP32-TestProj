
#include "ADC.h"
#include "rgb_lcd.h"
#include "lvgl.h"
#include "I2C.h"
#include "GT911.h"
#include "lvgl_port.h"
#include "FuelGauge.h"
#include "PCA9546.h"
#include "TCA9555.h"
#include "CustomUI.h"
#include "uart_ens.h"
#include "bluetooth.h"
#include "imu.h"
#include "nvs_op.h"

uint16_t colors[8] = {0xFFFF, 0x0000, 0xFF00, 0x00FF, 0xFFF0, 0x000F, 0xF0F0, 0x0F0F};
IMU_Accel_Data_t accel_data[256] = {0};
IMU_Gyro_Data_t gyro_data[256] = {0};


uint8_t recv_buf[7] = {0};
const static char TAG[] = "MAIN";
static bool is_charging = false;

void test_task(void *arg)
{
    int i = 0;
    uint8_t j = 0;

    // adc_read_start(voltage);
    for(;;)
    {
        bool current_charging_state = !TCA9555_GetPinState(TCA9555_PIN_CHR_STA_IND);
        if (is_charging != current_charging_state && current_charging_state == 1)
        {
            is_charging = true;
            // load_charging_scr();
            TCA9555_SetPinState(TCA9555_PIN_YELLOW, 0);
        }
        else if (is_charging != current_charging_state && current_charging_state == 0)
        {
            is_charging = false;
            // exit_charging_scr();
            TCA9555_SetPinState(TCA9555_PIN_YELLOW, 1);
        }

        uint8_t buf[2] = {0x2C, 0x2D};
        FuelGauge_ReadReg(buf, buf);
        ESP_LOGI(TAG, "Fuel Gauge: %02X %02X", buf[0], buf[1]); 
        set_battery_level(buf[0]);
        // rgb_lcd_brightness(j + 10);
        j = (j + 10) % 100;

        // buf[0] = 0x0A;
        // buf[1] = 0x0B;
        // FuelGauge_ReadReg(buf, buf);

        // adc_read_convert(&adc1_handle, &adc1_cali_chan0_handle, &voltage);
        // ble_send_data(recv_buf, 6);
        // uint16_t ax, ay, az;
        // IMU_read_accel(&ax, &ay, &az);
        // ESP_LOGI(TAG, "Accelerometer: x:%d, y:%d, z:%d", ax, ay, az);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void imu_task(void *arg)
{
    float ax, ay, az, gx, gy, gz;
    IMU_set_accel_odr(IMU_Accel_ODR_416Hz);
    IMU_set_gyro_odr(IMU_Gyro_ODR_416Hz);
    uint8_t scale_accel = IMU_get_scale_accel();
    uint8_t scale_gyro = IMU_get_scale_gyro();
    ESP_LOGI(TAG, "Accelerometer scale: %d", scale_accel);
    ESP_LOGI(TAG, "Gyroscope scale: %d", scale_gyro);
    while(1)
    {
        IMU_read_accel(&ax, &ay, &az, scale_accel);
        IMU_read_gyro(&gx, &gy, &gz, scale_gyro);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{

    // while(1);
    ens_uart_init();
    uint8_t test_buf[7] = {0x5A, 0xA5, 0x03, 0x40, 0x10, 1, 0x54};
    uint8_t test_buf2[7] = {0x5A, 0xA5, 0x03, 0x40, 0x10, 60, 0x8F};
    adc_init();
    nvs_op_init();
    // nvs_test();
    // char *bt_name;
    // bt_name = nvs_get_bluetooth_name();
    // printf("Bluetooth Name: %s\n", bt_name);
    // bt_name[0] = bt_name[0] == 'E' ? 'T' : 'E';
    // nvs_save_bluetooth_name(bt_name);
    rgb_lcd_init();
    // reset_schemeset();
    init_plans();
    bluetooth_init();
    // ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    // ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << 4;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(GPIO_NUM_4, 1);
    I2C_Init();
    PCA9546_Init();
    // PCA9546_EnableAllChannels();
    // while(1);
    TCA9555_Init();
    // TCA9554_Init();
    TCA9555_SetPinState(TCA9555_PIN_BL_EN, 1);
    TCA9555_SetPinState(TCA9555_PIN_GREEN, 0);
    TCA9555_SetPinState(TCA9555_PIN_YELLOW, 0);

    TCA9555_SetPinState(TCA9555_PIN_5V_PWR, 1);
    TCA9555_SetPinState(TCA9555_PIN_ADC_EN, 1);

    // TCA9554_SetPinState(TCA9554_PIN_2, 1);
    // TCA9554_SetPinState(TCA9554_PIN_5, 0);
    // TCA9554_SetPinState(TCA9554_PIN_6, 0);
    FuelGauge_Init();
    FuelGauge_Get_SOC();
    GT_Init();
    IMU_Init();
    lvgl_init_setup();
    if (lvgl_lock())
    {
        // lv_example_get_started_2();
        CustomUI();
        lvgl_unlock();
    }

    


    //xTaskCreate(test_task, "test_task", 4096, NULL, 1, NULL);

    xTaskCreate(tca9555_task, "tca9555_task", 4096, NULL, 1, NULL);

    // IMU_read_data(accel_data, gyro_data, 256);
    // for (uint16_t i = 0; i < 256; i++)
    // {
    //     ESP_LOGI(TAG, "Accel Data: %.2f, %.2f, %.2f", accel_data[i].ax, accel_data[i].ay, accel_data[i].az);
    //     ESP_LOGI(TAG, "Gyro Data: %.2f, %.2f, %.2f", gyro_data[i].gx, gyro_data[i].gy, gyro_data[i].gz);
    // }
    

    // xTaskCreate(imu_task, "imu_task", 4096, NULL, 3, NULL);
}
