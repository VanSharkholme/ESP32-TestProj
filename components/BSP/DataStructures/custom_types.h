#ifndef CUSTOM_TYPES_H_
#define CUSTOM_TYPES_H_

#include <stdint.h>

#define MAX_PLAN_NUM 15

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

typedef enum {
    SCHEME_TYPE_NMES,
    SCHEME_TYPE_TENS,
    SCHEME_TYPE_MNS,
    SCHEME_TYPE_TNS,
    SCHEME_TYPE_OTHER
} schemeType_t;

typedef struct
{
    uint8_t id;
    schemeType_t scheme_type; // 模式
    uint8_t name_len;
    char name[35];
    waveType_t wave_type;
    uint8_t current_mA;
    uint16_t pulse_width;
    uint8_t pulse_interval;
    freqType_t freq_type;
    uint16_t freq_min;
    uint16_t freq_max;
    uint16_t wave_rise;
    uint16_t wave_fall;
    uint8_t work_time;
    uint8_t break_time;
    uint8_t total_time_min;
} Plan;

typedef struct {
    char *name;
    uint8_t plan_num;
    Plan *plans[MAX_PLAN_NUM];
} SchemeSet;

typedef SchemeSet * pSchemeSet;
typedef pSchemeSet * SchemePage;



#endif /* CUSTOM_TYPES_H_ */