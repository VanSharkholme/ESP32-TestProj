#ifndef NVS_OP_H_
#define NVS_OP_H_

#include "nvs_flash.h"
#include "rgb_lcd.h"
#include "datastructures.h"

#define NVS_FORCE_RESET_PLANS 0

void nvs_op_init();
void nvs_test();
void nvs_save_bluetooth_name(char *name);
char *nvs_get_bluetooth_name();
// void nvs_save_schemeset_count(uint8_t count);
// uint8_t nvs_get_schemeset_count();
// void nvs_save_plan_count(uint8_t count);
// uint8_t nvs_get_plan_count();
// void nvs_save_schemeset(SchemeSet schemeset, uint8_t index);
// SchemeSet *nvs_get_schemeset(uint8_t index);
void nvs_save_plans(Plan *plans);
void nvs_get_plans(Plan *plans);

#endif /* NVS_OP_H_ */