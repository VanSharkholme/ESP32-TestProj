#include "timer.h"

void one_sec_timer_init(void *cb, void *user_data, gptimer_handle_t *out_timer_handle)
{
    gptimer_handle_t timer_handle;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_handle));
    gptimer_event_callbacks_t timer_callbacks = {
        .on_alarm = cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &timer_callbacks, user_data));

    ESP_ERROR_CHECK(gptimer_enable(timer_handle));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000000,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm_config));

    *out_timer_handle = timer_handle;
}