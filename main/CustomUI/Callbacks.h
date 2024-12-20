//
// Created by 13651 on 2024/11/19.
//

#ifndef LVGL_CALLBACKS_H
#define LVGL_CALLBACKS_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_port.h"
#include "CustomUI.h"
#include "ADC.h"
#include "uart_ens.h"
#include "nvs_op.h"
#include "imu.h"


void ImgBtnPressedCallback(lv_event_t *event);
void ImgBtnReleasedCallback(lv_event_t *event);
void AddSchemeBtnCallback(lv_event_t *event);
void BackBtnCallback(lv_event_t *event);
void PlanOptionCallback(lv_event_t *event);
void ClearBtnCallback(lv_event_t *event);
void PrevPageBtnCallback(lv_event_t *event);
void NextPageBtnCallback(lv_event_t *event);
void SyncAdjustBtnCallback(lv_event_t *event);
void AddCurrentBtnCallback(lv_event_t *event);
void SubCurrentBtnCallback(lv_event_t *event);
void CalibBtnCallback(lv_event_t *event);
void BluetoothBtnCallback(lv_event_t *event);
void ChildLockBtnCallback(lv_event_t *event);
void TimerLabelClickCallback(lv_event_t *event);
void StimulationStartBtnCallback(lv_event_t *event);
void ChannelLabelClickCallback(lv_event_t *event);
void ModalDelCallback(lv_event_t *event);
void DropModalDelCallback(lv_event_t *event);
void ProgressBarIndicatorCallback(lv_event_t * event);
void CurrentWarningModalCancelCallback(lv_event_t *event);
void CurrentWarningModalConfirmCallback(lv_event_t *event);
// void CurrentWarningModalCloseCallback(lv_event_t *event);
void CalibStartBtnCallback(lv_event_t *event);


#endif //LVGL_CALLBACKS_H
