#ifndef __TIMER_H__
#define __TIMER_H__

#include "esp_log.h"
#include "driver/gptimer.h"


void one_sec_timer_init(void *cb, void *user_data, gptimer_handle_t *out_timer_handle);


#endif