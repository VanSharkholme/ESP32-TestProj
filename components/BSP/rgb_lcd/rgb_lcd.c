#include "rgb_lcd.h"

static const char* TAG = "RGB_LCD";
esp_lcd_panel_handle_t panel_handle = NULL;
static portMUX_TYPE lcd_spinlock = portMUX_INITIALIZER_UNLOCKED;

static const st7701_lcd_init_cmd_t lcd_init_cmds[] = {
//  {cmd, { data }, data_size, delay_ms}
//厂家第二版本
{0xFF,(uint8_t[]) {0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
{0xEF,(uint8_t[]) {0x08}, 1, 0},
{0xFF,(uint8_t[]) {0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
{0xC0,(uint8_t[]) {0xE9, 0x03}, 2, 0},
{0xC1,(uint8_t[]) {0x0A, 0x02}, 2, 0},
{0xC2,(uint8_t[]) {0x01, 0x06}, 2, 0},
{0xC7,(uint8_t[]) {0x00}, 1, 0},
{0xCC,(uint8_t[]) {0x18}, 1, 0},
{0xB0,(uint8_t[]) {0x00, 0x0B, 0x10, 0x0D, 0x11, 0x06, 0x01, 0x08, 0x08, 0x1D, 0x04, 0x10, 0x10, 0x27, 0x30, 0x19}, 16, 0},
{0xB1,(uint8_t[]) {0x00, 0x0B, 0x14, 0x0C, 0x11, 0x05, 0x03, 0x08, 0x08, 0x20, 0x04, 0x13, 0x10, 0x28, 0x30, 0x19}, 16, 0},
{0xFF,(uint8_t[]) {0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
{0xB0,(uint8_t[]) {0x35}, 1, 0},
{0xB1,(uint8_t[]) {0x31}, 1, 0},
{0xB2,(uint8_t[]) {0x87}, 1, 0},
{0xB3,(uint8_t[]) {0x80}, 1, 0},
{0xB5,(uint8_t[]) {0x4E}, 1, 0},
{0xB7,(uint8_t[]) {0x85}, 1, 0},
{0xB8,(uint8_t[]) {0x10}, 1, 0},
{0xB9,(uint8_t[]) {0x10}, 1, 0},
{0xBC,(uint8_t[]) {0x03}, 1, 0},
{0xC0,(uint8_t[]) {0x89}, 1, 0},
{0xC1,(uint8_t[]) {0x78}, 1, 0},
{0xC2,(uint8_t[]) {0x78}, 1, 0},
{0xD0,(uint8_t[]) {0x88}, 1, 0},
{0xE0,(uint8_t[]) {0x00, 0x00, 0x02}, 3, 0},
{0xE1,(uint8_t[]) {0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20}, 11, 0},
{0xE2,(uint8_t[]) {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 13, 0},
{0xE3,(uint8_t[]) {0x00, 0x00, 0x33, 0x00}, 4, 0},
{0xE4,(uint8_t[]) {0x22, 0x00}, 2, 0},
{0xE5,(uint8_t[]) {0x04, 0x5C, 0xA0, 0xA0, 0x06, 0x5C, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 16, 0},
{0xE6,(uint8_t[]) {0x00, 0x00, 0x33, 0x00}, 4, 0},
{0xE7,(uint8_t[]) {0x22, 0x00}, 2, 0},
{0xE8,(uint8_t[]) {0x05, 0x5C, 0xA0, 0xA0, 0x07, 0x5C, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 16, 0},
{0xEB,(uint8_t[]) {0x02, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00}, 7, 0},
{0xEC,(uint8_t[]) {0x00, 0x00}, 2, 0},
{0xED,(uint8_t[]) {0xAA, 0x45, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0xB0, 0x54, 0xAA}, 16, 0},
{0xEF,(uint8_t[]) {0x08, 0x08, 0x08, 0x45, 0x3F, 0x54}, 6, 0},
{0xFF,(uint8_t[]) {0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
{0xE8,(uint8_t[]) {0x00, 0x0E}, 2, 0},
{0x11, NULL, 0, 120},
{0xFF,(uint8_t[]) {0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
{0xE8,(uint8_t[]) {0x00, 0x0C}, 2, 10},
{0xE8,(uint8_t[]) {0x00, 0x00}, 2, 0},
{0xE6,(uint8_t[]) {0x16, 0x7C}, 2, 0},
{0x36,(uint8_t[]) {0x08}, 1, 0},
{0x3A,(uint8_t[]) {0x55}, 1, 0},
{0x29, NULL, 0, 20}
};

void rgb_lcd_init()
{
    ESP_LOGI(TAG, "Install 3-wire SPI panel IO");
    spi_line_config_t line_config = {
        .cs_io_type = IO_TYPE_GPIO,
        .cs_gpio_num = EXAMPLE_LCD_IO_SPI_CS,
        .scl_io_type = IO_TYPE_GPIO,
        .scl_gpio_num = EXAMPLE_LCD_IO_SPI_SCL,
        .sda_io_type = IO_TYPE_GPIO,
        .sda_gpio_num = EXAMPLE_LCD_IO_SPI_SDA,
        .io_expander = NULL,
    };

    esp_lcd_panel_io_3wire_spi_config_t io_config = {
        .line_config = line_config,
        .expect_clk_speed = 500000,
        .spi_mode = 0,
        .lcd_cmd_bytes = 1,
        .lcd_param_bytes = 1,
        .flags = {
            .use_dc_bit = 1,
            .dc_zero_on_data = 0,
            .lsb_first = 0,
            .cs_high_active = 0,
            .del_keep_cs_inactive = 1,
        },
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_3wire_spi(&io_config, &io_handle));


    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = 16 * 1000 * 1000,
            .h_res = 480,
            .v_res = 854,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 10,
            .hsync_front_porch = 20,
            .vsync_pulse_width = 10,
            .vsync_back_porch = 10,
            .vsync_front_porch = 10,
            .flags = {
                .hsync_idle_low = 0,
                .vsync_idle_low = 0,
                .de_idle_high = 0,
                .pclk_active_neg = 0,
                .pclk_idle_high = 0
            }
        },
        .data_width = 16,
        .num_fbs = 2,
        .dma_burst_size = 64,
        // .bounce_buffer_size_px = 480 * 61,
        .hsync_gpio_num = PIN_NUM_HSYNC,
        .vsync_gpio_num = PIN_NUM_VSYNC,
        .de_gpio_num = PIN_NUM_DE,
        .pclk_gpio_num = PIN_NUM_PCLK,
        .disp_gpio_num = PIN_NUM_DISP_EN,
        .data_gpio_nums = {
            PIN_NUM_DATA0,
            PIN_NUM_DATA1,
            PIN_NUM_DATA2,
            PIN_NUM_DATA3,
            PIN_NUM_DATA4,
            PIN_NUM_DATA5,
            PIN_NUM_DATA6,
            PIN_NUM_DATA7,
            PIN_NUM_DATA8,
            PIN_NUM_DATA9,
            PIN_NUM_DATA10,
            PIN_NUM_DATA11,
            PIN_NUM_DATA12,
            PIN_NUM_DATA13,
            PIN_NUM_DATA14,
            PIN_NUM_DATA15
        },
        .flags = {
            .disp_active_low = 0,
            .refresh_on_demand = 0,
            .fb_in_psram = 1
        }
    };

    st7701_vendor_config_t vendor_config = {
        .rgb_config = &panel_config,
        .init_cmds = lcd_init_cmds,      // Uncomment these line if use custom initialization commands
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags = {
            .auto_del_panel_io = 1,         /**
                                             * Set to 1 if panel IO is no longer needed after LCD initialization.
                                             * If the panel IO pins are sharing other pins of the RGB interface to save GPIOs,
                                             * Please set it to 1 to release the pins.
                                             */
            .mirror_by_cmd = 0,             // Set to 0 if `auto_del_panel_io` is enabled
        },
    };
    const esp_lcd_panel_dev_config_t panel_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7701(io_handle, &panel_dev_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    // esp_lcd_panel_disp_on_off(panel_handle, true);
    // esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
    // ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    // ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

}

void rgb_lcd_brightness(uint8_t brightness)
{
    if (brightness > 100)
    {
        brightness = 100;
    }
    if (brightness < 0)
    {
        brightness = 0;
    }
    ESP_LOGI(TAG, "Set brightness to %d", brightness);
    brightness = (100 - brightness);
    
    ens_uart_send_cmd(0x40, 0x10, &brightness, 1);
}

void rgb_lcd_restart(void)
{
    if (panel_handle)
    {
        esp_lcd_rgb_panel_restart(panel_handle);
    }
    
}

void lcd_clear(uint16_t color)
{
    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    uint16_t *buffer = (uint16_t *)buf1;
    for (uint32_t i = 0; i < RESOLUTION_X * RESOLUTION_Y; i++)
    {
        buffer[i] = color;
    }
    taskENTER_CRITICAL(&lcd_spinlock);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, RESOLUTION_X, RESOLUTION_Y, buf1));
    taskEXIT_CRITICAL(&lcd_spinlock);

}