//
// Created by 13651 on 2024/11/16.
//

#ifndef LVGL_CUSTOMUI_H
#define LVGL_CUSTOMUI_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "stdio.h"
#include "Callbacks.h"
#include "data_structures.h"
#include "datastructures.h"
#include "TCA9555.h"
#include "timer.h"
#include "FuelGauge.h"

LV_IMG_DECLARE(AddButton_Green_fit)
LV_IMG_DECLARE(AddButton_White_120_fit)
LV_IMG_DECLARE(AddButton_White_140_fit)
LV_IMG_DECLARE(ArcProgress_fit)
LV_IMG_DECLARE(BatteryIcon_fit)
LV_IMG_DECLARE(BG1_fit)
LV_IMG_DECLARE(BG2_fit)
LV_IMG_DECLARE(BG3_fit)
LV_IMG_DECLARE(BluetoothIcon_fit)
LV_IMG_DECLARE(CalibIcon_fit)
LV_IMG_DECLARE(CancelButtonBig_fit)
LV_IMG_DECLARE(ChargeRing_fit)
LV_IMG_DECLARE(Clear_fit)
LV_IMG_DECLARE(ConfirmButtonBig_fit)
LV_IMG_DECLARE(HorizontalProgressBar_fit)
LV_IMG_DECLARE(LightningSymbol_fit)
LV_IMG_DECLARE(LockButton_Black_fit)
LV_IMG_DECLARE(LockIconTransparent_fit)
LV_IMG_DECLARE(MenuIcon_fit)
LV_IMG_DECLARE(PauseButton_fit)
LV_IMG_DECLARE(PauseIconTransparent_fit)
LV_IMG_DECLARE(PostureDemo_fit)
LV_IMG_DECLARE(SettingIcon_fit)
LV_IMG_DECLARE(StartButton_Gray_fit)
LV_IMG_DECLARE(StartButton_Green_fit)
LV_IMG_DECLARE(StartIconTransparent_fit)
LV_IMG_DECLARE(SubtractButton_fit)
LV_IMG_DECLARE(SubtractButton_White_120_fit)
LV_IMG_DECLARE(SubtractButton_White_140_fit)
LV_IMG_DECLARE(SyncAdjust_fit)
LV_IMG_DECLARE(SyncAdjust_bg_fit)
LV_IMG_DECLARE(WarningIcon_fit)
LV_IMG_DECLARE(BackButton_fit)
LV_IMG_DECLARE(BatteryBar_fit)
LV_IMG_DECLARE(EmptyBattery_fit)
LV_IMG_DECLARE(Info_Icon_Detail_fit)
LV_IMG_DECLARE(Info_Icon_Freq_fit)
LV_IMG_DECLARE(Info_Icon_Wavetype_fit)
LV_IMG_DECLARE(Info_Icon_Width_fit)
LV_IMG_DECLARE(Bluetooth_Disconnect_fit)
LV_IMG_DECLARE(NextPageBtn_fit)
LV_IMG_DECLARE(PrevPageBtn_fit)
LV_IMG_DECLARE(UnlockIconTransparent_fit)

LV_FONT_DECLARE(AliPuHui_20)
LV_FONT_DECLARE(AliPuHui_24)
LV_FONT_DECLARE(AliPuHui_28)
LV_FONT_DECLARE(AliPuHui_30)
LV_FONT_DECLARE(AliPuHui_40)
LV_FONT_DECLARE(AliPuHui_50)
LV_FONT_DECLARE(AliPuHui_70)


#define UI_CURRENT_ARC_LINE_WIDTH 6

#define UI_PLAN_PAGE_NUM_MAX (MAX_PLAN_NUM / UI_PLAN_NUM_PER_PAGE + 1)
#define UI_PLAN_NUM_PER_PAGE 6
#define UI_WARNING_CURRENT_LIMIT 60

#define BT_SETTING_PWD "123456"
#define CALIB_PWD "654321"

typedef enum {
    UI_CHANNEL_STATE_UNINIT,
    UI_CHANNEL_STATE_DISABLED,
    UI_CHANNEL_STATE_NOT_ADDED,
    UI_CHANNEL_STATE_ADDED,
    UI_CHANNEL_STATE_DROPPED
} UI_ChannelState;

typedef enum {
    UI_TIMER_STATE_UNINIT,
    UI_TIMER_STATE_STOP,
    UI_TIMER_STATE_START,
} UI_ChannelTimerState;

typedef struct {
    // uint8_t timer_min;
    // uint8_t timer_sec;
    uint16_t remaining_seconds;
    UI_ChannelTimerState state;
    gptimer_handle_t timer_handle;
    lv_timer_t *lvtimer;
} UI_ChannelTimer;

// typedef struct {
//     char *name;
//     uint8_t current_mA;
//     uint8_t timer_min;
//     uint8_t timer_sec;
// // todo: add more attributes
// } UI_Plan;

typedef struct {
    char *name;
    uint8_t index;
    UI_ChannelState state;
    UI_ChannelState prev_state;
    UI_ChannelTimer timer;
    lv_timer_t *tmp_timer;
//    UI_Plan *p_uiplan;
    Plan *pPlan;
} UI_Channel;

typedef enum {
    UI_CALIB_STATE_STOP,
    UI_CALIB_STATE_START,
    UI_CALIB_STATE_PAUSE,
} UI_CalibState;


extern lv_obj_t *main_scr;
extern lv_obj_t *scheme_scr;
extern lv_obj_t *calib_scr;
// extern SchemeSet schemeSet1;
// extern SchemeSet schemeSet2;
// extern SchemeSet schemeSet3;
// extern SchemeSet schemeSet4;
// extern SchemeSet schemeSet5;
// extern SchemeSet schemeSet6;
// extern SchemeSet schemeSet7;
// extern SchemeSet schemeSet8;
// extern SchemeSet schemeSet9;
// extern SchemeSet schemeSet10;
// extern SchemeSet schemeSet11;
// extern SchemeSet schemeSet12;

extern uint8_t valid_page_num;
extern uint8_t current_page_num;
extern bool is_hibernating;

void clear_all_channels();
void set_channel_state(lv_obj_t *channel, UI_ChannelState state, bool is_force_refresh);
void set_scheme_set_page(uint8_t page);
void refresh_channel_current(lv_obj_t *in_channel, int8_t difference, bool is_force_refresh);
void set_channel_current_by_force(lv_obj_t *in_channel, uint8_t current);
lv_obj_t *get_channel_by_index(uint8_t index);
uint8_t get_channel_needing_modifying();
uint8_t get_channel_with_same_plan(uint8_t plan_id);
void set_channel_plan(uint8_t index, Plan *plan, bool is_force_refresh);
uint8_t find_spare_channel(uint8_t *indicator);
lv_obj_t *create_warning_modal(char *msg);
void set_channel_timer_state(lv_obj_t *current_container, UI_ChannelTimerState state);
void update_scheme_pages();
void set_battery_level();
void load_charging_scr();
void exit_charging_scr();
void load_main_scr();
void pause_hibernation_timer();
void resume_hibernation_timer();
void reset_hibernation_timer();
void update_start_btn_status();
lv_obj_t *create_calib_scr();
void CustomUI();

#endif //LVGL_CUSTOMUI_H
