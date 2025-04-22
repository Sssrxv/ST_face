
#ifndef _SPI_LCD_H_
#define _SPI_LCD_H_
#include "stdint.h"
#include "syslog.h"


typedef enum {
    DIR_CLK_0 = 0,
    DIR_CLK_90,
    DIR_CLK_180,
    DIR_CLK_270,
    DIR_MAX
} spi_lcd_direction_t;


typedef void* spi_lcd_handle_t;



// color definition
#define WHITE       			0xFFFF
#define BLACK      				0x0000	  
#define BLUE       				0x001F  
#define BRED        			0XF81F
#define GRED 					0XFFE0
#define GBLUE					0X07FF
#define RED         			0xF800
#define MAGENTA     			0xF81F
#define GREEN       			0x07E0
#define CYAN        			0x7FFF
#define YELLOW      			0xFFE0
#define BROWN 					0XBC40 
#define BRRED 					0XFC07 
#define GRAY  					0X8430 
	

#define DARKBLUE      			0X01CF	
#define LIGHTBLUE      			0X7D7C 
#define GRAYBLUE       			0X5458 
	

#define LIGHTGREEN     			0X841F 
#define LIGHTGRAY     			0XEF5B 
#define LGRAY 					0XC618 

#define LGRAYBLUE      			0XA651 
#define LBBLUE          		0X2B12



spi_lcd_handle_t spi_lcd_get_handle(const char*dev_name);
int spi_lcd_init(spi_lcd_handle_t handle);

void spi_lcd_reset(spi_lcd_handle_t handle);

void spi_lcd_wr_ram_prepare(spi_lcd_handle_t handle);
void spi_lcd_set_window(spi_lcd_handle_t handle, uint16_t x_st, uint16_t y_st,uint16_t x_sp, uint16_t y_sp);
void spi_lcd_set_cursor(spi_lcd_handle_t handle, uint16_t Xpos, uint16_t Ypos);
void spi_lcd_draw_point(spi_lcd_handle_t handle, uint16_t x, uint16_t y);
void spi_lcd_draw_point_(spi_lcd_handle_t handle, uint16_t x, uint16_t y, uint16_t data);
void spi_lcd_clear(spi_lcd_handle_t handle, uint16_t color);
void spi_lcd_set_dir(spi_lcd_handle_t handle, spi_lcd_direction_t dir);

uint16_t spi_lcd_get_dir(spi_lcd_handle_t handle);
uint16_t spi_lcd_get_width(spi_lcd_handle_t handle);
uint16_t spi_lcd_get_height(spi_lcd_handle_t handle);
void spi_lcd_set_pen_fore_color(spi_lcd_handle_t handle, uint16_t col);
void spi_lcd_set_pen_back_color(spi_lcd_handle_t handle, uint16_t col);


void spi_lcd_fill_rect(spi_lcd_handle_t handle, uint16_t x, uint16_t y, uint16_t window_w, uint16_t window_h, uint16_t color);
void spi_lcd_wr_frame_stream_prepare(spi_lcd_handle_t handle, uint16_t x, uint16_t y, uint16_t frame_w, uint16_t frame_h);
void spi_lcd_wr_frame(spi_lcd_handle_t handle, uint8_t frame_buf[], uint32_t frame_size);



void spi_lcd_test(spi_lcd_handle_t handle);
int  spi_lcd_test_frame_rate_rgb565(spi_lcd_handle_t handle, uint16_t window_w, uint16_t window_h);


#endif
