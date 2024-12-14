//
// Created by 13651 on 2024/12/4.
//
#if 0
#ifndef LVGL_DATA_STRUCTURES_H
#define LVGL_DATA_STRUCTURES_H

#include "stdint.h"

typedef enum {
    WAVE_TYPE_DUAL_PHASE_SQUARE = 0b00,
    WAVE_TYPE_SINGLE_PHASE_SQUARE = 0b01,
    WAVE_TYPE_TRIANGLE = 0b10,
    WAVE_TYPE_SINE = 0b11
} waveType_t;

typedef enum {
    FREQ_TYPE_FIXED = 0b00,
    FREQ_TYPE_VARY = 0b01,
} freqType_t;

typedef struct
{
    uint8_t id;
    uint8_t name_len;
    char name[30];
    waveType_t wave_type;
    uint8_t current_mA;
    uint16_t pulse_width;
    uint16_t pulse_interval;
    freqType_t freq_type;
    uint16_t freq_min;
    uint16_t freq_max;
    uint16_t wave_rise;
    uint16_t wave_fall;
    uint8_t work_time;
    uint8_t break_time;
    uint8_t total_time_min;
} Plan;


#endif //LVGL_DATA_STRUCTURES_H

#endif