#include "I2C.h"

i2c_master_bus_handle_t i2c_bus0_handle;
i2c_master_bus_handle_t i2c_bus1_handle;


void I2C_Init(void)
{
    i2c_master_bus_config_t i2c_mst0_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_0,
        .scl_io_num = I2C_MASTER0_SCL_IO,
        .sda_io_num = I2C_MASTER0_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst0_config, &i2c_bus0_handle));
    i2c_master_bus_config_t i2c_mst1_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT_1,
        .scl_io_num = I2C_MASTER1_SCL_IO,
        .sda_io_num = I2C_MASTER1_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst1_config, &i2c_bus1_handle));


}