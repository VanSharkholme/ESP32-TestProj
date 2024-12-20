#include "nvs_op.h"

void nvs_op_init()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
}

void nvs_op_end(nvs_handle handle)
{
    nvs_close(handle);
    rgb_lcd_restart();
}

void nvs_test()
{
    nvs_op_init();
    nvs_op_init();
    nvs_handle handle;
    esp_err_t err = nvs_open("test", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("nvs_open failed with error %d\n", err);
        return;
    }
    uint8_t my_data = 0;
    err = nvs_get_u8(handle, "my_key", &my_data);
    my_data++;
    printf("my_data = %d\n", my_data);
    nvs_set_u8(handle, "my_key", my_data);
    nvs_commit(handle);
    nvs_op_end(handle);
}

void nvs_save_bluetooth_name(char *name)
{
    nvs_op_init();
    nvs_handle handle;
    esp_err_t err = nvs_open("bluetooth", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("nvs_open failed with error %d\n", err);
        nvs_op_end(handle);
        return;
    }
    nvs_set_str(handle, "name", name);
    nvs_commit(handle);
    nvs_op_end(handle);
}

char *nvs_get_bluetooth_name()
{
    nvs_op_init();
    nvs_handle handle;
    esp_err_t err = nvs_open("bluetooth", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("nvs_open failed with error %d\n", err);
        nvs_op_end(handle);
        return NULL;
    }
    size_t len = 0;
    err = nvs_get_str(handle, "name", NULL, &len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // printf("nvs_get_str failed with error %s\n", esp_err_to_name(err));
        nvs_set_str(handle, "name", "ESP32_TestProj");
        nvs_commit(handle);
        nvs_op_end(handle);
        return "ESP32_TestProj";
    }
    else if(err != ESP_OK)
    {
        printf("nvs_get_str failed with error %s\n", esp_err_to_name(err));
        nvs_op_end(handle);
        return NULL;
    }
    else {
        char *name = malloc(len+1);
        err = nvs_get_str(handle, "name", name, &len);
        if (err != ESP_OK) {
            printf("nvs_get_str failed with error %d\n", err);
            nvs_op_end(handle);
            return NULL;
        }
        name[len] = '\0';
        nvs_op_end(handle);
        return name;
    }
}

void nvs_save_plans(Plan *plans)
{
    nvs_op_init();
    nvs_handle handle;
    esp_err_t err = nvs_open("plans", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("nvs_open failed with error %d\n", err);
        nvs_op_end(handle);
        return;
    }
    nvs_set_blob(handle, "plans", (void*)plans, sizeof(Plan)*MAX_PLAN_NUM);
    nvs_commit(handle);
    nvs_op_end(handle);
    ESP_LOGI("nvs_op", "nvs save plans success");
}

void nvs_get_plans(Plan *plans)
{
    nvs_op_init();
    nvs_handle handle;
    esp_err_t err = nvs_open("plans", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("nvs_open failed with error %d\n", err);
        nvs_op_end(handle);
    }
    ESP_LOGI("nvs_op", "nvs get plans opened");
    uint16_t len = sizeof(Plan)*MAX_PLAN_NUM;
    err = nvs_get_blob(handle, "plans", (void*)plans, &len);
    ESP_LOGI("nvs_op", "nvs get plans get_blob");
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        reset_schemeset();
        nvs_set_blob(handle, "plans", saved_plans, sizeof(Plan)*MAX_PLAN_NUM);
        nvs_commit(handle);
    }
    else if (err != ESP_OK) {
        ESP_LOGE("nvs_op", "nvs_get_blob failed with error %s\n", esp_err_to_name(err));
        nvs_op_end(handle);
    }
    nvs_op_end(handle);
    ESP_LOGI("nvs_op", "nvs get plans success");
}
