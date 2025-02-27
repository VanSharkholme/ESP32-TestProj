#include "lvgl_port.h"
#include "lv_demos.h"

static SemaphoreHandle_t lvgl_mutex = NULL;
static TaskHandle_t lvgl_task_handle = NULL;
lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
lv_disp_drv_t disp_drv;      // contains callback functions
lv_indev_drv_t indev_drv;    // contains callback functions

bool lvgl_port_notify_rgb_vsync(void)
{
    BaseType_t need_yield = pdFALSE;

    // // Notify that the current RGB frame buffer has been transmitted
    // xTaskNotifyFromISR(lvgl_task_handle, ULONG_MAX, eNoAction, &need_yield);

    return (need_yield == pdTRUE);
}

IRAM_ATTR static bool rgb_lcd_on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    return lvgl_port_notify_rgb_vsync();
}

void IRAM_ATTR lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)disp_drv->user_data;
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, color_p);
    //     /* Action after last area refresh */
    // if (lv_disp_flush_is_last(disp_drv)) {
    //     /* Switch the current RGB frame buffer to `color_map` */
    //     esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, color_p);
    //     /* Waiting for the last frame buffer to complete transmission */
    //     ulTaskNotifyValueClear(NULL, ULONG_MAX);
    //     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // }
    lv_disp_flush_ready(disp_drv);
}

void IRAM_ATTR lvgl_indev_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    int16_t x, y;
    if(!is_hibernating && GT911_Read_Coordinate(&x, &y))
    {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
        reset_hibernation_timer();
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    // if (data->state == LV_INDEV_STATE_PRESSED)
    // {
    //     ESP_LOGI("LVGL_INDEV", "X: %d, Y: %d", data->point.x, data->point.y);
    // }
    // else
    // {
    //     ESP_LOGI("LVGL_INDEV", "Released");
    // }
    
    // data->continue_reading = false;
}

void increase_lvgl_tick(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

bool lvgl_lock(void)
{
    return xSemaphoreTakeRecursive(lvgl_mutex, portMAX_DELAY) == pdTRUE;
}

void lvgl_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_mutex);
}

void lvgl_timer_handler_task(void *arg)
{
    uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (lvgl_lock()) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            lvgl_unlock();
        }
        if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

void lvgl_init_setup(void)
{
    ESP_LOGI("LVGL_INIT", "Starting LVGL Init");
    lv_init();
    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, RESOLUTION_X * RESOLUTION_Y);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = RESOLUTION_X;
    disp_drv.ver_res = RESOLUTION_Y;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.user_data = panel_handle;
    // disp_drv.full_refresh = true;
    disp_drv.direct_mode = true;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_indev_read_cb;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    lvgl_mutex = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_mutex);
    xTaskCreate(lvgl_timer_handler_task, "lvgl_timer_handler_task", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, &lvgl_task_handle);
    // xTaskCreatePinnedToCore(lvgl_timer_handler_task, "lvgl_timer_handler_task", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, &lvgl_task_handle, 1);

    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = rgb_lcd_on_vsync_event,
    };
    esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv);
    
}