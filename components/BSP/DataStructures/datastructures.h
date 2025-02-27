#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

#include "stdint.h"
#include "custom_types.h"
#include "nvs_op.h"

// extern SchemeSet NMES;
// extern SchemeSet TENS;
// extern SchemeSet MNS;
// extern SchemeSet TNS;
// extern SchemeSet Others;
// extern SchemeSet test_schemeset;
extern Plan saved_plans[MAX_PLAN_NUM];
extern Plan bt_plan;

void reset_schemeset();
void init_plans();
void update_plan_position_by_used_times(Plan *pPlan);
void plan_to_data(Plan *plan, uint8_t channel_id, uint8_t *out_data);

#endif /* DATASTRUCTURES_H_ */