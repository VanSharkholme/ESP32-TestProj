#include "ADC.h"

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t adc_cali_handle;
static adc_unit_t adc_unit;
static adc_channel_t adc_channel;
static uint32_t read_index = 0;
static SemaphoreHandle_t adc_semphr_bin = NULL;

static int voltage[100] = {0};
static int sum = 0;

void adc_init()
{
    adc_oneshot_io_to_channel(ADC_PIN, &adc_unit, &adc_channel);

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = adc_unit,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, adc_channel, &config));

    adc_calibration_init(adc_unit, adc_channel, ADC_ATTEN);

    adc_semphr_bin = xSemaphoreCreateBinary();
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten)
{
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI("ADC", "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI("ADC", "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    if (ret == ESP_OK) {
        ESP_LOGI("ADC", "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW("ADC", "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE("ADC", "Invalid arg or no memory");
    }

    return calibrated;
}

void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI("ADC", "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI("ADC", "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

void adc_read_convert(int *voltage)
{
    static int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, adc_channel, &adc_raw));
    // ESP_LOGI("ADC", "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, adc_channel, adc_raw);
    if (adc_cali_handle) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, voltage));
    }
}

static bool IRAM_ATTR adc_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t higher_prio_task_woken = pdFALSE;
    adc_result_t *user_data_ptr = (adc_result_t *)user_data;
    adc_read_convert(&user_data_ptr->data[read_index]);
    read_index++;
    if (read_index >= user_data_ptr->len) {
        read_index = 0;
        xSemaphoreGiveFromISR(adc_semphr_bin, &higher_prio_task_woken);
        gptimer_stop(timer);
    }
    return higher_prio_task_woken == pdTRUE;
}

void adc_timer_init(uint32_t sample_freq, adc_result_t *adc_res_buffer, gptimer_handle_t *out_timer_handle)
{
    // ESP_LOGI("ADC", "ADC Timer Init");

    gptimer_handle_t timer_handle;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_handle));
    gptimer_event_callbacks_t timer_callbacks = {
        .on_alarm = adc_timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &timer_callbacks, adc_res_buffer));

    ESP_ERROR_CHECK(gptimer_enable(timer_handle));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000000 / sample_freq, // 1MHz / sample_freq
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm_config));

    *out_timer_handle = timer_handle;
}

void adc_continuous_read(uint32_t sample_freq, int adc_res_buffer[], uint32_t buffer_size)
{
    // ESP_LOGI("ADC", "ADC Continuous Read");
    read_index = 0;

    adc_result_t result;
    result.data = adc_res_buffer;
    result.len = buffer_size;

    gptimer_handle_t timer_handle;
    adc_timer_init(sample_freq, &result, &timer_handle);
    gptimer_start(timer_handle);
    xSemaphoreTake(adc_semphr_bin, portMAX_DELAY);
    // gptimer_stop(timer_handle);
    gptimer_disable(timer_handle);
    gptimer_del_timer(timer_handle);
}

float adc_impedance_measure()
{
    uint8_t recv_buf[6] = {0};

    ens_uart_send_cmd(0x50, 0x10, NULL, 0);
    ens_uart_recv(recv_buf, 6);
    sum = 0;
    for (uint8_t i = 0; i < 10; i++)
    {
        int max = 0;
        // ESP_LOGI("ADC", "ADC Voltage(take %d):", i);
        adc_continuous_read(40000, voltage, 100);
        for (int j = 0; j < 100; j++)
        {
            // printf("%d ", voltage[j]);
            if (max < voltage[j])
            {
                max = voltage[j];
            }
        }
        // printf("\n");
        sum += max;
        // ESP_LOGI("ADC", "Max Voltage: %d mV", max);
    }
    float ave_max = (float)sum / 10;
    float impedance = ave_max * 1000 / 64 / 33;
    ESP_LOGI("ADC", "Average Max Voltage: %.2f mV", ave_max);
    ESP_LOGI("ADC", "Impedance: %.2f", impedance);
    ens_uart_send_cmd(0x31, 0x10, NULL, 0);
    ens_uart_recv(recv_buf, 6);
    
    return impedance;

}

uint8_t adc_check_impedance()
{
    uint8_t flag = 0;
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR0, 0);
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR1, 0);
    float impedanceA = adc_impedance_measure();
    ESP_LOGI("ADC", "Impedance A: %.2f", impedanceA);
    if (impedanceA > 500)
        flag |= 1 << 0;
    
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR0, 1);
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR1, 0);
    float impedanceB = adc_impedance_measure();
    ESP_LOGI("ADC", "Impedance B: %.2f", impedanceB);
    if (impedanceB > 500)
        flag |= 1 << 1;

    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR0, 0);
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR1, 1);
    float impedanceC = adc_impedance_measure();
    ESP_LOGI("ADC", "Impedance C: %.2f", impedanceC);
    if (impedanceC > 500)
        flag |= 1 << 2;

    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR0, 1);
    TCA9555_SetPinState(TCA9555_PIN_ADC_ADDR1, 1);
    float impedanceD = adc_impedance_measure();
    ESP_LOGI("ADC", "Impedance D: %.2f", impedanceD);
    if (impedanceD > 500)
        flag |= 1 << 3;

    return flag;
}
