#include "TCA9555.h"
//#include "CustomUI.h"
#include "FuelGauge.h"

void set_battery_level();
bool lvgl_lock(void);
void lvgl_unlock(void);
void load_charging_scr();
void exit_charging_scr();
void pause_all_channels();
void wakeup();

i2c_master_dev_handle_t TCA9555_dev_handle = NULL;
PIN_IO pin_io_old,pin_io;
StateWork statework;
static const char *TAG = "TCA9555";
extern bool is_hibernating;

static SemaphoreHandle_t tca9555_mutex = NULL;

void TCA9555_Dev_Init(i2c_master_bus_handle_t bus_handle)
{
    if (bus_handle == NULL)
    {
		ESP_LOGE("I2C", "Bus handle is NULL");
		return;
    }

    i2c_device_config_t TCA9555_config = {
       .dev_addr_length = I2C_ADDR_BIT_LEN_7,
       .device_address = TCA9555_I2C_ADDR,
       .scl_speed_hz = 400000,
    };
    PCA9546_SelectChannel(TCA9555_I2C_CHANNEL);
    if(ESP_OK == i2c_master_probe(bus_handle, TCA9555_I2C_ADDR, -1))
    {
        ESP_LOGI("I2C", "Device found at address 0x%02X", TCA9555_config.device_address);
    }
    PCA9546_DeselectChannel(TCA9555_I2C_CHANNEL);
    TCA9555_dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &TCA9555_config, &TCA9555_dev_handle));
    tca9555_mutex = xSemaphoreCreateMutex();
    if (tca9555_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
    }
}

void TCA9555_Init()
{
    if (i2c_bus1_handle == NULL)
    {
        I2C_Init();
    }

    TCA9555_Dev_Init(i2c_bus1_handle);

}

void TCA9555_WriteReg(uint8_t reg_addr, uint8_t data)
{
    xSemaphoreTake(tca9555_mutex, portMAX_DELAY);
    uint8_t write_data[2] = {reg_addr, data};
    PCA9546_SelectChannel(TCA9555_I2C_CHANNEL);
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit(TCA9555_dev_handle, write_data, 2, -1);
    xSemaphoreGive(i2c_bus1_mutex);
    PCA9546_DeselectChannel(TCA9555_I2C_CHANNEL);
    xSemaphoreGive(tca9555_mutex);
}

void TCA9555_ReadReg(uint8_t reg_addr, uint8_t *data)
{
    xSemaphoreTake(tca9555_mutex, portMAX_DELAY);
    PCA9546_SelectChannel(TCA9555_I2C_CHANNEL);
    xSemaphoreTake(i2c_bus1_mutex, portMAX_DELAY);
    i2c_master_transmit_receive(TCA9555_dev_handle, &reg_addr, 1, data, 1, -1);
    xSemaphoreGive(i2c_bus1_mutex);
    PCA9546_DeselectChannel(TCA9555_I2C_CHANNEL);
    xSemaphoreGive(tca9555_mutex);
}

void TCA9555_SetPinDir(uint8_t pin, TCA9555_PIN_MODE_T mode)
{
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    reg_data = pin;
    if(pin<8)
        reg_addr = TCA9555_REG_CONFIGURATION_PORT0;
    else if(pin>=8)
    {
        reg_addr = TCA9555_REG_CONFIGURATION_PORT1;
        pin-=8;
    }

    TCA9555_ReadReg(reg_addr, &reg_data);
    if (mode == TCA9555_INPUT)
        reg_data |= 1<<pin;
    else
        reg_data &= ~(1<<pin);
    TCA9555_WriteReg(reg_addr, reg_data);
    
}

void TCA9555_SetPinState(uint8_t pin, uint8_t state)
{
    TCA9555_SetPinDir(pin, TCA9555_OUTPUT);
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    if(pin<8)
        reg_addr = TCA9555_REG_OUTPUT_PORT0;
    else if(pin>=8)
        reg_addr = TCA9555_REG_OUTPUT_PORT1;

    TCA9555_ReadReg(reg_addr, &reg_data);
    if(reg_addr == TCA9555_REG_OUTPUT_PORT1)
        pin-=8;
    if (state)
        reg_data |= 1<<pin;
    else
        reg_data &= ~(1<<pin);
    TCA9555_WriteReg(reg_addr, reg_data);
}

bool TCA9555_GetPinState(uint8_t pin)
{
    
    uint8_t reg_addr = 0;
    uint8_t reg_data = 0;
    bool level;
    //TCA9555_SetPinDir(pin, TCA9555_INPUT);
    if(pin < 8)
        reg_addr = TCA9555_REG_INPUT_PORT0;
    else if(pin>=8)
    {
        reg_addr = TCA9555_REG_INPUT_PORT1;
        pin -=8;
    }
    
    TCA9555_ReadReg(reg_addr, &reg_data); 
    level=((reg_data >> pin)&0x01) ;
    return level;
}

///////////////////////////////////////
uint8_t check_pin(uint8_t pin)
{
    //bool level;
    if(((pin_io_old.pin_data>>pin)&0x01) == ((pin_io.pin_data>>pin)&0x01))
    {
        //XL9555_GetPinState(pin,&level);
        return 1;
    }
    else 
        return 0;
    // pin_io.pin_data =((pin_io.pin_data & (~(1<<pin))) | (level<<pin));
    // if(((pin_io_old.pin_data>>pin)&0x01) == ((pin_io.pin_data>>pin)&0x01))
        // return 0;
    
    
}
uint8_t read_pin(uint8_t pin)
{
    bool level;
    
    pin_io_old.pin_data = (pin_io_old.pin_data & (~(1<<pin))) | (pin_io.pin_data & (1<<pin));
    level = TCA9555_GetPinState(pin);
    pin_io.pin_data =((pin_io.pin_data & (~(1<<pin))) | (level<<pin));
    return 1;
}
void tca9555_task(void *arg)
{
    uint8_t data[2];
    //  gpio_set_pull_mode(CTRL_I2C_START_NUM,GPIO_FLOATING);
    //  gpio_set_direction(CTRL_I2C_START_NUM, GPIO_MODE_OUTPUT);
    //   gpio_set_level(CTRL_I2C_START_NUM,0);
    //   vTaskDelay(pdMS_TO_TICKS(1000));
    //   gpio_set_level(CTRL_I2C_START_NUM,1);

    TCA9555_ReadReg(TCA9555_REG_INPUT_PORT0, data);
    TCA9555_ReadReg(TCA9555_REG_INPUT_PORT1, data+1);
    pin_io_old.pin_data = data[0] | (data[1]<<8);
    pin_io.pin_data = data[0] | (data[1]<<8);


    pin_io_old.pin_bit.pin_chr_sta_ind = pin_io_old.pin_bit.pin_chr_sta_ind ^ 1;
    ESP_LOGI(TAG, "TCA9555_PIN_CHR_STA_IND  data0 %02x data1  %02x pin_io %04x",data[0],data[1],pin_io.pin_data);
    //MUX_choose(MUX_TCA9555);
    TCA9555_SetPinState(TCA9555_PIN_5V_PWR,1);
    statework.st_data = 0;
    statework.st_bit.StOn = 1;
    for(;;)
    { 
        checkIO();
        ledTask(statework);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

}


void checkIO(void)
{
    if(check_pin(TCA9555_PIN_CHR_STA_IND))
    {
        read_pin(TCA9555_PIN_CHR_STA_IND);
    }
    else
    {//外部脚输入有变化
        //pin_io.pin_bit.pin_chr_sta_ind
        ESP_LOGI(TAG, "XL9555_PIN_CHR_STA_IND  %d ",pin_io.pin_bit.pin_chr_sta_ind);
        pin_io_old.pin_bit.pin_chr_sta_ind = pin_io.pin_bit.pin_chr_sta_ind;
        if(pin_io.pin_bit.pin_chr_sta_ind == 0)
        {
            statework.st_bit.StChrg = 1;
            ble_start_adv();
            if (lvgl_lock())
            {
                pause_all_channels();
                load_charging_scr();
                lvgl_unlock();
            }
        }
        else
        {
            statework.st_bit.StChrg = 0;
            ble_disconnect();
            ble_stop_adv();
            if (lvgl_lock())
            {
                exit_charging_scr();
                lvgl_unlock();
            }
            
        }
    }
    if(check_pin(TCA9555_PIN_BL_ADJ))
    {
        read_pin(TCA9555_PIN_BL_ADJ);
    }
    else
    {
        //外部脚输入有变化
        //pin_io.pin_bit.pin_chr_sta_ind
        bool level;
        if(pin_io.pin_bit.pin_bl_adj)
        {
            if (is_hibernating)
            {
                wakeup();
            }
            
            level = TCA9555_GetPinState(TCA9555_PIN_BL_EN);
            //ESP_LOGI(TAG, "level  %d ",level);
            level = level? 0:1;
            //ESP_LOGI(TAG, "level  %d ",level);
            TCA9555_SetPinState(TCA9555_PIN_BL_EN,level);
            pin_io_old.pin_bit.pin_bl_en = pin_io.pin_bit.pin_bl_en;
            pin_io.pin_bit.pin_bl_en = level;
          
        }
        ESP_LOGI(TAG, "TCA9555_PIN_BL_ADJ  %d ",pin_io.pin_bit.pin_bl_adj);
        ESP_LOGI(TAG, "TCA9555_PIN_BL_EN  %d ",pin_io.pin_bit.pin_bl_en);
        pin_io_old.pin_bit.pin_bl_adj = pin_io.pin_bit.pin_bl_adj;
    }


    // static bool is_charging = false;
    // bool current_charging_state = !TCA9555_GetPinState(TCA9555_PIN_TP_RST);
    // if (is_charging != current_charging_state && current_charging_state == 1)
    // {
        // is_charging = true;
    //    load_charging_scr();
        // TCA9555_SetPinState(TCA9555_PIN_YELLOW, 0);
    // }
    // else if (is_charging != current_charging_state && current_charging_state == 0)
    // {
        // is_charging = false;
    //    exit_charging_scr();
        // TCA9555_SetPinState(TCA9555_PIN_YELLOW, 1);
    // }


    //电池电量
    static uint16_t count1 = 0;
    if(count1++>30)
    {
        count1=0;
        // uint8_t buf[2] = {0x2C, 0x2D};
        // FuelGauge_ReadReg(buf, buf);
        // ESP_LOGI(TAG, "Fuel Gauge: %02X %02X", buf[0], buf[1]); 
        // if (buf[1] != 0)
        // {
        //     return;
        // }
        FuelGauge_Get_SOC();
        
        if(lvgl_lock())
        {
            // set_battery_level(buf[0]);
            set_battery_level();
            lvgl_unlock();
        }
        if(g_battery_soc > 90)
            statework.st_bit.StChrgOK = 1;
        else if(g_battery_soc < 20)
            statework.st_bit.StPowerLow = 1;
        else
        {
            statework.st_bit.StChrgOK = 0;
            statework.st_bit.StPowerLow = 0;

        }

    }

    
}



void ledTask(StateWork statew)
{
    if(statew.st_bit.StRun)
        LedCtrl(LedYellowFlashSlow,0,0);
    else if(statew.st_bit.StChrgOK)
        LedCtrl(LedGreenOn,0,0);
    else if(statew.st_bit.StChrg)
        LedCtrl(LedGreenFlashSlow,0,0);
    else if(statew.st_bit.StPowerLow)
        LedCtrl(LedYellowFlashFast,0,0);
    
    
    
    
    else if(statew.st_bit.StOn)
        LedCtrl(LedGreenOn,0,0);
    else 
        LedCtrl(LedGreenOn,0,0);

}



void LedCtrl(uint16_t uwLedType,uint16_t uwBLinkTime,uint16_t uwExeCycle)
{
	static uint16_t uwBLinkCnt[6] = {0,0,0,0,0,0};
	static uint16_t uwBLinkCnt1[6] = {0,0,0,0,0,0};
	static uint16_t uwBLinkPeriod[6] = {20,50,20,20,50,20};//20*20ms=400ms
	static uint16_t uwTotalBLinkPeriod[6] = {0,0,0,0,0,0};
	//uwBLinkPeriod[uwLedType] =  5 * uwExeCycle;//20ms  //20
    if(++uwBLinkCnt1[uwLedType] <= (uwBLinkPeriod[uwLedType]>>1))
    {
        sLedStatusSet(uwLedType,cLedON);
    }
    else if(uwBLinkCnt1[uwLedType] <= uwBLinkPeriod[uwLedType])
    {
        sLedStatusSet(uwLedType,cLedOFF);
    }
    else
    {
        uwBLinkCnt1[uwLedType] = 0;
    }
}

void sLedStatusSet(uint16_t wLedType,uint16_t wLedStatus)
{
	if (wLedStatus == cLedON)
	{
		if (wLedType == LedGreenOn)  
		{
			TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
		}
		else if(wLedType == LedGreenFlashSlow)
		{
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
        }
        else if(wLedType == LedGreenFlashFast)
        {          
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
        }
        else if(wLedType == LedYellowOn)
        {
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
        if (wLedType == LedYellowFlashSlow)
        {
        	TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
        else if(wLedType == LedYellowFlashFast)
        {          
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
	}
	else if(wLedStatus == cLedOFF)
	{
		if (wLedType == LedGreenOn)  
        {
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
        }
        else if(wLedType == LedGreenFlashSlow)
        {
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
        }
        else if(wLedType == LedGreenFlashFast)
        {
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
        }
        else if(wLedType == LedYellowOn)
        {
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedON);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
        else if(wLedType == LedYellowFlashSlow)
        {
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
        else if(wLedType == LedYellowFlashFast)
        {
            TCA9555_SetPinState(TCA9555_PIN_YELLOW,cLedOFF);
            TCA9555_SetPinState(TCA9555_PIN_GREEN,cLedOFF);
        }
       
    }
}