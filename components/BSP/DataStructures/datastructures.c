#include "datastructures.h"
#include "string.h"

Plan saved_plans[MAX_PLAN_NUM] = {0};

SchemeSet NMES = {
    .name = "神经肌肉电刺激",
    .plan_num = 0,
    .plans = {0}
};

SchemeSet TENS = {
    .name = "经皮神经电刺激",
    .plan_num = 0,
    .plans = {0}
};

SchemeSet MNS = {
    .name = "正中神经电刺激",
    .plan_num = 0,
    .plans = {0}
};

SchemeSet TNS = {
    .name = "胫神经电刺激",
    .plan_num = 0,
    .plans = {0}
};

SchemeSet Others = {
    .name = "其他",
    .plan_num = 0,
    .plans = {0}
};

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

SchemeSet test_schemeset = {
        .name = "神经电刺激",
        .plan_num = 5,
        .plans = {
            &test_plans[0],
            &test_plans[1],
            &test_plans[2],
            &test_plans[3],
            &test_plans[4]
        }
};

void print_plans(Plan *plans)
{
    for (uint8_t i = 0; i < MAX_PLAN_NUM; i++) {
        printf("===========plan %d:\n", i);
        printf("id: %d\n", plans[i].id);
        printf("scheme_type: %d\n", plans[i].scheme_type);
        printf("name_len: %d\n", plans[i].name_len);
        printf("name: %s\n", plans[i].name);
        printf("wave_type: %d\n", plans[i].wave_type);
        printf("current_mA: %d\n", plans[i].current_mA);
        printf("pulse_width: %d\n", plans[i].pulse_width);
        printf("pulse_interval: %d\n", plans[i].pulse_interval);
        printf("freq_type: %d\n", plans[i].freq_type);
        printf("freq_min: %d\n", plans[i].freq_min);
        printf("freq_max: %d\n", plans[i].freq_max);
        printf("wave_rise: %d\n", plans[i].wave_rise);
        printf("work_time: %d\n", plans[i].work_time);
        printf("wave_fall: %d\n", plans[i].wave_fall);
        printf("work_time: %d\n", plans[i].work_time);
        printf("break_time: %d\n", plans[i].break_time);
        printf("total_time_min: %d\n", plans[i].total_time_min);
    }
}


void reset_schemeset()
{
    Plan plans[MAX_PLAN_NUM] = {0};
    plans[0].id = 1,
    strcpy(plans[0].name, "脑卒中后肩痛");
    plans[0].name_len = 19,
    plans[0].current_mA = 10,
    plans[0].total_time_min = 30,
    plans[0].scheme_type = SCHEME_TYPE_TENS,
    plans[0].pulse_width = 200,
    plans[0].freq_min = 100,
    plans[0].freq_max = 100,
    plans[0].freq_type = FREQ_TYPE_FIXED,
    plans[0].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE,

    plans[1].id = 2;
    strcpy(plans[1].name, "带状疱疹神经痛");
    plans[1].name_len = 22;
    plans[1].current_mA = 10;
    plans[1].total_time_min = 30;
    plans[1].scheme_type = SCHEME_TYPE_TENS;
    plans[1].pulse_width = 250;
    plans[1].freq_min = 80;
    plans[1].freq_max = 80;
    plans[1].freq_type = FREQ_TYPE_FIXED;
    plans[1].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;

    plans[2].id = 3;
    strcpy(plans[2].name, "脑卒中上肢功能障碍");
    plans[2].name_len = 28;
    plans[2].current_mA = 10;
    plans[2].total_time_min = 30;
    plans[2].scheme_type = SCHEME_TYPE_NMES;
    plans[2].pulse_width = 200;
    plans[2].freq_min = 35;
    plans[2].freq_max = 35;
    plans[2].freq_type = FREQ_TYPE_FIXED;
    plans[2].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    plans[2].wave_rise = 0;
    plans[2].work_time = 5;
    plans[2].wave_fall = 0;
    plans[2].break_time = 20;

    plans[3].id = 4;
    strcpy(plans[3].name, "脑梗死吞咽训练");
    plans[3].name_len = 22;
    plans[3].current_mA = 10;
    plans[3].total_time_min = 20;
    plans[3].scheme_type = SCHEME_TYPE_NMES;
    plans[3].pulse_width = 300;
    plans[3].freq_min = 40;
    plans[3].freq_max = 40;
    plans[3].freq_type = FREQ_TYPE_FIXED;
    plans[3].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;

    plans[4].id = 5;
    strcpy(plans[4].name, "急迫性尿失禁");
    plans[4].name_len = 19;
    plans[4].current_mA = 10;
    plans[4].total_time_min = 20;
    plans[4].scheme_type = SCHEME_TYPE_TENS;
    plans[4].pulse_width = 300;
    plans[4].freq_min = 10;
    plans[4].freq_max = 10;
    plans[4].freq_type = FREQ_TYPE_FIXED;
    plans[4].wave_type = WAVE_TYPE_DUAL_PHASE_SQUARE;
    plans[4].wave_rise = 0;
    plans[4].work_time = 10;
    plans[4].wave_fall = 0;
    plans[4].break_time = 15;

    // print_plans(plans);
    nvs_save_plans(plans);

}

void init_plans()
{
    // uint8_t scheme_count = nvs_get_schemeset_count();
    nvs_get_plans(saved_plans);
    ESP_LOGI("init_plans", "read plans");
    // print_plans(saved_plans);
    for (uint8_t i = 0; i < MAX_PLAN_NUM; i++) {
        if (saved_plans[i].id == 0 || saved_plans[i].id > 15)
        {
            continue;
        }
        switch (saved_plans[i].scheme_type)
        {
            case SCHEME_TYPE_NMES:
                NMES.plans[NMES.plan_num++] = &saved_plans[i];
                break;
            case SCHEME_TYPE_TENS:
                TENS.plans[TENS.plan_num++] = &saved_plans[i];
                break;
            case SCHEME_TYPE_MNS:
                MNS.plans[MNS.plan_num++] = &saved_plans[i];
                break;
            case SCHEME_TYPE_TNS:
                TNS.plans[TNS.plan_num++] = &saved_plans[i];
                break;
            default:
                Others.plans[Others.plan_num++] = &saved_plans[i];
                break;
        }
    }
}

void plan_to_data(Plan *plan, uint8_t channel_id, uint8_t *out_data)
{
    out_data[0] = (plan->id & 0x0F) << 4 | (channel_id & 0x0F);
    out_data[1] = plan->wave_type;
    out_data[2] = plan->current_mA;
    out_data[3] = 0;
    out_data[4] = plan->pulse_width & 0xFF00 >> 8;
    out_data[5] = plan->pulse_width & 0x00FF;
    out_data[6] = plan->pulse_interval;
    out_data[7] = plan->freq_type;
    out_data[8] = plan->freq_min & 0xFF00 >> 8;
    out_data[9] = plan->freq_min & 0x00FF;
    out_data[10] = plan->freq_max & 0xFF00 >> 8;
    out_data[11] = plan->freq_max & 0x00FF;
    out_data[12] = plan->wave_rise & 0xFF00 >> 8;
    out_data[13] = plan->wave_rise & 0x00FF;
    out_data[14] = plan->wave_fall & 0xFF00 >> 8;
    out_data[15] = plan->wave_fall & 0x00FF;
    out_data[16] = plan->work_time;
    out_data[17] = 0;
    out_data[18] = plan->break_time;
    out_data[19] = 0;
    out_data[20] = plan->total_time_min;
}
