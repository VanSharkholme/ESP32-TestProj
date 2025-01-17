//
// Created by 13651 on 2024/11/19.
//

#include "Callbacks.h"

void ImgBtnPressedCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_set_style_img_recolor(btn, lv_color_black(), 0);
    lv_obj_set_style_img_recolor_opa(btn, LV_OPA_30, 0);
}

void ImgBtnReleasedCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_set_style_img_recolor_opa(btn, LV_OPA_TRANSP, 0);
}

void AddSchemeBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *btn_container = lv_obj_get_parent(btn);
    lv_obj_t *channel = lv_obj_get_parent(btn_container);
//    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
    lv_obj_set_user_data(scheme_scr, channel);
    update_scheme_pages();
    lv_obj_t *scheme_container = lv_obj_get_child(scheme_scr, 1);
    lv_obj_t *page_indicator_container = lv_obj_get_child(scheme_container, 2);
    lv_obj_t *page_num_label = lv_obj_get_child(page_indicator_container, 1);
    current_page_num = 0;
    set_scheme_set_page(0);
    lv_label_set_text_fmt(page_num_label, "%d  /  %d", current_page_num + 1, valid_page_num);

    
    lv_scr_load(scheme_scr);
}

void BackBtnCallback(lv_event_t *event)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_t *back_scr = main_scr;
    if (scr == calib_scr)
    {
        back_scr = lv_obj_get_user_data(scr);
        lv_obj_del_async(scr);
    }
    
    lv_scr_load(back_scr);
}

void PlanOptionCallback(lv_event_t *event)
{
    lv_obj_t *plan_option = lv_event_get_target(event);
    Plan *plan = (Plan *) lv_obj_get_user_data(plan_option);
    lv_obj_t *channel = (lv_obj_t *) lv_obj_get_user_data(scheme_scr);
    UI_Channel *ch = (UI_Channel *)lv_obj_get_user_data(channel);
    saved_plans[plan->id - 1].used_times++;
    if (plan->channel_num == 1)
    {
        set_channel_plan(ch->index, plan, false);
    }
    else if (plan->channel_num >= 2 && plan->channel_num <= 4)
    {
        uint8_t spare_channel_indicator = 0;
        uint8_t spare_channel_num = 0;
        uint8_t unassigned_channel_num = plan->channel_num;
        uint8_t assigning_channel_index = ch->index;
        spare_channel_num = find_spare_channel(&spare_channel_indicator);
        if (spare_channel_num < plan->channel_num)
        {
            create_warning_modal("所选方案通道数不足\n请重新选择");
            return;
        }
        // Plan *plan_to_assign = (Plan *)malloc(sizeof(Plan));
        // memcpy(plan_to_assign, plan, sizeof(Plan));
        // plan_to_assign->id = ch->index + 1;
        for (uint8_t i = 0; i < 4; ++i)
        {
            uint8_t next_channel = assigning_channel_index + i;
            if ((next_channel == next_channel % 4) && (spare_channel_indicator & (1 << next_channel)))
            {
                lv_obj_t *assigning_channel = get_channel_by_index(next_channel);
                UI_Channel *assigning_ch = (UI_Channel *)lv_obj_get_user_data(assigning_channel);
                assigning_ch->pPlan = (Plan *)malloc(sizeof(Plan));
                memcpy(assigning_ch->pPlan, plan, sizeof(Plan));
                assigning_ch->pPlan->id = next_channel + 1;
                assigning_ch->timer.remaining_seconds = plan->total_time_min * 60;
                set_channel_state(assigning_channel, UI_CHANNEL_STATE_ADDED, false);
                lv_obj_set_style_bg_color(assigning_channel, lv_color_hex(0xdff7f5), 0);
                unassigned_channel_num--;
                if (unassigned_channel_num <= 0)
                    break;
            }
            if (i == 0)
                continue;
            next_channel = assigning_channel_index - i;
            if ((next_channel == next_channel % 4) && (spare_channel_indicator & (1 << next_channel)))
            {
                lv_obj_t *assigning_channel = get_channel_by_index(next_channel);
                UI_Channel *assigning_ch = (UI_Channel *)lv_obj_get_user_data(assigning_channel);
                assigning_ch->pPlan = (Plan *)malloc(sizeof(Plan));
                memcpy(assigning_ch->pPlan, plan, sizeof(Plan));
                assigning_ch->pPlan->id = next_channel + 1;
                assigning_ch->timer.remaining_seconds = plan->total_time_min * 60;
                set_channel_state(assigning_channel, UI_CHANNEL_STATE_ADDED, false);
                unassigned_channel_num--;
                if (unassigned_channel_num <= 0)
                    break;
            }
            
        }
        
        
    }
    update_plan_position_by_used_times(plan);
    lv_scr_load(main_scr);
    update_start_btn_status();
}

void ClearBtnCallback(lv_event_t *event)
{
    for (int i = 0; i < 4; ++i) {
        lv_obj_t *channel = get_channel_by_index(i);
        UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
        if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state == UI_TIMER_STATE_START)
            return;
            // set_channel_timer_state(channel, UI_TIMER_STATE_STOP);
    }
    clear_all_channels();
    update_start_btn_status();
}

void PrevPageBtnCallback(lv_event_t *event)
{
    if (valid_page_num == 1)
    {
        return;
    }
    if (current_page_num == 0)
    {
        return;
    }
    current_page_num = current_page_num - 1;
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *container = lv_obj_get_parent(btn);
    lv_obj_t *page_num_label = lv_obj_get_child(container, 1);
    lv_obj_t *scheme_set_list_container = lv_obj_get_user_data(btn);
    set_scheme_set_page(current_page_num);
    lv_label_set_text_fmt(page_num_label, "%d  /  %d", current_page_num + 1, valid_page_num);
}

void NextPageBtnCallback(lv_event_t *event)
{
    if (valid_page_num == 1)
    {
        return;
    }
    if (current_page_num + 1 >= valid_page_num)
    {
        return;
    }
    
    current_page_num = current_page_num + 1;
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *container = lv_obj_get_parent(btn);
    lv_obj_t *page_num_label = lv_obj_get_child(container, 1);
    lv_obj_t *scheme_set_list_container = lv_obj_get_user_data(btn);
    set_scheme_set_page(current_page_num);
    lv_label_set_text_fmt(page_num_label, "%d  /  %d", current_page_num + 1, valid_page_num);
}

void AddCurrentBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *current_container = lv_obj_get_parent(btn);
    lv_obj_t *channel = lv_obj_get_parent(current_container);
    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
    refresh_channel_current(channel, 1, false);
    // if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state == UI_TIMER_STATE_START)
    lv_timer_pause(ch->tmp_timer);
    lv_timer_set_repeat_count(ch->tmp_timer, 10);
    lv_timer_reset(ch->tmp_timer);
    ens_send_channel_plan_data(ch->pPlan, ch->index);
    ens_send_start_cmd(1 << ch->index);
    if(ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START)
    {
        lv_timer_resume(ch->tmp_timer);
    }
    update_start_btn_status();
}

void SubCurrentBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *current_container = lv_obj_get_parent(btn);
    lv_obj_t *channel = lv_obj_get_parent(current_container);
    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
    refresh_channel_current(channel, -1, false);
    ens_send_channel_plan_data(ch->pPlan, ch->index);
    lv_timer_pause(ch->tmp_timer);
    lv_timer_set_repeat_count(ch->tmp_timer, 10);
    lv_timer_reset(ch->tmp_timer);
    ens_send_start_cmd(1 << ch->index);
    if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START)
    {
        lv_timer_resume(ch->tmp_timer);
    }
    update_start_btn_status();
}

void SyncConfirmBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(btn);
    lv_obj_t *sync_adjust_container = lv_obj_get_child(modal_bg, 0);
    lv_obj_t *sync_adjust_bg = lv_obj_get_child(sync_adjust_container, 0);
    lv_obj_t *difference_label = lv_obj_get_child(sync_adjust_bg, 0);
    int8_t *difference = (int8_t *)lv_obj_get_user_data(difference_label);
    // uint8_t channel_indicator = 0;
    // channel_indicator = get_channel_needing_modifying();
    ens_stop_all_channel();
    for (int i = 0; i < 4; ++i) {
        // if ((channel_indicator & (1 << i)))
        // {
            lv_obj_t *channel = get_channel_by_index(i);
            UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
            refresh_channel_current(channel, *difference, false);
            ens_send_channel_plan_data(ch->pPlan, ch->index);
            lv_timer_pause(ch->tmp_timer);
            lv_timer_set_repeat_count(ch->tmp_timer, 10);
            lv_timer_reset(ch->tmp_timer);
            if(ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START)
            {
                lv_timer_resume(ch->tmp_timer);
            }
        // }
    }
    ens_send_start_cmd(0x0F);

    lv_obj_del_async(modal_bg);
    update_start_btn_status();

}

void CancelBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(btn);
    lv_obj_del_async(modal_bg);
}

void SyncAddBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *sync_adjust_container = lv_obj_get_parent(btn);
    lv_obj_t *sync_adjust_bg = lv_obj_get_child(sync_adjust_container, 0);
    lv_obj_t *difference_label = lv_obj_get_child(sync_adjust_bg, 0);
    int8_t *difference = (int8_t *)lv_obj_get_user_data(difference_label);
    *difference += 1;
    if (*difference > 0)
        lv_label_set_text_fmt(difference_label, "+%d", *difference);
    else
        lv_label_set_text_fmt(difference_label, "%d", *difference);
}

void SyncSubBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *sync_adjust_container = lv_obj_get_parent(btn);
    lv_obj_t *sync_adjust_bg = lv_obj_get_child(sync_adjust_container, 0);
    lv_obj_t *difference_label = lv_obj_get_child(sync_adjust_bg, 0);
    int8_t *difference = (int8_t *)lv_obj_get_user_data(difference_label);
    *difference -= 1;
    if (*difference > 0)
        lv_label_set_text_fmt(difference_label, "+%d", *difference);
    else
        lv_label_set_text_fmt(difference_label, "%d", *difference);
}

void SyncAdjustBtnCallback(lv_event_t *event)
{
    lv_obj_t *modal_bg = lv_obj_create(main_scr);
    lv_obj_set_size(modal_bg, lv_obj_get_width(main_scr), lv_obj_get_height(main_scr));
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_add_flag(modal_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *sync_adjust_container = lv_obj_create(modal_bg);
    lv_obj_set_size(sync_adjust_container, 300, 400);
    lv_obj_align(sync_adjust_container, LV_ALIGN_CENTER, 0, -120);
    lv_obj_set_style_bg_opa(sync_adjust_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(sync_adjust_container, LV_OPA_TRANSP, 0);

    lv_obj_t *sync_adjust_bg = lv_img_create(sync_adjust_container);
    lv_img_set_src(sync_adjust_bg, &SyncAdjust_bg_fit);
    lv_obj_center(sync_adjust_bg);

    lv_obj_t *difference_label = lv_label_create(sync_adjust_bg);
    static int8_t difference = 0;
    difference = 0;
    lv_label_set_text_fmt(difference_label, "%d", difference);
    lv_obj_set_style_text_font(difference_label, &AliPuHui_50, 0);
    lv_obj_align(difference_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_user_data(difference_label, &difference);
    lv_obj_set_style_text_color(difference_label, lv_color_white(), 0);

    lv_obj_t *unit_label = lv_label_create(sync_adjust_bg);
    lv_label_set_text(unit_label, "mA");
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_16, 0);
    lv_obj_align_to(unit_label, difference_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(unit_label, lv_color_white(), 0);

    lv_obj_t *sync_add_btn = lv_imgbtn_create(sync_adjust_container);
    lv_imgbtn_set_src(sync_add_btn, LV_IMGBTN_STATE_RELEASED, NULL, &AddButton_White_140_fit, NULL);
    lv_imgbtn_set_src(sync_add_btn, LV_IMGBTN_STATE_PRESSED, NULL, &AddButton_Green_fit, NULL);
    lv_obj_set_size(sync_add_btn, AddButton_White_140_fit.header.w, AddButton_White_140_fit.header.h);
    lv_obj_align(sync_add_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(sync_add_btn, SyncAddBtnCallback, LV_EVENT_CLICKED, NULL);

    lv_obj_t *sync_sub_btn = lv_imgbtn_create(sync_adjust_container);
    lv_imgbtn_set_src(sync_sub_btn, LV_IMGBTN_STATE_RELEASED, NULL, &SubtractButton_White_140_fit, NULL);
    lv_imgbtn_set_src(sync_sub_btn, LV_IMGBTN_STATE_PRESSED, NULL, &SubtractButton_fit, NULL);
    lv_obj_set_size(sync_sub_btn, SubtractButton_White_140_fit.header.w, SubtractButton_White_140_fit.header.h);
    lv_obj_align(sync_sub_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(sync_sub_btn, SyncSubBtnCallback, LV_EVENT_CLICKED, NULL);

    lv_obj_t *confirm_btn = lv_imgbtn_create(modal_bg);
    lv_imgbtn_set_src(confirm_btn, LV_IMGBTN_STATE_RELEASED, NULL, &ConfirmButtonBig_fit, NULL);
    lv_obj_set_size(confirm_btn, ConfirmButtonBig_fit.header.w, ConfirmButtonBig_fit.header.h);
    lv_obj_align(confirm_btn, LV_ALIGN_BOTTOM_MID, 0, -220);
    lv_obj_add_event_cb(confirm_btn, SyncConfirmBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);

    lv_obj_t *cancel_btn = lv_imgbtn_create(modal_bg);
    lv_imgbtn_set_src(cancel_btn, LV_IMGBTN_STATE_RELEASED, NULL, &CancelButtonBig_fit, NULL);
    lv_obj_set_size(cancel_btn, CancelButtonBig_fit.header.w, CancelButtonBig_fit.header.h);
    lv_obj_align(cancel_btn, LV_ALIGN_BOTTOM_MID, 0, -140);
    lv_obj_add_event_cb(cancel_btn, CancelBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);

}


void TextAreaCallback(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *textarea = lv_event_get_target(event);
    lv_obj_t *keyboard = lv_event_get_user_data(event);
    if (code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(keyboard, textarea);
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_LEAVE) {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}
void CalibPasswordConfirmBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(btn);
    lv_obj_t *calib_pwd_container = lv_obj_get_child(modal_bg, 0);
    lv_obj_t *calib_password_input = lv_obj_get_child(calib_pwd_container, 0);
    char *calib_password = (char *)lv_textarea_get_text(calib_password_input);
    lv_obj_t *current_scr = lv_scr_act();
    if (strcmp(calib_password, CALIB_PWD) == 0)
    {
        lv_obj_del_async(modal_bg);
        calib_scr = create_calib_scr();
        if (current_scr != calib_scr)
            lv_obj_set_user_data(calib_scr, current_scr);
        lv_scr_load(calib_scr);
    } 
}
void CalibBtnCallback(lv_event_t *event)
{
    lv_obj_t *current_scr = lv_scr_act();
    lv_obj_t *modal_bg = lv_obj_create(current_scr);
    lv_obj_set_size(modal_bg, lv_obj_get_width(current_scr), lv_obj_get_height(current_scr));
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_set_style_pad_all(modal_bg, 0, 0);
    lv_obj_add_flag(modal_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *calib_pwd_container = lv_obj_create(modal_bg);
    lv_obj_set_size(calib_pwd_container, 400, 400);
    lv_obj_align(calib_pwd_container, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_style_bg_opa(calib_pwd_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(calib_pwd_container, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(calib_pwd_container, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *keyboard = lv_keyboard_create(modal_bg);
    lv_obj_set_size(keyboard,LV_PCT(100), 300);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_event_cb(keyboard, lv_keyboard_def_event_cb);
    lv_obj_add_event_cb(keyboard, lv_keyboard_def_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *calib_password_input = lv_textarea_create(calib_pwd_container);
    lv_textarea_set_placeholder_text(calib_password_input, "请输入设置密码");
    lv_obj_set_style_text_font(calib_password_input, &AliPuHui_28, 0);
    lv_obj_set_style_bg_color(calib_password_input, lv_color_hex(0xe3e3e3), 0);
    lv_obj_set_style_radius(calib_password_input, 0, 0);
    lv_obj_set_size(calib_password_input, LV_PCT(100), 55);
    lv_obj_align(calib_password_input, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_scroll_dir(calib_password_input, LV_DIR_HOR);
    lv_obj_add_event_cb(calib_password_input, TextAreaCallback, LV_EVENT_ALL, keyboard);

    lv_obj_t *cancel_btn = lv_imgbtn_create(modal_bg);
    lv_imgbtn_set_src(cancel_btn, LV_IMGBTN_STATE_RELEASED, NULL, &CancelButtonBig_fit, NULL);
    lv_obj_set_size(cancel_btn, CancelButtonBig_fit.header.w, CancelButtonBig_fit.header.h);
    lv_obj_align_to(cancel_btn, keyboard, LV_ALIGN_OUT_TOP_MID, 0, -50);
    lv_obj_add_event_cb(cancel_btn, CancelBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);
    lv_obj_t *confirm_btn = lv_imgbtn_create(modal_bg);

    lv_imgbtn_set_src(confirm_btn, LV_IMGBTN_STATE_RELEASED, NULL, &ConfirmButtonBig_fit, NULL);
    lv_obj_set_size(confirm_btn, ConfirmButtonBig_fit.header.w, ConfirmButtonBig_fit.header.h);
    lv_obj_align_to(confirm_btn, cancel_btn, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_obj_add_event_cb(confirm_btn, CalibPasswordConfirmBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);

}
void BTConfirmBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(btn);
    lv_obj_t *bluetooth_container = lv_obj_get_child(modal_bg, 0);
    lv_obj_t *bluetooth_name_input = lv_obj_get_child(bluetooth_container, 1);
    char *bluetooth_name = (char *)lv_textarea_get_text(bluetooth_name_input);
    LV_LOG_USER("bluetooth name: %s", bluetooth_name);
    nvs_save_bluetooth_name(bluetooth_name);
    lv_obj_del_async(modal_bg);
}

void create_bluetooth_setting_modal(lv_obj_t *modal_bg)
{

    lv_obj_t *bluetooth_container = lv_obj_create(modal_bg);
    lv_obj_set_size(bluetooth_container, 400, 400);
    lv_obj_align(bluetooth_container, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_style_bg_opa(bluetooth_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(bluetooth_container, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(bluetooth_container, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *tag = lv_label_create(bluetooth_container);
    lv_label_set_text(tag, "蓝牙设置");
    lv_obj_set_style_text_font(tag, &AliPuHui_30, 0);
    lv_obj_set_style_text_color(tag, lv_color_white(), 0);

    lv_obj_t *keyboard = lv_keyboard_create(modal_bg);
    lv_obj_set_size(keyboard,LV_PCT(100), 300);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_event_cb(keyboard, lv_keyboard_def_event_cb);
    lv_obj_add_event_cb(keyboard, lv_keyboard_def_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *bluetooth_name_input = lv_textarea_create(bluetooth_container);
    lv_textarea_set_placeholder_text(bluetooth_name_input, "请输入蓝牙名称");
    lv_obj_set_style_text_font(bluetooth_name_input, &AliPuHui_28, 0);
    lv_obj_set_style_bg_color(bluetooth_name_input, lv_color_hex(0xe3e3e3), 0);
    lv_obj_set_style_radius(bluetooth_name_input, 0, 0);
    lv_obj_set_size(bluetooth_name_input,LV_PCT(100), 55);
    lv_obj_align(bluetooth_name_input, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_scroll_dir(bluetooth_name_input, LV_DIR_HOR);
    lv_obj_add_event_cb(bluetooth_name_input, TextAreaCallback, LV_EVENT_ALL, keyboard);

    lv_obj_t *cancel_btn = lv_imgbtn_create(modal_bg);
    lv_imgbtn_set_src(cancel_btn, LV_IMGBTN_STATE_RELEASED, NULL, &CancelButtonBig_fit, NULL);
    lv_obj_set_size(cancel_btn, CancelButtonBig_fit.header.w, CancelButtonBig_fit.header.h);
    lv_obj_align_to(cancel_btn, keyboard, LV_ALIGN_OUT_TOP_MID, 0, -50);
    lv_obj_add_event_cb(cancel_btn, CancelBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);
    lv_obj_t *confirm_btn = lv_imgbtn_create(modal_bg);

    lv_imgbtn_set_src(confirm_btn, LV_IMGBTN_STATE_RELEASED, NULL, &ConfirmButtonBig_fit, NULL);
    lv_obj_set_size(confirm_btn, ConfirmButtonBig_fit.header.w, ConfirmButtonBig_fit.header.h);
    lv_obj_align_to(confirm_btn, cancel_btn, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_obj_add_event_cb(confirm_btn, BTConfirmBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);

}

void BTPasswordConfirmBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(btn);
    lv_obj_t *bluetooth_container = lv_obj_get_child(modal_bg, 0);
    lv_obj_t *bluetooth_password_input = lv_obj_get_child(bluetooth_container, 0);
    char *bluetooth_password = (char *)lv_textarea_get_text(bluetooth_password_input);
    if (strcmp(bluetooth_password, BT_SETTING_PWD) == 0)
    {
        lv_obj_clean(modal_bg);
        create_bluetooth_setting_modal(modal_bg);
    }
}

void BluetoothBtnCallback(lv_event_t *event)
{
    lv_obj_t *modal_bg = lv_obj_create(main_scr);
    lv_obj_set_size(modal_bg, lv_obj_get_width(main_scr), lv_obj_get_height(main_scr));
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_set_style_pad_all(modal_bg, 0, 0);
    lv_obj_add_flag(modal_bg, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *bluetooth_container = lv_obj_create(modal_bg);
    lv_obj_set_size(bluetooth_container, 400, 400);
    lv_obj_align(bluetooth_container, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_style_bg_opa(bluetooth_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(bluetooth_container, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(bluetooth_container, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *keyboard = lv_keyboard_create(modal_bg);
    lv_obj_set_size(keyboard,LV_PCT(100), 300);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_event_cb(keyboard, lv_keyboard_def_event_cb);
    lv_obj_add_event_cb(keyboard, lv_keyboard_def_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *bluetooth_password_input = lv_textarea_create(bluetooth_container);
    lv_textarea_set_placeholder_text(bluetooth_password_input, "请输入设置密码");
    lv_obj_set_style_text_font(bluetooth_password_input, &AliPuHui_28, 0);
    lv_obj_set_style_bg_color(bluetooth_password_input, lv_color_hex(0xe3e3e3), 0);
    lv_obj_set_style_radius(bluetooth_password_input, 0, 0);
    lv_obj_set_size(bluetooth_password_input, LV_PCT(100), 55);
    lv_obj_align(bluetooth_password_input, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_scroll_dir(bluetooth_password_input, LV_DIR_HOR);
    lv_obj_add_event_cb(bluetooth_password_input, TextAreaCallback, LV_EVENT_ALL, keyboard);

    lv_obj_t *cancel_btn = lv_imgbtn_create(modal_bg);
    lv_imgbtn_set_src(cancel_btn, LV_IMGBTN_STATE_RELEASED, NULL, &CancelButtonBig_fit, NULL);
    lv_obj_set_size(cancel_btn, CancelButtonBig_fit.header.w, CancelButtonBig_fit.header.h);
    lv_obj_align_to(cancel_btn, keyboard, LV_ALIGN_OUT_TOP_MID, 0, -50);
    lv_obj_add_event_cb(cancel_btn, CancelBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(cancel_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);
    lv_obj_t *confirm_btn = lv_imgbtn_create(modal_bg);

    lv_imgbtn_set_src(confirm_btn, LV_IMGBTN_STATE_RELEASED, NULL, &ConfirmButtonBig_fit, NULL);
    lv_obj_set_size(confirm_btn, ConfirmButtonBig_fit.header.w, ConfirmButtonBig_fit.header.h);
    lv_obj_align_to(confirm_btn, cancel_btn, LV_ALIGN_OUT_TOP_MID, 0, -20);
    lv_obj_add_event_cb(confirm_btn, BTPasswordConfirmBtnCallback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnPressedCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(confirm_btn, ImgBtnReleasedCallback, LV_EVENT_RELEASED, NULL);

}

void ChildLockBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    bool *is_locked = (bool *)lv_obj_get_user_data(btn);
    *is_locked = !(*is_locked);
    if (*is_locked) {
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &LockIconTransparent_fit, NULL);
        lv_obj_set_size(btn, LockIconTransparent_fit.header.w, LockIconTransparent_fit.header.h);
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *channel = get_channel_by_index(i);
            set_channel_state(channel, UI_CHANNEL_STATE_DISABLED, false);
        }

        lv_obj_t *clear_btn = lv_obj_get_child(main_scr, 1);
        lv_obj_clear_flag(clear_btn, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_t *sync_adjust_btn = lv_obj_get_child(main_scr, 2);
        lv_obj_clear_flag(sync_adjust_btn, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_t *start_btn = lv_obj_get_child(main_scr, 3);
        lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Gray_fit, NULL);
        lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_t *start_label = lv_obj_get_child(main_scr, 4);
        lv_obj_set_style_text_color(start_label, lv_color_hex(0xdfe1ea), 0);
    }
    else {
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &UnlockIconTransparent_fit, NULL);
        lv_obj_set_size(btn, UnlockIconTransparent_fit.header.w, UnlockIconTransparent_fit.header.h);
        lv_obj_align(btn, LV_ALIGN_LEFT_MID, 0, 0);
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *channel = get_channel_by_index(i);
            UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
            set_channel_state(channel, ch->prev_state, false);
        }

        lv_obj_t *clear_btn = lv_obj_get_child(main_scr, 1);
        lv_obj_add_flag(clear_btn, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_t *sync_adjust_btn = lv_obj_get_child(main_scr, 2);
        lv_obj_add_flag(sync_adjust_btn, LV_OBJ_FLAG_CLICKABLE);

        update_start_btn_status();
        // lv_obj_t *start_btn = lv_obj_get_child(main_scr, 3);
        // lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Green_fit, NULL);
        // lv_obj_add_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);
    }
}

void TimerLabelClickCallback(lv_event_t *event)
{
    lv_obj_t *channel = lv_event_get_user_data(event);
    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);

    if (ch->timer.state == UI_TIMER_STATE_STOP || ch->timer.state == UI_TIMER_STATE_UNINIT)
    {
        bool impedance_flag = adc_check_impedance(ch->index);
        if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START) {
            if (impedance_flag) {
                set_channel_state(channel, UI_CHANNEL_STATE_DROPPED, false);
            }
            else {
                lv_timer_pause(ch->tmp_timer);
                set_channel_timer_state(channel, UI_TIMER_STATE_START);
                ens_send_start_cmd((1 << ch->index));
            }
        }
    }
    else
    {
        set_channel_timer_state(channel, UI_TIMER_STATE_STOP);
        ens_send_stop_cmd((1 << ch->index));
    }

}

void StimulationStartBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    bool *is_stimulation_running = (bool *)lv_obj_get_user_data(btn);
    if (*is_stimulation_running == false) {
        // impedance_flag = 0;
        *is_stimulation_running = !(*is_stimulation_running);
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &PauseButton_fit, NULL);
        lv_obj_t *stimulation_start_label = lv_obj_get_child(main_scr, 4);
        lv_label_set_text(stimulation_start_label, "全部暂停");
        uint8_t channel_sel = 0;
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *channel = get_channel_by_index(i);
            UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
            if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START)
            {
                bool impedance_flag = adc_check_impedance(i);
                if (impedance_flag) {
                    // if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state != UI_TIMER_STATE_START)
                    set_channel_state(channel, UI_CHANNEL_STATE_DROPPED, false);
                }
                else {
                    lv_timer_pause(ch->tmp_timer);
                    set_channel_timer_state(channel, UI_TIMER_STATE_START);
                    channel_sel |= (1 << i);
                }
            }
        }
        ens_send_start_cmd(channel_sel);
        pause_hibernation_timer();

    }
    else {
        *is_stimulation_running = !(*is_stimulation_running);
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartButton_Green_fit, NULL);
        lv_obj_t *stimulation_start_label = lv_obj_get_child(main_scr, 4);
        lv_label_set_text(stimulation_start_label, "全部开始");
        uint8_t channel_sel = 0;
        for (int i = 0; i < 4; ++i) {
            lv_obj_t *channel = get_channel_by_index(i);
            UI_Channel *ch = (UI_Channel *) lv_obj_get_user_data(channel);
            if (ch->state == UI_CHANNEL_STATE_ADDED && ch->timer.state == UI_TIMER_STATE_START)
            {
                set_channel_timer_state(channel, UI_TIMER_STATE_STOP);
                channel_sel |= (1 << i);
            }
        }
        ens_send_stop_cmd(channel_sel);
        resume_hibernation_timer();
    }
}

void DropModalDelCallback(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_target(event);
    lv_obj_t *modal_bg = lv_obj_get_parent(obj);
    lv_obj_t *channel = lv_event_get_user_data(event);
    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
    if (adc_check_impedance(ch->index) == 0) {// 代表阻抗测试通过
        lv_obj_del_async(modal_bg);
    }
}

void ModalDelCallback(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_target(event);
    lv_obj_del_async(obj);
}

void ChannelLabelClickCallback(lv_event_t *event)
{
    lv_obj_t *channel = lv_event_get_user_data(event);
    UI_Channel *ch = (UI_Channel*)lv_obj_get_user_data(channel);
    Plan *pPlan = ch->pPlan;

    lv_obj_t *modal_container = lv_obj_create(main_scr);
    lv_obj_set_size(modal_container, lv_obj_get_width(main_scr), lv_obj_get_height(main_scr));
    lv_obj_set_style_bg_opa(modal_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(modal_container, 0, 0);
    lv_obj_set_style_border_width(modal_container, 0, 0);
    lv_obj_add_flag(modal_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(modal_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(modal_container, ModalDelCallback, LV_EVENT_CLICKED, modal_container);

    lv_obj_t *modal_bg = lv_obj_create(modal_container);
    lv_obj_set_size(modal_bg,LV_PCT(100), 500);
    lv_obj_align(modal_bg, LV_ALIGN_BOTTOM_MID, 0, 40);
    lv_obj_set_style_bg_color(modal_bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_bg, LV_OPA_80, 0);
    lv_obj_set_style_border_width(modal_bg, 0, 0);
    lv_obj_set_style_pad_all(modal_bg, 0, 0);
    lv_obj_set_style_radius(modal_bg, 50, 0);

    lv_obj_t *title_label = lv_label_create(modal_bg);
    lv_label_set_text(title_label, "方案详情:");
    lv_obj_set_style_text_font(title_label, &AliPuHui_30, 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 30, 30);

    lv_obj_t *plan_name_container = lv_obj_create(modal_bg);
    lv_obj_align(plan_name_container, LV_ALIGN_TOP_LEFT, 30, 80);
    lv_obj_set_size(plan_name_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(plan_name_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(plan_name_container, LV_OPA_TRANSP, 0);
    lv_obj_t *plan_name_icon = lv_img_create(plan_name_container);
    lv_img_set_src(plan_name_icon, &Info_Icon_Detail_fit);
    lv_obj_set_style_pad_all(plan_name_icon, 5, 0);
    lv_obj_align(plan_name_icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *plan_name_label = lv_label_create(plan_name_container);
    lv_label_set_text_fmt(plan_name_label, "方案名称: %s", pPlan->name);
    lv_obj_set_style_text_font(plan_name_label, &AliPuHui_24, 0);
    lv_obj_set_style_text_color(plan_name_label, lv_color_white(), 0);
    lv_obj_align_to(plan_name_label, plan_name_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    lv_obj_t *wave_type_container = lv_obj_create(modal_bg);
    lv_obj_align(wave_type_container, LV_ALIGN_TOP_LEFT, 30, 120);
    lv_obj_set_size(wave_type_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(wave_type_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(wave_type_container, LV_OPA_TRANSP, 0);
    lv_obj_t *wave_type_icon = lv_img_create(wave_type_container);
    lv_img_set_src(wave_type_icon, &Info_Icon_Wavetype_fit);
    lv_obj_set_style_pad_all(wave_type_icon, 5, 0);
    lv_obj_align(wave_type_icon, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *wave_type_label = lv_label_create(wave_type_container);
    switch (pPlan->wave_type)
    {
    case WAVE_TYPE_DUAL_PHASE_SQUARE:
        lv_label_set_text(wave_type_label, "波形： 双相方波");
        break;
    case WAVE_TYPE_SINGLE_PHASE_SQUARE:
        lv_label_set_text(wave_type_label, "波形： 单相方波");
        break;
    case WAVE_TYPE_TRIANGLE:
        lv_label_set_text(wave_type_label, "波形： 三角波");
        break;
    case WAVE_TYPE_SINE:
        lv_label_set_text(wave_type_label, "波形： 正弦波");
        break;
    default:
        break;
    }
    lv_obj_set_style_text_font(wave_type_label, &AliPuHui_24, 0);
    lv_obj_set_style_text_color(wave_type_label, lv_color_white(), 0);
    lv_obj_align_to(wave_type_label, wave_type_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    lv_obj_t *wave_freq_container = lv_obj_create(modal_bg);
    lv_obj_align(wave_freq_container, LV_ALIGN_TOP_LEFT, 30, 160);
    lv_obj_set_size(wave_freq_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(wave_freq_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(wave_freq_container, LV_OPA_TRANSP, 0);
    lv_obj_t *wave_freq_icon = lv_img_create(wave_freq_container);
    lv_img_set_src(wave_freq_icon, &Info_Icon_Freq_fit);
    lv_obj_set_style_pad_all(wave_freq_icon, 5, 0);
    lv_obj_align(wave_freq_icon, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *width_container = lv_obj_create(modal_bg);
    lv_obj_align(width_container, LV_ALIGN_TOP_LEFT, 30, 200);
    lv_obj_set_size(width_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(width_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(width_container, LV_OPA_TRANSP, 0);
    lv_obj_t *width_icon = lv_img_create(width_container);
    lv_img_set_src(width_icon, &Info_Icon_Width_fit);
    lv_obj_set_style_pad_all(width_icon, 5, 0);
    lv_obj_align(width_icon, LV_ALIGN_LEFT_MID, 0, 0);

    if (pPlan->freq_type == FREQ_TYPE_FIXED)
    {
        lv_obj_t *freq_label = lv_label_create(wave_freq_container);
        lv_label_set_text_fmt(freq_label, "脉冲频率: %d Hz", pPlan->freq_min);
        lv_obj_set_style_text_font(freq_label, &AliPuHui_24, 0);
        lv_obj_set_style_text_color(freq_label, lv_color_white(), 0);
        lv_obj_align_to(freq_label, wave_freq_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        lv_obj_t *pulse_width_label = lv_label_create(width_container);
        lv_label_set_text_fmt(pulse_width_label, "脉冲宽度: %d us", pPlan->pulse_width_us);
        lv_obj_set_style_text_font(pulse_width_label, &AliPuHui_24, 0);
        lv_obj_set_style_text_color(pulse_width_label, lv_color_white(), 0);
        lv_obj_align_to(pulse_width_label, width_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
    else if (pPlan->freq_type == FREQ_TYPE_VARY)
    {
        lv_obj_t *carrier_freq_label = lv_label_create(wave_freq_container);
        lv_label_set_text_fmt(carrier_freq_label, "载波频率: %d Hz", pPlan->freq_max);
        lv_obj_set_style_text_font(carrier_freq_label, &AliPuHui_24, 0);
        lv_obj_set_style_text_color(carrier_freq_label, lv_color_white(), 0);
        lv_obj_align_to(carrier_freq_label, wave_freq_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        lv_obj_t *mod_freq_label = lv_label_create(width_container);
        lv_label_set_text_fmt(mod_freq_label, "调制频率: %d us", pPlan->freq_min);
        lv_obj_set_style_text_font(mod_freq_label, &AliPuHui_24, 0);
        lv_obj_set_style_text_color(mod_freq_label, lv_color_white(), 0);
        lv_obj_align_to(mod_freq_label, width_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
    

}

void ProgressBarIndicatorCallback(lv_event_t * event)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(event);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj = lv_event_get_target(event);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                    label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
        /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}

void CurrentWarningModalCancelCallback(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_user_data(event);
    lv_obj_del_async(obj);
}

void CurrentWarningModalConfirmCallback(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_user_data(event);
    lv_obj_t *channel = lv_obj_get_user_data(obj);
    // set_channel_current_by_force(channel, UI_WARNING_CURRENT_LIMIT + 1);
    refresh_channel_current(channel, 1, true);
    lv_obj_del_async(obj);

}

void MainScrBtnCallback(lv_event_t *event)
{
    lv_scr_load(main_scr);
}

void calib_progress_task(void *arg)
{
    lv_obj_t *progress_bar = (lv_obj_t *)arg;
    uint16_t progress = 0;
    while (1)
    {
        xQueueReceive(imu_read_progress_queue, &progress, portMAX_DELAY);
        lvgl_lock();
        lv_bar_set_value(progress_bar, progress * 100 / IMU_BATCH_SIZE, LV_ANIM_OFF);
        lvgl_unlock();
        if (progress == IMU_BATCH_SIZE)
        {
            lvgl_lock();
            uint8_t *calib_cnt = (uint8_t *)lv_obj_get_user_data(progress_bar);
            lv_obj_t *scr = lv_obj_get_parent(progress_bar);
            lv_obj_t *label = lv_obj_get_child(scr, 3);
            lv_obj_t *start_btn_container = lv_obj_get_child(scr, 4);
            lv_obj_t *start_btn = lv_obj_get_child(start_btn_container, 0);
            lv_imgbtn_set_src(start_btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartIconTransparent_fit, NULL);
            UI_CalibState *calib_state = (UI_CalibState *)lv_obj_get_user_data(scr);
            *calib_state = UI_CALIB_STATE_STOP;
            (*calib_cnt)++;
            if (*calib_cnt == 1)
            {
                lv_label_set_text(label, "第二次");
                lv_obj_t *start_btn_label = lv_obj_get_child(start_btn_container, 1);
                lv_label_set_text(start_btn_label, "开始校准");
            }
            else if (*calib_cnt == 2)
            {
                lv_label_set_text(label, "第三次");
                lv_obj_t *start_btn_label = lv_obj_get_child(start_btn_container, 1);
                lv_label_set_text(start_btn_label, "开始校准");
            }
            else if (*calib_cnt == 3)
            {
                lv_obj_del_async(progress_bar);
                lv_obj_del_async(start_btn_container);
                lv_label_set_text_fmt(label, "校准结果  %dHZ", 4);
                lv_obj_t *btn = lv_btn_create(scr);
                lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                lv_obj_align_to(btn, label, LV_ALIGN_OUT_BOTTOM_MID, -55, 20);
                lv_obj_add_event_cb(btn, MainScrBtnCallback, LV_EVENT_CLICKED, NULL);
                lv_obj_set_style_bg_color(btn, lv_color_hex(0x54bcbd), 0);
                lv_obj_t *btn_label = lv_label_create(btn);
                lv_obj_center(btn_label);
                lv_label_set_text(btn_label, "开始刺激");
                lv_obj_set_style_text_font(btn_label, &AliPuHui_30, 0);
                
            }
            lvgl_unlock();
            break;
        }
    }
    vTaskDelete(NULL);
}

void CalibStartBtnCallback(lv_event_t *event)
{
    lv_obj_t *btn = lv_event_get_target(event);
    lv_obj_t *scr = lv_event_get_user_data(event);
    lv_obj_t *container = lv_obj_get_parent(btn);
    lv_obj_t *label = lv_obj_get_child(container, 1);
    UI_CalibState *calib_state = (UI_CalibState *)lv_obj_get_user_data(scr);
    lv_obj_t *progress_bar = lv_obj_get_child(scr, 2);
    // lv_obj_t *label = lv_obj_get_child(scr, 3);
    uint8_t *calib_cnt = (uint8_t *)lv_obj_get_user_data(label);
    if (*calib_state == UI_CALIB_STATE_STOP) {
        *calib_state = UI_CALIB_STATE_START;
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &PauseIconTransparent_fit, NULL);
        lv_label_set_text(label, "暂停");
        xTaskCreate(calib_progress_task, "calib_progress_task", 1024 * 8, progress_bar, 1, NULL);
        xTaskCreate(IMU_read_data_task, "IMU_read_data_Task", 1024 * 8, NULL, 1, &imu_task_handle);
    }
    else if (*calib_state == UI_CALIB_STATE_START) {
        *calib_state = UI_CALIB_STATE_PAUSE;
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &StartIconTransparent_fit, NULL);
        vTaskSuspend(imu_task_handle);
        lv_label_set_text(label, "开始校准");
    }
    else if (*calib_state == UI_CALIB_STATE_PAUSE) {
        *calib_state = UI_CALIB_STATE_START;
        lv_label_set_text(label, "暂停");
        lv_imgbtn_set_src(btn, LV_IMGBTN_STATE_RELEASED, NULL, &PauseIconTransparent_fit, NULL);
        vTaskResume(imu_task_handle);
    }
    
}
