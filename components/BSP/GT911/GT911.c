#include "GT911.h"

uint32_t GT911_CFG_TBL[186] = {
    0x41,0x20,0x03,0xE0,0x01, 0x0A,0x0F,0x20,0x01,0x0A, // 0x8050
    0x28,0x0F,0x5A,0x3C,0x03, 0x05,0x00,0x00,0x00,0x00,	// 0x805A
    0x00,0x00,0x05,0x00,0x00, 0x00,0x00,0x8A,0x2A,0x0C, // 0x8064
    0x32,0x34,0x0C,0x08,0x00, 0x00,0x00,0x03,0x02,0x25, // 0x806E
    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x8078

    0x00,0x28,0x55,0x94,0xC5, 0x02,0x07,0x00,0x00,0x04,	// 0x8082
    0x8D,0x2B,0x00,0x80,0x32, 0x00,0x75,0x3A,0x00,0x6C,	// 0x808C
    0x43,0x00,0x64,0x4F,0x00, 0x64,0x00,0x00,0x00,0x00,	// 0x8096
    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x80A0
    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x80AA

    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x80B4
    0x00,0x00,0x18,0x16,0x14, 0x12,0x10,0x0E,0x0C,0x0A,	// 0x80BE
    0x08,0x06,0x04,0x02,0xFF, 0xFF,0x00,0x00,0x00,0x00,	// 0x80C8
    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x80D2
    0x00,0x00,0x24,0x22,0x21, 0x20,0x1F,0x1E,0x1D,0x1C,	// 0x80DC
    
    0x18,0x16,0x13,0x12,0x10, 0x0F,0x0A,0x08,0x06,0x04,	// 0x80E6
    0x02,0x00,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0x00,0x00,	// 0x80F0
    0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,	// 0x80FA
    0x00,0x00,0x00,0x00,0x58, 0x01
};

i2c_master_dev_handle_t gt911_dev_handle;

void delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void GT_GPIOInit()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << GT_INT_PIN;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void GT_INT_Init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << GT_INT_PIN;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void GT_DEV_Init(i2c_master_bus_handle_t bus_handle)
{
	if (bus_handle == NULL)
	{
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
	}
	
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x5D,
        .scl_speed_hz = 400000,
    };

	GT_GPIOInit();
	delay_ms(5);
	GT_RES_Clr();
	delay_ms(10); 
    GT_RES_Set();
	delay_ms(10);
	GT_RES_Clr();
	GT_INT_Clr();
	delay_ms(20);
	if(GT_Read_ADDR==0x28)
	{
		GT_INT_Set();
	}
	else
	{
		GT_INT_Clr();
	}
	delay_ms(20);
	GT_RES_Set();
	delay_ms(20);
	GT_INT_Set();
    delay_ms(10); //退出sleep mode
    GT_INT_Init();



    if(ESP_OK == i2c_master_probe(bus_handle, 0x5D, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", dev_cfg.device_address);
    }

	gt911_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &gt911_dev_handle));
    uint8_t id_reg_addr[2] = {0x81, 0x40};
    uint8_t id_reg_data[4] = {0};
    i2c_master_transmit_receive(gt911_dev_handle, id_reg_addr, 2, id_reg_data, 4, -1);
	if (id_reg_data[0] != '9' || id_reg_data[1] != '1' || id_reg_data[2] != '1' || id_reg_data[3] != 0)
	{
		ESP_LOGE("I2C", "GT911 not found");
		gt911_dev_handle = NULL;
		return;
	}
	
    ESP_LOGI("I2C", "ID register data: GT%c%c%c%c", id_reg_data[0], id_reg_data[1], id_reg_data[2], id_reg_data[3]);
}

esp_err_t GT_WR_DATA(uint8_t addr[2],uint8_t data[], uint8_t len)
{
	uint8_t tx_buf[512] = {0};
	tx_buf[0] = addr[0];
	tx_buf[1] = addr[1];
	for(int i = 0; i < len; i++)
	{
		tx_buf[i+2] = data[i];
	}
	return i2c_master_transmit(gt911_dev_handle, tx_buf, len+2, -1);
}

esp_err_t GT_RD_DATA(uint8_t addr[2],uint8_t len,uint8_t value[])
{
	return i2c_master_transmit_receive(gt911_dev_handle, addr, 2, value, len, -1);
}

void GT_Init()
{
	if (i2c_bus0_handle == NULL)
	{
		I2C_Init();
	}
	
    GT_DEV_Init(i2c_bus0_handle);
	if (gt911_dev_handle == NULL)
	{
		ESP_LOGE("I2C", "GT911 initialization failed");
		return;
	}

}

bool GT911_Read_Coordinate(int16_t *x, int16_t *y)
{
	uint8_t point_reg_addr[2] = {0x81, 0x4E};
	uint8_t coord_reg_addr[2] = {0x81, 0x50};
	uint8_t coord_buf[4] = {0};
    uint8_t instruction = 0;
    GT_RD_DATA(point_reg_addr, 1, coord_buf);
    if (coord_buf[0] & 0x80)
    {
        coord_buf[0] = 0;
        GT_RD_DATA(coord_reg_addr, 4, coord_buf);
        *x = (int32_t) (coord_buf[0] | (coord_buf[1] << 8));
        *y = (int32_t) (coord_buf[2] | (coord_buf[3] << 8));
        GT_WR_DATA(point_reg_addr, &instruction, 1);
		#ifdef GT911_COORD_LOG
			ESP_LOGI("GT911", "X: %d, Y: %d", *x, *y);
		#endif
        return true;
    } else
    {
        GT_WR_DATA(point_reg_addr, &instruction, 1);
		#ifdef GT911_COORD_LOG
			ESP_LOGI("GT911", "No touch detected");
		#endif
        return false;
    }

}
