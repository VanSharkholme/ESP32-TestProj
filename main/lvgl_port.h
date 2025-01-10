#ifndef LVGL_PORT_H_
#define LVGL_PORT_H_

#include "rgb_lcd.h"
#include "lvgl.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "GT911.h"
#include "esp_timer.h"
#include "CustomUI.h"

#define LVGL_TICK_PERIOD_MS 3
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1
#define LVGL_TASK_STACK_SIZE 4096
#define LVGL_TASK_PRIORITY 5

bool lvgl_lock(void);
void lvgl_unlock(void);
void lvgl_init_setup(void);

#endif // !LVGL_PORT_H_
