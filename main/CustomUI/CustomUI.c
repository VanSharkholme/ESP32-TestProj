//
// Created by 13651 on 2024/11/16.
//

#include "CustomUI.h"

lv_obj_t *main_scr;
lv_obj_t *scheme_scr;
lv_obj_t *calib_scr;
lv_obj_t *charging_scr;

UI_Channel ChannelA;
UI_Channel ChannelB;
UI_Channel ChannelC;
UI_Channel ChannelD;

lv_obj_t *lv_channel_a;
lv_obj_t *lv_channel_b;
lv_obj_t *lv_channel_c;
lv_obj_t *lv_channel_d;

PlanPage schemePages[UI_PLAN_PAGE_NUM_MAX];
pPlan page1[UI_PLAN_NUM_PER_PAGE];
pPlan page2[UI_PLAN_NUM_PER_PAGE];
pPlan page3[UI_PLAN_NUM_PER_PAGE];
uint8_t valid_page_num = 0;
uint8_t current_page_num = 0;

static uint8_t calib_cnt = 0;
static uint8_t to_modify_plan_ids[4] = {255, 255, 255, 255};
static uint8_t to_modify_plan_cnt = 0;

void Channel_Struct_Init(UI_Channel *channel, char *name, uint8_t index) {
    channel->name = name;
    channel->index = index;
//    channel->p_uiplan = NULL;
    channel->pPlan = NULL;
    channel->timer.remaining_seconds = 0;
    channel->timer.state = UI_TIMER_STATE_UNINIT;
    channel->state = UI_CHANNEL_STATE_UNINIT;
}

void Channel_Struct_Clear(UI_Channel *channel, bool need_to_free_pPlan)
{
    uint8_t i = 0;
    if (need_to_free_pPlan)
    {
        free(channel->pPlan);
        channel->pPlan = NULL;
    }
    channel->timer.remaining_seconds = 0;
    channel->timer.state = UI_TIMER_STATE_UNINIT;
}

void addCallbackforImgBtn(lv_obj_t *btn) {
    lv_obj_add_event_cb(btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);
}

lv_obj_t *create_warning_modal(char *msg)
{
    lv_obj_t *modal_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(modal_container, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_bg_opa(modal_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(modal_container, LV_OPA_TRANSP, 0);

    lv_obj_t *modal_bg = lv_obj_create(modal_container);
    lv_obj_set_size(modal_bg, 292, 178);
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_set_style_pad_all(modal_bg, 0, 0);
    lv_obj_center(modal_bg);
    lv_obj_add_flag(modal_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *modal_title = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_title, 292, 70);
    lv_obj_set_style_bg_opa(modal_title, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_title, 0, 0);

    lv_obj_t *warning_icon = lv_img_create(modal_title);
    lv_img_set_src(warning_icon, &WarningIcon_fit);
    lv_obj_align(warning_icon, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    lv_obj_t *modal_title_label = lv_label_create(modal_title);
    lv_label_set_text(modal_title_label, "提示");
    lv_obj_set_style_text_color(modal_title_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_title_label, &AliPuHui_30, 0);
    lv_obj_align_to(modal_title_label, warning_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *modal_content = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_content, 292, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(modal_content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_content, 0, 0);
    lv_obj_center(modal_content);

    lv_obj_t *modal_content_label = lv_label_create(modal_content);
    lv_label_set_text(modal_content_label, msg);
    lv_obj_set_style_text_color(modal_content_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_content_label, &AliPuHui_24, 0);
    lv_obj_set_style_text_align(modal_content_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(modal_content_label);

    lv_obj_t *modal_btn_container = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_btn_container, 292, 60);
    lv_obj_set_style_bg_opa(modal_btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(modal_btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_btn_container, 0, 0);
    lv_obj_align(modal_btn_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(modal_btn_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *modal_btn_cancel_label = lv_label_create(modal_btn_container);
    lv_label_set_text(modal_btn_cancel_label, "取消");
    lv_obj_set_style_text_color(modal_btn_cancel_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_btn_cancel_label, &AliPuHui_24, 0);
    lv_obj_align(modal_btn_cancel_label, LV_ALIGN_LEFT_MID, 30, 0);
    lv_obj_add_event_cb(modal_btn_cancel_label, CurrentWarningModalCancelCallback, LV_EVENT_CLICKED, modal_container);
    lv_obj_add_flag(modal_btn_cancel_label, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *mid_line = lv_obj_create(modal_btn_container);
    lv_obj_set_size(mid_line, 1, 34);
    lv_obj_set_style_bg_color(mid_line, lv_color_white(), 0);
    lv_obj_align(mid_line, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t *modal_btn_confirm_label = lv_label_create(modal_btn_container);
    lv_label_set_text(modal_btn_confirm_label, "继续");
    lv_obj_set_style_text_color(modal_btn_confirm_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_btn_confirm_label, &AliPuHui_24, 0);
    lv_obj_align(modal_btn_confirm_label, LV_ALIGN_RIGHT_MID, -30, 0);
    lv_obj_add_event_cb(modal_btn_confirm_label, CurrentWarningModalCancelCallback, LV_EVENT_CLICKED, modal_container);
    lv_obj_add_flag(modal_btn_confirm_label, LV_OBJ_FLAG_CLICKABLE);

    return modal_container;
}

lv_obj_t *create_current_warning_modal(lv_obj_t *in_channel)
{
    lv_obj_t *modal_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(modal_container, lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_bg_opa(modal_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(modal_container, LV_OPA_TRANSP, 0);
    lv_obj_set_user_data(modal_container, in_channel);

    lv_obj_t *modal_bg = lv_obj_create(modal_container);
    lv_obj_set_size(modal_bg, 292, 178);
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_set_style_pad_all(modal_bg, 0, 0);
    lv_obj_center(modal_bg);
    lv_obj_add_flag(modal_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *modal_title = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_title, 292, 70);
    lv_obj_set_style_bg_opa(modal_title, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_title, 0, 0);

    lv_obj_t *warning_icon = lv_img_create(modal_title);
    lv_img_set_src(warning_icon, &WarningIcon_fit);
    lv_obj_align(warning_icon, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    lv_obj_t *modal_title_label = lv_label_create(modal_title);
    lv_label_set_text(modal_title_label, "提示");
    lv_obj_set_style_text_color(modal_title_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_title_label, &AliPuHui_30, 0);
    lv_obj_align_to(modal_title_label, warning_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_t *modal_content = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_content, 292, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(modal_content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_content, 0, 0);
    lv_obj_center(modal_content);

    lv_obj_t *modal_content_label = lv_label_create(modal_content);
    lv_label_set_text_fmt(modal_content_label, "电流已超过%d\n是否继续上调？", UI_WARNING_CURRENT_LIMIT);
    lv_obj_set_style_text_color(modal_content_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_content_label, &AliPuHui_24, 0);
    lv_obj_center(modal_content_label);

    lv_obj_t *modal_btn_container = lv_obj_create(modal_bg);
    lv_obj_set_size(modal_btn_container, 292, 60);
    lv_obj_set_style_bg_opa(modal_btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(modal_btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(modal_btn_container, 0, 0);
    lv_obj_align(modal_btn_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(modal_btn_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *modal_btn_cancel_label = lv_label_create(modal_btn_container);
    lv_label_set_text(modal_btn_cancel_label, "取消");
    lv_obj_set_style_text_color(modal_btn_cancel_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_btn_cancel_label, &AliPuHui_24, 0);
    lv_obj_align(modal_btn_cancel_label, LV_ALIGN_LEFT_MID, 30, 0);
    lv_obj_add_event_cb(modal_btn_cancel_label, CurrentWarningModalCancelCallback, LV_EVENT_CLICKED, modal_container);
    lv_obj_add_flag(modal_btn_cancel_label, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *mid_line = lv_obj_create(modal_btn_container);
    lv_obj_set_size(mid_line, 1, 34);
    lv_obj_set_style_bg_color(mid_line, lv_color_white(), 0);
    lv_obj_align(mid_line, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *modal_btn_confirm_label = lv_label_create(modal_btn_container);
    lv_label_set_text(modal_btn_confirm_label, "继续");
    lv_obj_set_style_text_color(modal_btn_confirm_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(modal_btn_confirm_label, &AliPuHui_24, 0);
    lv_obj_align(modal_btn_confirm_label, LV_ALIGN_RIGHT_MID, -30, 0);
    lv_obj_add_event_cb(modal_btn_confirm_label, CurrentWarningModalConfirmCallback, LV_EVENT_CLICKED, modal_container);
    lv_obj_add_flag(modal_btn_confirm_label, LV_OBJ_FLAG_CLICKABLE);

    return modal_container;
}

uint8_t get_channel_with_same_plan(uint8_t plan_id)
{
    uint8_t indicator = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
        if (ch->state == UI_CHANNEL_STATE_ADDED && ch->pPlan->id == plan_id)
        {
            indicator |= (1 << i);
        }
    }
    return indicator;
}

void set_channel_current_by_force(lv_obj_t *in_channel, uint8_t current)
{
    UI_Channel *in_ch = (UI_Channel *) lv_obj_get_user_data(in_channel);
    uint8_t channel_indicator = get_channel_with_same_plan(in_ch->pPlan->id);
    in_ch->pPlan->current_mA = current;
    for (uint8_t i = 0; i < 4; ++i) {
        if (channel_indicator & (1 << i))
        {
            lv_obj_t *channel = get_channel_by_index(i);
            UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
            lv_obj_t *current_container = lv_obj_get_child(channel, 2);
            lv_obj_t *current_arc = lv_obj_get_child(current_container, 2);
            lv_arc_set_value(current_arc, current);
            lv_obj_t *current_label = lv_obj_get_child(current_arc, 0);
            lv_label_set_text_fmt(current_label, "%d", current);
        }
    }
}

void refresh_channel_current(lv_obj_t *in_channel, int8_t difference, bool is_force_refresh)
{
    UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(in_channel);
    int8_t current = ch->pPlan->current_mA;
    if (current + difference < 0)
        current = 0;
    else if (!is_force_refresh && current + difference == (UI_WARNING_CURRENT_LIMIT + 1) && difference > 0)
    {
        create_current_warning_modal(in_channel);
    }
    else if (current + difference > 100)
        current = 100;
    else
        current += difference;
    ch->pPlan->current_mA = current;
//    ui_ch->pPlan->g_chanelConfig[ui_ch->index].configParameter.current = current;
    // uint8_t channel_indicator = get_channel_with_same_plan(ch->pPlan->id);
    // for (uint8_t i = 0; i < 4; ++i) {
    //     if (channel_indicator & (1 << i))
    //     {
    lv_obj_t *channel = in_channel;
    lv_obj_t *current_container = lv_obj_get_child(channel, 2);
    lv_obj_t *current_arc = lv_obj_get_child(current_container, 2);
    lv_arc_set_value(current_arc, current);
    lv_obj_t *current_label = lv_obj_get_child(current_arc, 0);
    lv_label_set_text_fmt(current_label, "%d", current);
        // }
    // }

}

// bool channel_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
// {
//     lv_obj_t *channel = (lv_obj_t *) user_data;
//     UI_Channel *ch = lv_obj_get_user_data(channel);
//     ch->timer.remaining_seconds--;
//     lv_obj_t *timer_container = lv_obj_get_child(channel, 1);
//     lv_obj_t *timer_label = lv_obj_get_child(timer_container, 0);
//     lv_label_set_text_fmt(timer_label, "%02d:%02d", ch->timer.remaining_seconds / 60, ch->timer.remaining_seconds % 60);
//     if (ch->timer.remaining_seconds == 0) {
//         set_channel_timer_state(channel, UI_TIMER_STATE_STOP);
//     }
//     return false;
// }
void channel_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *channel = (lv_obj_t *) timer->user_data;
    UI_Channel *ch = lv_obj_get_user_data(channel);
    ch->timer.remaining_seconds--;
    lv_obj_t *timer_container = lv_obj_get_child(channel, 1);
    lv_obj_t *timer_label = lv_obj_get_child(timer_container, 0);
    lv_label_set_text_fmt(timer_label, "%02d:%02d", ch->timer.remaining_seconds / 60, ch->timer.remaining_seconds % 60);
    if (ch->timer.remaining_seconds == 0) {
        set_channel_timer_state(channel, UI_TIMER_STATE_STOP);
    }
    // return false;
}

void check_last_channel_finished(void)
{
    bool all_finished = true;
    for (uint8_t i = 0; i < 4; ++i) {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
        if (ch->state == UI_CHANNEL_STATE_ADDED)
            all_finished &= ch->timer.state == UI_TIMER_STATE_STOP;
    }
    if (all_finished)
    {
        lv_obj_t *btn = lv_obj_get_child(main_scr, 3);
        bool *is_stimulation_running = (bool *) lv_obj_get_user_data(btn);
        *is_stimulation_running = false;
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Green_fit, NULL);
        statework.st_bit.StRun = 0;
    }    
}

void ens_stop_single_channel_plan(Plan *plan, uint8_t index)
{
    ens_uart_send_cmd(0x31, 0x10, NULL, 0);
    uint8_t ch_w_same_plan = get_channel_with_same_plan(plan->id);
    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
        if (ch_w_same_plan & (1 << i))
            continue;
        if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state == UI_TIMER_STATE_START)
        {
            uint8_t channel_indicator = 1 << i;
            ens_uart_send_cmd(0x30, 0x10, &channel_indicator, 1);
        }
    }
}

void set_channel_timer_state(lv_obj_t *channel, UI_ChannelTimerState state)
{
    UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
    lv_obj_t *timer_container = lv_obj_get_child(channel, 1);
    ch->timer.state = state;
    if (ch->timer.state == UI_TIMER_STATE_STOP)
    {
        lv_obj_set_style_bg_color(timer_container, lv_color_hex(0xEFF4FE), 0);
        ens_stop_channel_plan(ch->pPlan, ch->index);
        lv_timer_pause(ch->timer.lvtimer);
        // gptimer_stop(ch->timer.timer_handle);
        if (ch->timer.remaining_seconds == 0)
        {
            // gptimer_disable(ch->timer.timer_handle);
            // gptimer_del_timer(ch->timer.timer_handle);
            lv_timer_del(ch->timer.lvtimer);
            ch->timer.remaining_seconds = ch->pPlan->total_time_min * 60;
        }
        

        check_last_channel_finished();
    }
    else if (ch->timer.state == UI_TIMER_STATE_START)
    {
        lv_obj_set_style_bg_color(timer_container, lv_color_hex(0xA9EAE3), 0);
        ens_start_channel_plan(ch->pPlan, ch->index);
        if (ch->timer.remaining_seconds == ch->pPlan->total_time_min * 60)
        {
            // one_sec_timer_init(channel_timer_cb, channel, &ch->timer.timer_handle);
            ch->timer.lvtimer = lv_timer_create(channel_timer_cb, 1000, channel);
            lv_timer_set_repeat_count(ch->timer.lvtimer, ch->pPlan->total_time_min * 60);
            lv_timer_ready(ch->timer.lvtimer);
        }
        lv_timer_resume(ch->timer.lvtimer);
        // gptimer_start(ch->timer.timer_handle);
        statework.st_bit.StRun = 1;

    }


}

void set_channel_plan(uint8_t index, Plan *plan, bool is_force_refresh)
{
    lv_obj_t *channel = get_channel_by_index(index);
    UI_Channel *ch = (UI_Channel *)lv_obj_get_user_data(channel);
    ch->pPlan = malloc(sizeof(Plan));
    memcpy(ch->pPlan, plan, sizeof(Plan));
    ch->pPlan->id = index + 1;
    ch->timer.remaining_seconds = plan->total_time_min * 60;
    set_channel_state(channel, UI_CHANNEL_STATE_ADDED, is_force_refresh);
}

uint8_t find_spare_channel(uint8_t *indicator)
{
    uint8_t spare_channel_num = 0;
    *indicator = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
        if (ch->state == UI_CHANNEL_STATE_NOT_ADDED)
        {
            *indicator |= (1 << i);
            spare_channel_num++;
        }
    }
    return spare_channel_num;    
}

void set_channel_state(lv_obj_t *channel, UI_ChannelState state, bool is_force_refresh) {
    UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
    if (ch->state != state || is_force_refresh)
    {
        if (state != UI_CHANNEL_STATE_DROPPED)
        {
            ch->prev_state = ch->state;
            ch->state = state;
        }
        
        if (state == UI_CHANNEL_STATE_DISABLED) {
            if (ch->prev_state == UI_CHANNEL_STATE_ADDED)
            {
                lv_obj_t *timer_label = lv_obj_get_child(lv_obj_get_child(channel, 1), 0);
                lv_obj_t *current_adjust_container = lv_obj_get_child(channel, 2);
                lv_obj_t *add_btn = lv_obj_get_child(current_adjust_container, 0);
                lv_obj_t *sub_btn = lv_obj_get_child(current_adjust_container, 1);
                lv_obj_clear_flag(timer_label, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(add_btn, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_clear_flag(sub_btn, LV_OBJ_FLAG_CLICKABLE);
            }
            else if (ch->prev_state == UI_CHANNEL_STATE_NOT_ADDED)
            {
                lv_obj_t *name_label = lv_obj_get_child(channel, 0);
                lv_obj_set_style_text_color(name_label, lv_color_hex(0xCDCDCD), 0);

                lv_obj_t *add_scheme_btn = lv_obj_get_child(lv_obj_get_child(channel, 1), 0);
                lv_obj_clear_flag(add_scheme_btn, LV_OBJ_FLAG_CLICKABLE);

                lv_obj_t *add_scheme_btn_label = lv_obj_get_child(add_scheme_btn, 0);
                lv_obj_set_style_text_color(add_scheme_btn_label, lv_color_hex(0xCDCDCD), 0);

            }

        }
        else if (state == UI_CHANNEL_STATE_NOT_ADDED) {
            lv_obj_clean(channel);
            lv_obj_set_style_bg_color(channel, lv_color_hex(0xf5f8ff), 0);

            //----- channel name -----
            lv_obj_t *label = lv_label_create(channel);
            lv_label_set_text(label, ch->name);
            lv_obj_set_style_text_font(label, &AliPuHui_20, 0);
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);

            //----- add scheme button container -----
            lv_obj_t *add_scheme_container = lv_obj_create(channel);
            lv_obj_set_size(add_scheme_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_pad_all(add_scheme_container, 0, 0);
            lv_obj_set_style_bg_opa(add_scheme_container, LV_OPA_TRANSP, 0);
            lv_obj_align(add_scheme_container, LV_ALIGN_CENTER, 0, 10);

            //----- add scheme button -----
            lv_obj_t *add_scheme_btn = lv_btn_create(add_scheme_container);
            lv_obj_remove_style_all(add_scheme_btn);
            lv_obj_set_style_bg_opa(add_scheme_btn, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(add_scheme_btn, LV_OPA_COVER, 0);
            lv_obj_set_size(add_scheme_btn, 170, 60);
            lv_obj_align(add_scheme_btn, LV_ALIGN_CENTER, 0, 0);
            lv_obj_add_event_cb(add_scheme_btn, AddSchemeBtnCallback, LV_EVENT_CLICKED, NULL);

            //----- scheme button label -----
            lv_obj_t *add_scheme_label = lv_label_create(add_scheme_btn);
            lv_label_set_text(add_scheme_label, "+ 添加方案");
            lv_obj_set_style_text_font(add_scheme_label, &AliPuHui_20, 0);
            lv_obj_set_style_text_color(add_scheme_label, lv_color_hex(0x1e827e), 0);
            lv_obj_align(add_scheme_label, LV_ALIGN_CENTER, 0, 0);

        }
        else if (state == UI_CHANNEL_STATE_ADDED) {
            if (ch->prev_state == UI_CHANNEL_STATE_DISABLED)
            {
                lv_obj_t *current_adjust_container = lv_obj_get_child(channel, 2);
                lv_obj_t *timer_label = lv_obj_get_child(lv_obj_get_child(channel, 1), 0);
                lv_obj_t *add_btn = lv_obj_get_child(current_adjust_container, 0);
                lv_obj_t *sub_btn = lv_obj_get_child(current_adjust_container, 1);
                lv_obj_add_flag(timer_label, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(add_btn, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_flag(sub_btn, LV_OBJ_FLAG_CLICKABLE);
                return;
            }
            lv_obj_clean(channel);
            lv_obj_set_style_bg_color(channel, lv_color_hex(0xffffff), 0);

            //----- channel name -----
            lv_obj_t *label = lv_label_create(channel);
            lv_label_set_text(label, ch->name);
            lv_obj_set_style_text_font(label, &AliPuHui_20, 0);
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);
            lv_obj_add_event_cb(label, ChannelLabelClickCallback, LV_EVENT_CLICKED, channel);
            lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);

            //----- channel timer container -----
            lv_obj_t *timer_container = lv_obj_create(channel);
            lv_obj_set_style_bg_color(timer_container, lv_color_hex(0xeff4fe), 0);
            lv_obj_set_style_border_opa(timer_container, LV_OPA_TRANSP, 0);
            lv_obj_set_size(timer_container, 100, 50);
            lv_obj_align_to(timer_container, label, LV_ALIGN_OUT_BOTTOM_MID, 20, 10);
            lv_obj_set_style_radius(timer_container, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_scrollbar_mode(timer_container, LV_SCROLLBAR_MODE_OFF);
            lv_obj_clear_flag(timer_container, LV_OBJ_FLAG_SCROLLABLE);

            //----- channel timer label -----
            lv_obj_t *timer_label = lv_label_create(timer_container);
            lv_label_set_text_fmt(timer_label, "%02d:%02d", ch->timer.remaining_seconds / 60, ch->timer.remaining_seconds % 60);
            lv_obj_set_size(timer_label, 100, 50);
            lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_30, 0);
            lv_obj_align(timer_label, LV_ALIGN_CENTER, 10, 8);
            lv_obj_add_event_cb(timer_label, TimerLabelClickCallback, LV_EVENT_CLICKED, channel);
            lv_obj_add_flag(timer_label, LV_OBJ_FLAG_CLICKABLE);

            //----- channel current adjustment container -----
            static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
            static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
            lv_obj_t *adjust_container = lv_obj_create(channel);
            lv_obj_set_size(adjust_container, 280, LV_PCT(100));
            lv_obj_align(adjust_container, LV_ALIGN_RIGHT_MID, 0, 0);
            lv_obj_set_scrollbar_mode(adjust_container, LV_SCROLLBAR_MODE_OFF);
            lv_obj_clear_flag(adjust_container, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_opa(adjust_container, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(adjust_container, LV_OPA_TRANSP, 0);

            //----- channel current add button -----
            lv_obj_t *add_btn = lv_imgbtn_create(adjust_container);
            lv_imgbtn_set_src(add_btn, LV_IMGBTN_STATE_RELEASED, NULL, &AddButton_White_120_fit, NULL);
            lv_obj_set_size(add_btn, AddButton_White_120_fit.header.w, AddButton_White_120_fit.header.h);
            lv_obj_align(add_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
            lv_obj_add_event_cb(add_btn, AddCurrentBtnCallback, LV_EVENT_CLICKED, NULL);
            addCallbackforImgBtn(add_btn);


            //----- channel current subtract button -----
            lv_obj_t *sub_btn = lv_imgbtn_create(adjust_container);
            lv_imgbtn_set_src(sub_btn, LV_IMGBTN_STATE_RELEASED, NULL, &SubtractButton_White_120_fit, NULL);
            lv_obj_set_size(sub_btn, SubtractButton_White_120_fit.header.w, SubtractButton_White_120_fit.header.h);
            lv_obj_align(sub_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
            lv_obj_add_event_cb(sub_btn, SubCurrentBtnCallback, LV_EVENT_CLICKED, NULL);
            addCallbackforImgBtn(sub_btn);
            //    lv_obj_set_grid_cell(sub_btn, LV_GRID_ALIGN_SPACE_BETWEEN, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

            //----- channel current arc -----
            lv_obj_t *current_arc = lv_arc_create(adjust_container);
            lv_arc_set_bg_angles(current_arc, 0, 270);
            lv_arc_set_value(current_arc, ch->pPlan->current_mA);
            lv_arc_set_rotation(current_arc, 135);
            lv_obj_remove_style(current_arc, NULL, LV_PART_KNOB);
            lv_obj_clear_flag(current_arc, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_size(current_arc, 100, 100);
            lv_obj_align(current_arc, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_style_arc_width(current_arc, UI_CURRENT_ARC_LINE_WIDTH, LV_PART_MAIN);
            lv_obj_set_style_arc_width(current_arc, UI_CURRENT_ARC_LINE_WIDTH, LV_PART_INDICATOR);

            //----- channel current value label -----
            lv_obj_t *current_label = lv_label_create(current_arc);
            lv_label_set_text_fmt(current_label, "%d", ch->pPlan->current_mA);
            lv_obj_set_style_text_font(current_label, &AliPuHui_40, 0);
            lv_obj_align(current_label, LV_ALIGN_CENTER, 0, 0);

            //----- channel current unit label -----
            lv_obj_t *ma_label = lv_label_create(current_arc);
            lv_label_set_text(ma_label, "mA");
            lv_obj_set_style_text_font(ma_label, &AliPuHui_20, 0);
            lv_obj_align_to(ma_label, current_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        }
        else if (state == UI_CHANNEL_STATE_DROPPED) {
            lv_obj_t *modal_bg = lv_obj_create(channel);
    //        lv_obj_set_style_pad_all(channel, 0, 0);
            lv_obj_set_size(modal_bg, lv_obj_get_width(channel), lv_obj_get_height(channel));
            lv_obj_center(modal_bg);
            lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
            lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
            lv_obj_set_style_border_width(modal_bg, 0, 0);
            lv_obj_set_style_pad_all(modal_bg, 0, 0);
            lv_obj_set_style_radius(modal_bg, 8, 0);

            lv_obj_t *warning_icon = lv_img_create(modal_bg);
            lv_img_set_src(warning_icon, &WarningIcon_fit);
            lv_obj_align(warning_icon, LV_ALIGN_CENTER, -70, 0);

            lv_obj_t *label = lv_label_create(modal_bg);
            lv_label_set_text(label, "电极片脱落");
            lv_obj_align(label, LV_ALIGN_CENTER, 20, 0);
            lv_obj_set_style_text_font(label, &AliPuHui_24, 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);

            lv_obj_t *obj = lv_obj_create(modal_bg);
            lv_obj_set_size(obj,LV_PCT(100),LV_PCT(100));
            lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
            lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
            lv_obj_add_event_cb(obj, DropModalDelCallback, LV_EVENT_CLICKED, channel);
        }
    }
}

uint8_t get_channel_needing_modifying()
{
    uint8_t indicator = 0;
    to_modify_plan_cnt = 0;
    to_modify_plan_ids[0] = 255;
    to_modify_plan_ids[1] = 255;
    to_modify_plan_ids[2] = 255;
    to_modify_plan_ids[3] = 255;
    for (uint8_t i = 0; i < 4; i++)
    {
        bool need_to_modify_pPlan = true;
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
        if (ch->state != UI_CHANNEL_STATE_ADDED)
            continue;
        for (uint8_t j = 0; j < to_modify_plan_cnt; j++)
        {
            if (ch->pPlan->id == to_modify_plan_ids[j])
            {
                need_to_modify_pPlan = false;
                break;
            }
        }
        if (need_to_modify_pPlan)
        {
            indicator |= (1 << i);
            to_modify_plan_ids[to_modify_plan_cnt] = ch->pPlan->id;
            to_modify_plan_cnt++;
        }
    }
    return indicator;
}

void clear_all_channels()
{
    if (ChannelA.timer.state == UI_TIMER_STATE_START)
    {
        ens_stop_channel_plan(ChannelA.pPlan, ChannelA.index);
        lv_timer_del(ChannelA.timer.lvtimer);
        // gptimer_stop(ChannelA.timer.timer_handle);
        // gptimer_disable(ChannelA.timer.timer_handle);
        // gptimer_del_timer(ChannelA.timer.timer_handle);
    }
    
    if (ChannelB.timer.state == UI_TIMER_STATE_START)
    {
        ens_stop_channel_plan(ChannelB.pPlan, ChannelB.index);
        lv_timer_del(ChannelB.timer.lvtimer);
        // gptimer_stop(ChannelB.timer.timer_handle);
        // gptimer_disable(ChannelB.timer.timer_handle);
        // gptimer_del_timer(ChannelB.timer.timer_handle);
    }
    
    if (ChannelC.timer.state == UI_TIMER_STATE_START)
    {
        ens_stop_channel_plan(ChannelC.pPlan, ChannelC.index);
        lv_timer_del(ChannelC.timer.lvtimer);
        // gptimer_stop(ChannelC.timer.timer_handle);
        // gptimer_disable(ChannelC.timer.timer_handle);
        // gptimer_del_timer(ChannelC.timer.timer_handle);
    }
    
    if (ChannelD.timer.state == UI_TIMER_STATE_START)
    {
        ens_stop_channel_plan(ChannelD.pPlan, ChannelD.index);
        lv_timer_del(ChannelD.timer.lvtimer);
        // gptimer_stop(ChannelD.timer.timer_handle);
        // gptimer_disable(ChannelD.timer.timer_handle);
        // gptimer_del_timer(ChannelD.timer.timer_handle);
    }
    
    uint8_t indicator = get_channel_needing_modifying();
    Channel_Struct_Clear(&ChannelA, indicator & 0x01);
    Channel_Struct_Clear(&ChannelB, indicator & 0x02);
    Channel_Struct_Clear(&ChannelC, indicator & 0x04);
    Channel_Struct_Clear(&ChannelD, indicator & 0x08);

    set_channel_state(lv_channel_a, UI_CHANNEL_STATE_NOT_ADDED, false);
    set_channel_state(lv_channel_b, UI_CHANNEL_STATE_NOT_ADDED, false);
    set_channel_state(lv_channel_c, UI_CHANNEL_STATE_NOT_ADDED, false);
    set_channel_state(lv_channel_d, UI_CHANNEL_STATE_NOT_ADDED, false);
}

lv_obj_t *get_channel_by_index(uint8_t index)
{
    lv_obj_t *container = lv_obj_get_child(main_scr, 5);
    return lv_obj_get_child(container, index);
}

lv_obj_t *create_channel(lv_obj_t *parent, UI_Channel *ch) {
    //----- channel container -----
    lv_obj_t *channel = lv_obj_create(parent);
    lv_obj_set_size(channel, 420, 50);
    lv_obj_clear_flag(channel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_opa(channel, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_grow(channel, 1);

    lv_obj_set_user_data(channel, ch);
    set_channel_state(channel, UI_CHANNEL_STATE_NOT_ADDED, false);
    return channel;
}

void set_battery_level()
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    lv_obj_t *top_bar = lv_obj_get_child(scr, 0);
    lv_obj_t *empty_battery = NULL;
    if (scr == charging_scr)
    {
        lv_obj_t *charging_ring = lv_obj_get_child(scr, 1);
        lv_obj_t *soc_label = lv_obj_get_child(charging_ring, 0);
        lv_label_set_text_fmt(soc_label, "%d", g_battery_soc);
        empty_battery = lv_obj_get_child(top_bar, 0);
        lv_obj_t *percent_label = lv_obj_get_child(charging_ring, 1);
        lv_obj_align_to(percent_label, soc_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -10);
    }
    else
        empty_battery = lv_obj_get_child(top_bar, 1);
    // else
    // {
    lv_obj_t *battery_level1 = lv_obj_get_child(empty_battery, 0);
    lv_obj_t *battery_level2 = lv_obj_get_child(empty_battery, 1);
    lv_obj_t *battery_level3 = lv_obj_get_child(empty_battery, 2);
    lv_obj_t *battery_level4 = lv_obj_get_child(empty_battery, 3);

    if (75 < g_battery_soc && g_battery_soc <= 100) {
        lv_obj_clear_flag(battery_level1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level4, LV_OBJ_FLAG_HIDDEN);
    }
    else if (50 < g_battery_soc && g_battery_soc <= 75) {
        lv_obj_clear_flag(battery_level1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level4, LV_OBJ_FLAG_HIDDEN);
    }
    else if (25 < g_battery_soc && g_battery_soc <= 50) {
        lv_obj_clear_flag(battery_level1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(battery_level2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level4, LV_OBJ_FLAG_HIDDEN);
    }
    else if (5 < g_battery_soc && g_battery_soc <= 25) {
        lv_obj_clear_flag(battery_level1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level4, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(battery_level1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_level4, LV_OBJ_FLAG_HIDDEN);
    }
    // }
    
}

void set_bluetooth_status(bool is_connected)
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    lv_obj_t *top_bar = lv_obj_get_child(scr, 0);
    lv_obj_t *bluetooth_icon;
    if (scr == charging_scr)
        bluetooth_icon = lv_obj_get_child(top_bar, 1);
    else
        bluetooth_icon = lv_obj_get_child(top_bar, 2);
    if (is_connected)
        lv_img_set_src(bluetooth_icon, &BluetoothIcon_fit);
    else
        lv_img_set_src(bluetooth_icon, &Bluetooth_Disconnect_fit);
}

lv_obj_t *create_top_bar(lv_obj_t *scr, bool is_main_scr, bool is_charging_scr)
{

    lv_obj_t *top_bar = lv_obj_create(scr);
    lv_obj_set_size(top_bar, lv_obj_get_width(scr), 80);
    lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(top_bar, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);
    if (is_main_scr)
    {
        lv_obj_t *child_lock_btn = lv_imgbtn_create(top_bar);
        lv_imgbtn_set_src(child_lock_btn, LV_IMGBTN_STATE_RELEASED, NULL, &LockIconTransparent_fit, NULL);
        lv_obj_set_size(child_lock_btn, LockIconTransparent_fit.header.w, LockIconTransparent_fit.header.h);
        lv_obj_align(child_lock_btn, LV_ALIGN_LEFT_MID, 0, 0);
        addCallbackforImgBtn(child_lock_btn);
        lv_obj_add_event_cb(child_lock_btn, ChildLockBtnCallback, LV_EVENT_CLICKED, NULL);
        static bool is_locked = false;
        lv_obj_set_user_data(child_lock_btn, &is_locked);
    }
    else if (!is_charging_scr)
    {
        lv_obj_t *back_btn = lv_imgbtn_create(top_bar);
        lv_imgbtn_set_src(back_btn, LV_IMGBTN_STATE_RELEASED, NULL, &BackButton_fit, NULL);
        lv_obj_set_size(back_btn, BackButton_fit.header.w, BackButton_fit.header.h);
        lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 0, 0);
        addCallbackforImgBtn(back_btn);
        lv_obj_add_event_cb(back_btn, BackBtnCallback, LV_EVENT_CLICKED, NULL);
    }
    lv_obj_t *battery_icon = lv_img_create(top_bar);
    lv_img_set_src(battery_icon, &EmptyBattery_fit);
    lv_obj_align(battery_icon, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *battery_level_1 = lv_img_create(battery_icon);
    lv_img_set_src(battery_level_1, &BatteryBar_fit);
    lv_obj_align(battery_level_1, LV_ALIGN_LEFT_MID, 5, 0);

    lv_obj_t *battery_level_2 = lv_img_create(battery_icon);
    lv_img_set_src(battery_level_2, &BatteryBar_fit);
    lv_obj_align_to(battery_level_2, battery_level_1, LV_ALIGN_OUT_RIGHT_MID, 3, 0);

    lv_obj_t *battery_level_3 = lv_img_create(battery_icon);
    lv_img_set_src(battery_level_3, &BatteryBar_fit);
    lv_obj_align_to(battery_level_3, battery_level_2, LV_ALIGN_OUT_RIGHT_MID, 3, 0);

    lv_obj_t *battery_level_4 = lv_img_create(battery_icon);
    lv_img_set_src(battery_level_4, &BatteryBar_fit);
    lv_obj_align_to(battery_level_4, battery_level_3, LV_ALIGN_OUT_RIGHT_MID, 3, 0);

    lv_obj_t *bluetooth_icon = lv_img_create(top_bar);
    lv_img_set_src(bluetooth_icon, &Bluetooth_Disconnect_fit);
    lv_obj_set_size(bluetooth_icon, Bluetooth_Disconnect_fit.header.w, Bluetooth_Disconnect_fit.header.h);
    lv_obj_align_to(bluetooth_icon, battery_icon, LV_ALIGN_OUT_LEFT_MID, -15, 0);
    // addCallbackforImgBtn(bluetooth_btn);

    if (!is_charging_scr)
    {
        lv_obj_t *calib_btn = lv_imgbtn_create(top_bar);
        lv_imgbtn_set_src(calib_btn, LV_IMGBTN_STATE_RELEASED, NULL, &CalibIcon_fit, NULL);
        lv_obj_set_size(calib_btn, CalibIcon_fit.header.w, CalibIcon_fit.header.h);
        lv_obj_align_to(calib_btn, bluetooth_icon, LV_ALIGN_OUT_LEFT_MID, -15, 0);
        lv_obj_add_event_cb(calib_btn, CalibBtnCallback, LV_EVENT_CLICKED, NULL);
        addCallbackforImgBtn(calib_btn);

        lv_obj_t *setting_btn = lv_imgbtn_create(top_bar);
        lv_imgbtn_set_src(setting_btn, LV_IMGBTN_STATE_RELEASED, NULL, &SettingIcon_fit, NULL);
        lv_obj_set_size(setting_btn, SettingIcon_fit.header.w, SettingIcon_fit.header.h);
        lv_obj_align_to(setting_btn, calib_btn, LV_ALIGN_OUT_LEFT_MID, -15, 0);
        lv_obj_add_event_cb(setting_btn, BluetoothBtnCallback, LV_EVENT_CLICKED, NULL);
        addCallbackforImgBtn(setting_btn);
        
    }
    

    return top_bar;
}

void update_start_btn_status()
{
    bool disable_start_btn = true;
    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
        if (ch->state == UI_CHANNEL_STATE_ADDED && ch->pPlan->current_mA > 0)
            disable_start_btn = false;
    }
    if (disable_start_btn)
    {
        lv_obj_t *start_btn = lv_obj_get_child(main_scr, 3);
        lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Gray_fit, NULL);
        lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *start_label = lv_obj_get_child(main_scr, 4);
        lv_obj_set_style_text_color(start_label, lv_color_hex(0xdfe1ea), 0);
    }
    else
    {
        lv_obj_t *start_btn = lv_obj_get_child(main_scr, 3);
        bool *is_stimulation_running = (bool*)lv_obj_get_user_data(start_btn);
        if (*is_stimulation_running)
            lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &PauseButton_fit, NULL);
        else
            lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Green_fit, NULL);
        lv_obj_add_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *start_label = lv_obj_get_child(main_scr, 4);
        lv_obj_set_style_text_color(start_label, lv_color_black(), 0);
    }
    
    
}

lv_obj_t *create_main_scr() {
    lv_obj_t *scr = lv_img_create(NULL);
    lv_img_set_src(scr, &BG2_fit);
    lv_obj_align(scr, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *top_bar = create_top_bar(scr, true, false);

    lv_obj_t *clear_btn = lv_imgbtn_create(scr);
    lv_imgbtn_set_src(clear_btn, LV_IMGBTN_STATE_RELEASED, NULL, &Clear_fit, NULL);
    lv_obj_set_size(clear_btn, Clear_fit.header.w, Clear_fit.header.h);
    lv_obj_align(clear_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(clear_btn, ClearBtnCallback, LV_EVENT_CLICKED, NULL);
    addCallbackforImgBtn(clear_btn);

    lv_obj_t *sync_adjust_btn = lv_imgbtn_create(scr);
    lv_imgbtn_set_src(sync_adjust_btn, LV_IMGBTN_STATE_RELEASED, NULL, &SyncAdjust_fit, NULL);
    lv_obj_set_size(sync_adjust_btn, SyncAdjust_fit.header.w, SyncAdjust_fit.header.h);
    lv_obj_align(sync_adjust_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(sync_adjust_btn, SyncAdjustBtnCallback, LV_EVENT_CLICKED, NULL);
    addCallbackforImgBtn(sync_adjust_btn);

    lv_obj_t *stimulation_start_btn = lv_imgbtn_create(scr);
    lv_imgbtn_set_src(stimulation_start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Green_fit, NULL);
    lv_imgbtn_set_src(stimulation_start_btn, LV_IMGBTN_STATE_DISABLED, NULL, &StartButton_Gray_fit, NULL);
    lv_obj_align(stimulation_start_btn, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_size(stimulation_start_btn, StartButton_Green_fit.header.w, StartButton_Green_fit.header.h);
    addCallbackforImgBtn(stimulation_start_btn);
    static bool is_stimulation_running = false;
    lv_obj_set_user_data(stimulation_start_btn, &is_stimulation_running);
    lv_obj_add_event_cb(stimulation_start_btn, StimulationStartBtnCallback, LV_EVENT_CLICKED, NULL);

    lv_obj_t *stimulation_start_label = lv_label_create(scr);
    lv_label_set_text(stimulation_start_label, "全部开始");
    lv_obj_set_style_text_font(stimulation_start_label, &AliPuHui_20, 0);
    lv_obj_align(stimulation_start_label, LV_ALIGN_BOTTOM_MID, 0, -25);

    lv_obj_t *container = lv_obj_create(scr);
    lv_obj_set_size(container, LV_SIZE_CONTENT, 660);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_bg_color(container, lv_color_hex(0xFF0000), 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN_WRAP);

    Channel_Struct_Init(&ChannelA, "通道A", 0);
    Channel_Struct_Init(&ChannelB, "通道B", 1);
    Channel_Struct_Init(&ChannelC, "通道C", 2);
    Channel_Struct_Init(&ChannelD, "通道D", 3);

    lv_channel_a = create_channel(container, &ChannelA);
    lv_channel_b = create_channel(container, &ChannelB);
    lv_channel_c = create_channel(container, &ChannelC);
    lv_channel_d = create_channel(container, &ChannelD);

    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(container, LV_OPA_TRANSP, 0);
    return scr;
}

lv_obj_t *create_plan_option(lv_obj_t *parent, Plan *plan) {
    //----- Scheme Dropdown -----
    lv_obj_t *plan_option = lv_btn_create(parent);
    lv_obj_set_size(plan_option, LV_PCT(100), 80);
    lv_obj_set_style_bg_color(plan_option, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(plan_option, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_flex_flow(plan_option, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(plan_option, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(plan_option, PlanOptionCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_set_user_data(plan_option, plan);

    //----- Scheme Tag Bullet Point -----
    lv_obj_t *title_bullet_point = lv_label_create(plan_option);
    lv_label_set_text(title_bullet_point, "•");
    lv_obj_set_style_text_font(title_bullet_point, &AliPuHui_40, 0);
    lv_obj_set_style_text_color(title_bullet_point, lv_color_hex(0x8c9bbc), 0);

    //----- Scheme Tag -----
    lv_obj_t *scheme_tag = lv_label_create(plan_option);
    lv_label_set_text(scheme_tag, plan->name);
    lv_obj_set_style_text_font(scheme_tag, &AliPuHui_24, 0);
    lv_obj_set_style_text_color(scheme_tag, lv_color_hex(0x303030), 0);

    return plan_option;
}

void update_scheme_pages()
{
    valid_page_num = 1;
    uint8_t current_page_plan_index = 0;

    for (uint8_t i = 0; i < MAX_PLAN_NUM; i++)
    {
        if(saved_plans[i].id > 0 && saved_plans[i].id <= MAX_PLAN_NUM)
        {
            schemePages[valid_page_num - 1][current_page_plan_index++] = &saved_plans[i];
            if (current_page_plan_index == UI_PLAN_NUM_PER_PAGE)
            {
                current_page_plan_index = 0;
                valid_page_num++;
            }
        }
    }
}

void init_scheme_pages()
{

    // schemePages[0] = page1;
    // schemePages[1] = page2;
    // schemePages[2] = page3;
    for (uint8_t i = 0; i < UI_PLAN_PAGE_NUM_MAX; i++)
    {
        schemePages[i] = (PlanPage)malloc(UI_PLAN_NUM_PER_PAGE * sizeof(pPlan));
        memset(schemePages[i], 0, UI_PLAN_NUM_PER_PAGE * sizeof(pPlan));
    }
    update_scheme_pages();

}

void set_scheme_set_page(uint8_t page)
{
    lv_obj_t *scheme_container = lv_obj_get_child(scheme_scr, 1);
    lv_obj_t *scheme_set_list_container = lv_obj_get_child(scheme_container, 1);
    page = page % valid_page_num;
    lv_obj_clean(scheme_set_list_container);
    PlanPage current_page = schemePages[page];
    for (uint8_t i = 0; i < UI_PLAN_NUM_PER_PAGE; ++i) {
        if (current_page[i])
            create_plan_option(scheme_set_list_container, current_page[i]);
    }

}

lv_obj_t *create_scheme_scr() {
    lv_obj_t *scr = lv_img_create(NULL);
    lv_img_set_src(scr, &BG3_fit);

    lv_obj_t *top_bar = create_top_bar(scr, false, false);

    //----- Scheme Container -----
    lv_obj_t *scheme_container = lv_obj_create(scr);
    lv_obj_set_size(scheme_container, 450, 760);
    lv_obj_align(scheme_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(scheme_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(scheme_container, LV_OPA_TRANSP, 0);


    //----- Scheme Title -----
    lv_obj_t *scheme_title = lv_label_create(scheme_container);
    lv_label_set_text(scheme_title, "方案选择");
    lv_obj_set_style_text_font(scheme_title, &AliPuHui_30, 0);
    lv_obj_align(scheme_title, LV_ALIGN_TOP_LEFT, 0, 0);

    //----- Scheme List Container -----
    lv_obj_t *scheme_set_list_container = lv_obj_create(scheme_container);
    lv_obj_set_size(scheme_set_list_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(scheme_set_list_container, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_align(scheme_set_list_container, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_opa(scheme_set_list_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(scheme_set_list_container, LV_OPA_TRANSP, 0);

    init_scheme_pages();


    //----- Page Indicator Container -----
    lv_obj_t *page_indicator_container = lv_obj_create(scheme_container);
    lv_obj_set_size(page_indicator_container, LV_PCT(100), 50);
    lv_obj_align(page_indicator_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(page_indicator_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(page_indicator_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(page_indicator_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(page_indicator_container, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(page_indicator_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(page_indicator_container, LV_OBJ_FLAG_SCROLLABLE);

    //----- Previous Page Button -----
    lv_obj_t *prev_page_btn = lv_imgbtn_create(page_indicator_container);
    lv_imgbtn_set_src(prev_page_btn, LV_IMGBTN_STATE_RELEASED, NULL, &PrevPageBtn_fit, NULL);
    lv_obj_set_size(prev_page_btn, PrevPageBtn_fit.header.w, PrevPageBtn_fit.header.h);
    addCallbackforImgBtn(prev_page_btn);
    lv_obj_add_event_cb(prev_page_btn, PrevPageBtnCallback, LV_EVENT_CLICKED, NULL);

    //----- Current Page Label -----
    lv_obj_t *current_page_label = lv_label_create(page_indicator_container);
    lv_label_set_text_fmt(current_page_label, "%d  /  %d", current_page_num + 1, valid_page_num);
    lv_obj_set_style_text_font(current_page_label, &AliPuHui_24, 0);

    //----- Next Page Button -----
    lv_obj_t *next_page_btn = lv_imgbtn_create(page_indicator_container);
    lv_imgbtn_set_src(next_page_btn, LV_IMGBTN_STATE_RELEASED, NULL, &NextPageBtn_fit, NULL);
    lv_obj_set_size(next_page_btn, NextPageBtn_fit.header.w, NextPageBtn_fit.header.h);
    addCallbackforImgBtn(next_page_btn);
    lv_obj_add_event_cb(next_page_btn, NextPageBtnCallback, LV_EVENT_CLICKED, NULL);

    return scr;
}



void set_calib_state(UI_CalibState state)
{
    
}

lv_obj_t *create_calib_scr()
{
    static UI_CalibState calib_state = UI_CALIB_STATE_STOP;
    calib_cnt = 0;
    lv_obj_t *scr = lv_img_create(NULL);
    lv_img_set_src(scr, &BG3_fit);
    lv_obj_set_user_data(scr, &calib_state);

    lv_obj_t *top_bar = create_top_bar(scr, false, false);

    lv_obj_t *posture_demo_img = lv_img_create(scr);
    lv_img_set_src(posture_demo_img, &PostureDemo_fit);
//    lv_obj_set_size(posture_demo_img, 450, 760);
    lv_obj_align(posture_demo_img, LV_ALIGN_CENTER, -15, -55);

    lv_obj_t *progress_bar = lv_bar_create(scr);
    lv_obj_set_size(progress_bar, 300, 20);
    lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 100);
    static lv_grad_dsc_t grad_dsc;
    grad_dsc.dir = LV_GRAD_DIR_HOR;
    grad_dsc.stops_count = 2;
    grad_dsc.stops[0].color = lv_color_hex(0xC7E1E1);
    grad_dsc.stops[1].color = lv_color_hex(0x319E94);
    grad_dsc.stops[0].frac = 0;
    grad_dsc.stops[1].frac = 255;
    lv_bar_set_range(progress_bar, 0, 100);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_grad(progress_bar, &grad_dsc, LV_PART_INDICATOR);
    lv_obj_add_event_cb(progress_bar, ProgressBarIndicatorCallback, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_user_data(progress_bar, &calib_cnt);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "第一次");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 160);
    lv_obj_set_style_text_font(label, &AliPuHui_30, 0);

    lv_obj_t *start_btn_container = lv_obj_create(scr);
    lv_obj_set_size(start_btn_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(start_btn_container, LV_ALIGN_BOTTOM_MID, 0, -150);
    lv_obj_set_style_bg_opa(start_btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(start_btn_container, LV_OPA_TRANSP, 0);

    lv_obj_t *calib_start_btn = lv_imgbtn_create(start_btn_container);
    lv_imgbtn_set_src(calib_start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartIconTransparent_fit, NULL);
    lv_obj_set_size(calib_start_btn, StartIconTransparent_fit.header.w, StartIconTransparent_fit.header.h);
    addCallbackforImgBtn(calib_start_btn);
    lv_obj_align(calib_start_btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(calib_start_btn, CalibStartBtnCallback, LV_EVENT_CLICKED, scr);

    lv_obj_t *calib_start_label = lv_label_create(start_btn_container);
    lv_label_set_text(calib_start_label, "开始校准");
    lv_obj_set_style_text_font(calib_start_label, &AliPuHui_30, 0);
    lv_obj_align_to(calib_start_label, calib_start_btn, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    return scr;
}

lv_obj_t *create_charging_scr()
{
    lv_obj_t *scr = lv_img_create(NULL);
    lv_img_set_src(scr, &BG1_fit);

    lv_obj_t *top_bar = create_top_bar(scr, false, true);

    lv_obj_t *charging_ring = lv_img_create(scr);
    lv_img_set_src(charging_ring, &ChargeRing_fit);
    lv_obj_align(charging_ring, LV_ALIGN_CENTER, 0, -90);

    lv_obj_t *soc_label = lv_label_create(charging_ring);
    lv_label_set_text_fmt(soc_label, "%d", g_battery_soc);
    lv_obj_set_style_text_font(soc_label, &AliPuHui_70, 0);
    lv_obj_align(soc_label, LV_ALIGN_CENTER, -3, 5);

    lv_obj_t *percent_label = lv_label_create(charging_ring);
    lv_label_set_text(percent_label, "%");
    lv_obj_set_style_text_font(percent_label, &lv_font_montserrat_30, 0);
    lv_obj_align_to(percent_label, soc_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -10);

    lv_obj_t *lighting_icon = lv_img_create(scr);
    lv_img_set_src(lighting_icon, &LightningSymbol_fit);
    lv_obj_align(lighting_icon, LV_ALIGN_CENTER, 0, 178);

    lv_obj_t *charging_label = lv_label_create(scr);
    lv_label_set_text(charging_label, "正在充电");
    lv_obj_set_style_text_font(charging_label, &AliPuHui_30, 0);
    lv_obj_set_style_text_color(charging_label, lv_color_hex(0x9AC7C0), 0);
    lv_obj_align_to(charging_label, lighting_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    return scr;
}

void load_charging_scr()
{
    lv_obj_set_user_data(charging_scr, lv_scr_act());
    lv_scr_load(charging_scr);
}

void exit_charging_scr()
{
    lv_obj_t *act_scr = lv_obj_get_user_data(charging_scr);
    lv_scr_load(act_scr);
}

void CustomUI() {
    main_scr = create_main_scr();
    scheme_scr = create_scheme_scr();
    set_scheme_set_page(current_page_num);

    calib_scr = create_calib_scr();
    charging_scr = create_charging_scr();
    lv_obj_set_user_data(charging_scr, main_scr);
//    lv_scr_load(charging_scr);
    lv_scr_load(main_scr);
    update_start_btn_status();
}