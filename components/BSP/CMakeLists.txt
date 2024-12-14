set(src_dirs 
            ADC
            rgb_lcd
            I2C
            GT911
            # TCA9554
            FuelGauge
            UART
            Bluetooth
            ST_IMU
            Protocal
            DataStructures
            NVS_op
            Timer
            TCA9555
            PCA9546
            )

set(include_dirs
            ADC
            rgb_lcd
            I2C
            GT911
            # TCA9554
            FuelGauge
            UART
            Bluetooth
            ST_IMU
            Protocal
            DataStructures
            NVS_op
            Timer
            TCA9555
            PCA9546
            )

set(requires
            driver
            esp_common
            esp_lcd
            esp_adc
            log
            esp_driver_gpio
            esp_driver_uart
            esp_lcd_panel_io_additions
            esp_lcd_st7701
            bt
            nvs_flash
            )

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})