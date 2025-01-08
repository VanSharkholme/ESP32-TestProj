#ifndef ADC_H
#define ADC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/gptimer.h"
#include "uart_ens.h"
#include "TCA9555.h"

#define ADC_ATTEN           ADC_ATTEN_DB_12
#define ADC_PIN 16

typedef struct {
        uint32_t len;
        int *data;
} adc_result_t;

typedef enum {
        ADC_CHANNEL_SEL_A = 0x01,
        ADC_CHANNEL_SEL_B = 0x02,
        ADC_CHANNEL_SEL_C = 0x04,
        ADC_CHANNEL_SEL_D = 0x08,
        ADC_CHANNEL_SEL_ALL = 0x0F,
} ADC_CHANNEL_SEL_t;

void adc_init();
bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten);
void adc_calibration_deinit(adc_cali_handle_t handle);
void adc_read_convert(int *voltage);
void adc_continuous_read(uint32_t sample_freq, int adc_res_buffer[], uint32_t buffer_size);
float adc_impedance_measure(uint8_t channel_sel);
bool adc_check_impedance(uint8_t channel);

#endif // !ADC_H
