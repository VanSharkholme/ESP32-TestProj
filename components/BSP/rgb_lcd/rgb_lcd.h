#ifndef RGB_LCD_H
#define RGB_LCD_H

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_lcd_st7701.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uart_ens.h"


#define EXAMPLE_LCD_IO_SPI_CS           (GPIO_NUM_42)
#define EXAMPLE_LCD_IO_SPI_SCL          (GPIO_NUM_2)
#define EXAMPLE_LCD_IO_SPI_SDA          (GPIO_NUM_1)

#define PIN_NUM_BK_LIGHT       -1
#define PIN_NUM_HSYNC          38
#define PIN_NUM_VSYNC          39
#define PIN_NUM_DE             40
#define PIN_NUM_PCLK           41
#define PIN_NUM_DATA0          (GPIO_NUM_46)    // B0
#define PIN_NUM_DATA1          (GPIO_NUM_3)     // B1
#define PIN_NUM_DATA2          (GPIO_NUM_8)     // B2
#define PIN_NUM_DATA3          (GPIO_NUM_18)    // B3
#define PIN_NUM_DATA4          (GPIO_NUM_17)    // B4
#define PIN_NUM_DATA5          (GPIO_NUM_14)    // G0
#define PIN_NUM_DATA6          (GPIO_NUM_13)    // G1
#define PIN_NUM_DATA7          (GPIO_NUM_12)    // G2
#define PIN_NUM_DATA8          (GPIO_NUM_11)    // G3
#define PIN_NUM_DATA9          (GPIO_NUM_10)    // G4
#define PIN_NUM_DATA10         (GPIO_NUM_9)     // G5
#define PIN_NUM_DATA11         (GPIO_NUM_5)     // R0
#define PIN_NUM_DATA12         (GPIO_NUM_45)    // R1
#define PIN_NUM_DATA13         (GPIO_NUM_48)    // R2
#define PIN_NUM_DATA14         (GPIO_NUM_47)    // R3
#define PIN_NUM_DATA15         (GPIO_NUM_21)    // R4
#define PIN_NUM_DISP_EN        -1
#define RESOLUTION_X           480
#define RESOLUTION_Y           854

extern esp_lcd_panel_handle_t panel_handle;

void rgb_lcd_init();
void rgb_lcd_brightness(uint8_t brightness);
void rgb_lcd_restart(void);
void lcd_clear(uint16_t color);

#endif // !RGB_LCD_H
