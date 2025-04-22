#ifndef _SPI_LCD_DRIVER_H_
#define _SPI_LCD_DRIVER_H_
#include "spi_lcd.h"
#include "stdlib.h"
#include "stdint.h"
#include "spi.h"
#include "gpio.h"
#include "spi.h"
#include "aiva_malloc.h" 
#include "aiva_sleep.h"
#include "syslog.h"


#define SPI_LCD_DELAY_MS(nms)   aiva_msleep(nms)

#define SPI_LCD_NAME_LEN        (30)



struct _spi_lcd_t;
typedef struct _spi_lcd_t spi_lcd_t;

typedef uint8_t gpio_pin_num_t;

typedef struct _spi_lcd_hw_t {
    spi_device_num_t spi_dev;
    spi_work_mode_t  spi_work_mode;
    spi_chip_select_t spi_cs;
    uint32_t spi_clk_rate;

    gpio_pin_num_t rst_pin;
    gpio_pin_num_t rs_pin;
    gpio_pin_num_t bkl_pin;   // bk light
} spi_lcd_hw_t;


typedef struct _lcd_params_t {
    spi_lcd_direction_t dir;
    uint16_t width;
    uint16_t height;
    uint16_t fore_color;
    uint16_t back_color;
    uint16_t cmd_set_x;
    uint16_t cmd_set_y;
    uint16_t cmd_wr_ram;
} lcd_params_t;


typedef void(*lcd_init_fun)(spi_lcd_t *spi_lcd);
typedef void(*lcd_wr_point_data_fun)(spi_lcd_t *spi_lcd, uint16_t data);
typedef void(*lcd_clear)(spi_lcd_t *spi_lcd, uint16_t color);
typedef void(*lcd_rotate)(spi_lcd_t *spi_lcd, spi_lcd_direction_t dir);


typedef struct _lcd_op_t {
    lcd_init_fun init;
    lcd_wr_point_data_fun send_point_data;
    lcd_clear clear;
    lcd_rotate rotate;
} lcd_op_t;



typedef struct _spi_lcd_t {
    spi_lcd_hw_t hw_dev;
    lcd_params_t params;
    lcd_op_t op;
    char name[SPI_LCD_NAME_LEN + 1];
} spi_lcd_t;



void spi_lcd_set_pin(gpio_pin_num_t pin_num, enum _gpio_pin_value_t pin_val);
int  spi_lcd_spi_send_data(spi_lcd_t *spi_lcd, const uint8_t *buf, uint32_t len);
int  spi_lcd_spi_send_byte(spi_lcd_t *spi_lcd, uint8_t byte);

void spi_lcd_wr_reg(spi_lcd_t *spi_lcd, uint8_t data);
void spi_lcd_wr_data(spi_lcd_t *spi_lcd, uint8_t data);
void spi_lcd_wr_data_2_reg(spi_lcd_t *spi_lcd, uint8_t reg, uint16_t val);

#endif
