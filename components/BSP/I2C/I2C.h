#ifndef I2C_H_
#define I2C_H_

#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#define I2C_PORT_0 I2C_NUM_0
#define I2C_PORT_1 I2C_NUM_1

#define I2C_MASTER0_SCL_IO 7
#define I2C_MASTER0_SDA_IO 15

#define I2C_MASTER1_SCL_IO 2
#define I2C_MASTER1_SDA_IO 1

extern i2c_master_bus_handle_t i2c_bus0_handle;
extern i2c_master_bus_handle_t i2c_bus1_handle;


void I2C_Init();

#endif /* I2C_H_ */