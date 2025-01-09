#include "datastructures.h"
#include "string.h"

Plan saved_plans[MAX_PLAN_NUM] = {0};
Plan bt_plan;

// SchemeSet NMES = {
//     .name = "神经肌肉电刺激",
//     .plan_num = 0,
//     .plans = {0}
// };

// SchemeSet TENS = {
//     .name = "经皮神经电刺激",
//     .plan_num = 0,
//     .plans = {0}
// };

// SchemeSet MNS = {
//     .name = "正中神经电刺激",
//     .plan_num = 0,
//     .plans = {0}
// };

// SchemeSet TNS = {
//     .name = "胫神经电刺激",
//     .plan_num = 0,
//     .plans = {0}
// };

// SchemeSet Others = {
//     .name = "其他",
//     .plan_num = 0,
//     .plans = {0}
// };

Plan test_plans[5] ={
        {
            .name = "肌力增强--上臂",
            .current_mA = 20,
            .total_time_min = 60
        },
        {
            .name = "肌力增强--上臂",
            .current_mA = 15,
            .total_time_min = 25
        },
        {
            .name = "肌力增强--上臂",
            .current_mA = 50,
            .total_time_min = 1,
        },
        {
            .name = "肌力增强--上臂",
            .current_mA = 25,
            .total_time_min = 15,
        },
        {
            .name = "肌力增强--上臂",
            .current_mA = 10,
            .total_time_min = 35,
        }
};

// SchemeSet test_schemeset = {
//         .name = "神经电刺激",
//         .plan_num = 5,
//         .plans = {
//             &test_plans[0],
//             &test_plans[1],
//             &test_plans[2],
//             &test_plans[3],
//             &test_plans[4]
//         }
// };

void print_plans(Plan *plans)
{
    for (uint8_t i = 0; i < MAX_PLAN_NUM; i++) {
        printf("===========plan %d:\n", i);
        printf("id: %d\n", plans[i].id);
        // printf("scheme_type: %d\n", plans[i].scheme_type);
        printf("name_len: %d\n", plans[i].name_len);
        printf("name: %s\n", plans[i].name);
        printf("wave_type: %d\n", plans[i].wave_type);
        printf("current_mA: %d\n", plans[i].current_mA);
        printf("pulse_width_us: %d\n", plans[i].pulse_width_us);
        printf("pulse_interval: %d\n", plans[i].pulse_interval);
        printf("freq_type: %d\n", plans[i].freq_type);
        printf("freq_min: %d\n", plans[i].freq_min);
        printf("freq_max: %d\n", plans[i].freq_max);
        printf("wave_rise_ms: %d\n", plans[i].wave_rise_ms);
        printf("work_time_sec: %d\n", plans[i].work_time_sec);
        printf("wave_fall_ms: %d\n", plans[i].wave_fall_ms);
        printf("work_time_sec: %d\n", plans[i].work_time_sec);
        printf("break_time_sec: %d\n", plans[i].break_time_sec);
        printf("total_time_min: %d\n", plans[i].total_time_min);
    }
}


void reset_schemeset()
{
    // saved_plans[0].id = 1,
    // strcpy(saved_plans[0].name, "低频-标准方案");
    // saved_plans[0].name_len = 20,
    // saved_plans[0].channel_num = 1,
    // saved_plans[0].used_times = 0;
    // saved_plans[0].is_bluetooth = false;
    // saved_plans[0].current_mA = 100,
    // saved_plans[0].total_time_min = 30,
    // saved_plans[0].pulse_width_us = 300,
    // saved_plans[0].freq_min = 40,
    // saved_plans[0].freq_max = 40,
    // saved_plans[0].freq_type = FREQ_TYPE_FIXED,
    // saved_plans[0].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE,
    // saved_plans[0].wave_rise_ms = 1000;
    // saved_plans[0].work_time_sec = 3;
    // saved_plans[0].wave_fall_ms = 1000;
    // saved_plans[0].break_time_sec = 10;

    // saved_plans[1].id = 2;
    // strcpy(saved_plans[1].name, "低频-脑卒中上肢功能障碍方案");
    // saved_plans[1].name_len = 41;
    // saved_plans[1].channel_num = 1,
    // saved_plans[1].used_times = 0;
    // saved_plans[1].is_bluetooth = false;
    // saved_plans[1].current_mA = 10;
    // saved_plans[1].total_time_min = 20;
    // saved_plans[1].pulse_width_us = 200;
    // saved_plans[1].freq_min = 35;
    // saved_plans[1].freq_max = 35;
    // saved_plans[1].freq_type = FREQ_TYPE_FIXED;
    // saved_plans[1].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    // saved_plans[1].wave_rise_ms = 0;
    // saved_plans[1].work_time_sec = 5;
    // saved_plans[1].wave_fall_ms = 0;
    // saved_plans[1].break_time_sec = 20;

    // saved_plans[2].id = 3;
    // strcpy(saved_plans[2].name, "低频-急迫性尿失禁方案");
    // saved_plans[2].name_len = 32;
    // saved_plans[2].channel_num = 1,
    // saved_plans[2].used_times = 0;
    // saved_plans[2].is_bluetooth = false;
    // saved_plans[2].current_mA = 10;
    // saved_plans[2].total_time_min = 20;
    // saved_plans[2].pulse_width_us = 300;
    // saved_plans[2].freq_min = 10;
    // saved_plans[2].freq_max = 10;
    // saved_plans[2].freq_type = FREQ_TYPE_FIXED;
    // saved_plans[2].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    // saved_plans[2].wave_rise_ms = 0;
    // saved_plans[2].work_time_sec = 10;
    // saved_plans[2].wave_fall_ms = 0;
    // saved_plans[2].break_time_sec = 5;

    // saved_plans[3].id = 4;
    // strcpy(saved_plans[3].name, "低频-OAB胫神经方案");
    // saved_plans[3].name_len = 26;
    // saved_plans[3].channel_num = 1,
    // saved_plans[3].used_times = 0;
    // saved_plans[3].is_bluetooth = false;
    // saved_plans[3].current_mA = 10;
    // saved_plans[3].total_time_min = 30;
    // saved_plans[3].pulse_width_us = 200;
    // saved_plans[3].freq_min = 20;
    // saved_plans[3].freq_max = 20;
    // saved_plans[3].freq_type = FREQ_TYPE_FIXED;
    // saved_plans[3].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    // saved_plans[3].wave_rise_ms = 0;
    // saved_plans[3].work_time_sec = 10;
    // saved_plans[3].wave_fall_ms = 0;
    // saved_plans[3].break_time_sec = 5;

    // saved_plans[4].id = 5;
    // strcpy(saved_plans[4].name, "中频-促进骨折愈合方案");
    // saved_plans[4].name_len = 32;
    // saved_plans[4].channel_num = 1,
    // saved_plans[4].used_times = 0;
    // saved_plans[4].is_bluetooth = false;
    // saved_plans[4].current_mA = 10;
    // saved_plans[4].total_time_min = 30;
    // saved_plans[4].pulse_width_us = 0;
    // saved_plans[4].freq_min = 100;
    // saved_plans[4].freq_max = 4000;
    // saved_plans[4].freq_type = FREQ_TYPE_VARY;
    // saved_plans[4].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    // saved_plans[4].wave_rise_ms = 0;
    // saved_plans[4].work_time_sec = 0;
    // saved_plans[4].wave_fall_ms = 0;
    // saved_plans[4].break_time_sec = 0;

    // saved_plans[5].id = 6;
    // strcpy(saved_plans[5].name, "中频-缓解疼痛方案");
    // saved_plans[5].name_len = 26;
    // saved_plans[5].channel_num = 1,
    // saved_plans[5].used_times = 0;
    // saved_plans[5].is_bluetooth = false;
    // saved_plans[5].current_mA = 10;
    // saved_plans[5].total_time_min = 20;
    // saved_plans[5].pulse_width_us = 0;
    // saved_plans[5].freq_min = 100;
    // saved_plans[5].freq_max = 5000;
    // saved_plans[5].freq_type = FREQ_TYPE_VARY;
    // saved_plans[5].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    // saved_plans[5].wave_rise_ms = 0;
    // saved_plans[5].work_time_sec = 0;
    // saved_plans[5].wave_fall_ms = 0;
    // saved_plans[5].break_time_sec = 0;

    // saved_plans[6].id = 7;
    // strcpy(saved_plans[6].name, "中频-OAB骶神经方案");
    // saved_plans[6].name_len = 26;
    // saved_plans[6].channel_num = 2,
    // saved_plans[6].used_times = 0;
    // saved_plans[6].is_bluetooth = false;
    // saved_plans[6].current_mA = 10;
    // saved_plans[6].total_time_min = 20;
    // saved_plans[6].pulse_width_us = 0;
    // saved_plans[6].freq_min = 20;
    // saved_plans[6].freq_max = 4000;
    // saved_plans[6].freq_type = FREQ_TYPE_VARY;
    // saved_plans[6].wave_type = WAVE_TYPE_SINE;
    // saved_plans[6].wave_rise_ms = 0;
    // saved_plans[6].work_time_sec = 0;
    // saved_plans[6].wave_fall_ms = 0;
    // saved_plans[6].break_time_sec = 0;
    saved_plans[0].id = 1,
    strcpy(saved_plans[0].name, "周围神经伤痛");
    saved_plans[0].name_len = 18,
    saved_plans[0].channel_num = 1,
    saved_plans[0].used_times = 0;
    saved_plans[0].is_bluetooth = false;
    saved_plans[0].current_mA = 10,
    saved_plans[0].total_time_min = 10,
    saved_plans[0].pulse_width_us = 0,
    saved_plans[0].scheme_type = SCHEME_TYPE_MID;
    saved_plans[0].freq_min = 100,
    saved_plans[0].freq_max = 4000,
    saved_plans[0].freq_type = FREQ_TYPE_FIXED,
    saved_plans[0].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE,
    saved_plans[0].wave_rise_ms = 0;
    saved_plans[0].work_time_sec = 0;
    saved_plans[0].wave_fall_ms = 0;
    saved_plans[0].break_time_sec = 0;

    saved_plans[1].id = 2;
    strcpy(saved_plans[1].name, "肩周炎");
    saved_plans[1].name_len = 6;
    saved_plans[1].channel_num = 1,
    saved_plans[1].used_times = 0;
    saved_plans[1].is_bluetooth = false;
    saved_plans[1].current_mA = 10;
    saved_plans[1].total_time_min = 10;
    saved_plans[1].pulse_width_us = 0;
    saved_plans[1].scheme_type = SCHEME_TYPE_MID;
    saved_plans[1].freq_min = 50;
    saved_plans[1].freq_max = 2000;
    saved_plans[1].freq_type = FREQ_TYPE_FIXED;
    saved_plans[1].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    saved_plans[1].wave_rise_ms = 0;
    saved_plans[1].work_time_sec = 0;
    saved_plans[1].wave_fall_ms = 0;
    saved_plans[1].break_time_sec = 0;

    saved_plans[2].id = 3;
    strcpy(saved_plans[2].name, "下肢血液循环");
    saved_plans[2].name_len = 18;
    saved_plans[2].channel_num = 1,
    saved_plans[2].used_times = 0;
    saved_plans[2].is_bluetooth = false;
    saved_plans[2].current_mA = 10;
    saved_plans[2].total_time_min = 10;
    saved_plans[2].pulse_width_us = 0;
    saved_plans[2].scheme_type = SCHEME_TYPE_MID;
    saved_plans[2].freq_min = 50;
    saved_plans[2].freq_max = 4000;
    saved_plans[2].freq_type = FREQ_TYPE_FIXED;
    saved_plans[2].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    saved_plans[2].wave_rise_ms = 0;
    saved_plans[2].work_time_sec = 0;
    saved_plans[2].wave_fall_ms = 0;
    saved_plans[2].break_time_sec = 0;

    saved_plans[3].id = 4;
    strcpy(saved_plans[3].name, "软化瘢痕");
    saved_plans[3].name_len = 12;
    saved_plans[3].channel_num = 1,
    saved_plans[3].used_times = 0;
    saved_plans[3].is_bluetooth = false;
    saved_plans[3].current_mA = 10;
    saved_plans[3].total_time_min = 10;
    saved_plans[3].pulse_width_us = 0;
    saved_plans[3].scheme_type = SCHEME_TYPE_MID;
    saved_plans[3].freq_min = 2;
    saved_plans[3].freq_max = 5000;
    saved_plans[3].freq_type = FREQ_TYPE_FIXED;
    saved_plans[3].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    saved_plans[3].wave_rise_ms = 0;
    saved_plans[3].work_time_sec = 0;
    saved_plans[3].wave_fall_ms = 0;
    saved_plans[3].break_time_sec = 0;

    saved_plans[4].id = 5;
    strcpy(saved_plans[4].name, "尿潴留");
    saved_plans[4].name_len = 9;
    saved_plans[4].channel_num = 1,
    saved_plans[4].used_times = 0;
    saved_plans[4].is_bluetooth = false;
    saved_plans[4].current_mA = 10;
    saved_plans[4].total_time_min = 10;
    saved_plans[4].pulse_width_us = 0;
    saved_plans[4].scheme_type = SCHEME_TYPE_MID;
    saved_plans[4].freq_min = 10;
    saved_plans[4].freq_max = 4000;
    saved_plans[4].freq_type = FREQ_TYPE_FIXED;
    saved_plans[4].wave_type = WAVE_TYPE_SINGLE_PHASE_SQUARE;
    saved_plans[4].wave_rise_ms = 0;
    saved_plans[4].work_time_sec = 0;
    saved_plans[4].wave_fall_ms = 0;
    saved_plans[4].break_time_sec = 0;

    saved_plans[5].id = 6;
    strcpy(saved_plans[5].name, "镇痛");
    saved_plans[5].name_len = 6;
    saved_plans[5].channel_num = 1,
    saved_plans[5].used_times = 0;
    saved_plans[5].is_bluetooth = false;
    saved_plans[5].current_mA = 10;
    saved_plans[5].total_time_min = 10;
    saved_plans[5].pulse_width_us = 0;
    saved_plans[5].scheme_type = SCHEME_TYPE_MID;
    saved_plans[5].freq_min = 100;
    saved_plans[5].freq_max = 4000;
    saved_plans[5].freq_type = FREQ_TYPE_FIXED;
    saved_plans[5].wave_type = WAVE_TYPE_SINE;
    saved_plans[5].wave_rise_ms = 0;
    saved_plans[5].work_time_sec = 0;
    saved_plans[5].wave_fall_ms = 0;
    saved_plans[5].break_time_sec = 0;

    saved_plans[6].id = 7;
    strcpy(saved_plans[6].name, "兴奋神经肌肉");
    saved_plans[6].name_len = 18;
    saved_plans[6].channel_num = 1,
    saved_plans[6].used_times = 0;
    saved_plans[6].is_bluetooth = false;
    saved_plans[6].current_mA = 10;
    saved_plans[6].total_time_min = 10;
    saved_plans[6].pulse_width_us = 300;
    saved_plans[6].scheme_type = SCHEME_TYPE_LOW;
    saved_plans[6].freq_min = 40;
    saved_plans[6].freq_max = 40;
    saved_plans[6].freq_type = FREQ_TYPE_FIXED;
    saved_plans[6].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    saved_plans[6].wave_rise_ms = 1000;
    saved_plans[6].work_time_sec = 3;
    saved_plans[6].wave_fall_ms = 1000;
    saved_plans[6].break_time_sec = 10;
    
}

void init_plans()
{
    // uint8_t scheme_count = nvs_get_schemeset_count();
    nvs_get_plans(saved_plans);
    ESP_LOGI("init_plans", "read plans");
    // print_plans(saved_plans);
    // for (uint8_t i = 0; i < MAX_PLAN_NUM; i++) {
    //     if (saved_plans[i].id == 0 || saved_plans[i].id > 100)
    //     {
    //         continue;
    //     }
    //     switch (saved_plans[i].scheme_type)
    //     {
    //         case SCHEME_TYPE_NMES:
    //             NMES.plans[NMES.plan_num++] = &saved_plans[i];
    //             break;
    //         case SCHEME_TYPE_TENS:
    //             TENS.plans[TENS.plan_num++] = &saved_plans[i];
    //             break;
    //         case SCHEME_TYPE_MNS:
    //             MNS.plans[MNS.plan_num++] = &saved_plans[i];
    //             break;
    //         case SCHEME_TYPE_TNS:
    //             TNS.plans[TNS.plan_num++] = &saved_plans[i];
    //             break;
    //         default:
    //             Others.plans[Others.plan_num++] = &saved_plans[i];
    //             break;
    //     }
    // }
}

void update_plan_position_by_used_times(Plan *pPlan)
{
    if (pPlan->id <= 1)
        return;
    uint8_t i = pPlan->id - 1;
    Plan temp = saved_plans[i];
    while (i > 0 && saved_plans[i-1].used_times <= saved_plans[i].used_times)
    {
        saved_plans[i] = saved_plans[i-1];
        saved_plans[i].id = i+1;
        i--;
    }
    saved_plans[i] = temp;
    saved_plans[i].id = i+1;
    nvs_save_plans(saved_plans);
    
}

void plan_to_data(Plan *plan, uint8_t channel_id, uint8_t *out_data)
{
    // out_data[0] = (plan->id & 0x0F) << 4 | ((1 << channel_id) & 0x0F);
    out_data[0] = (1) << 4 | ((1 << channel_id) & 0x0F);  // 方案编号全部设置为1
    out_data[1] = plan->wave_type;
    out_data[2] = plan->current_mA;
    out_data[3] = 0;
    out_data[4] = (plan->pulse_width_us & 0xFF00) >> 8;
    out_data[5] = plan->pulse_width_us & 0x00FF;
    out_data[6] = plan->pulse_interval;
    out_data[7] = plan->freq_type;
    out_data[8] = (plan->freq_min & 0xFF00) >> 8;
    out_data[9] = plan->freq_min & 0x00FF;
    out_data[10] = (plan->freq_max & 0xFF00) >> 8;
    out_data[11] = plan->freq_max & 0x00FF;
    out_data[12] = (plan->wave_rise_ms & 0xFF00) >> 8;
    out_data[13] = plan->wave_rise_ms & 0x00FF;
    out_data[14] = (plan->wave_fall_ms & 0xFF00) >> 8;
    out_data[15] = plan->wave_fall_ms & 0x00FF;
    out_data[16] = plan->work_time_sec;
    out_data[17] = 0;
    out_data[18] = plan->break_time_sec;
    out_data[19] = 0;
    out_data[20] = plan->total_time_min;
}
