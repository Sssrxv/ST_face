#include "spi_lcd_driver.h"
#include "udevice.h"

static void st7796_send_point_data(spi_lcd_t *spi_lcd, uint16_t data);
static void st7796_clear(spi_lcd_t *spi_lcd, uint16_t color);
static void st7796_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction);
static void st7796_init(spi_lcd_t *spi_lcd);



spi_lcd_t spi_lcd_st7796 = {
        .name = "ST7796_320x480",
        .hw_dev = {
            .spi_dev = SPI_DEVICE_1,
            .spi_work_mode = SPI_WORK_MODE_3,
            .spi_cs = SPI_CHIP_SELECT_0,
            .spi_clk_rate = 25000000UL,
            .rst_pin = GPIO_PIN5,
            .rs_pin = GPIO_PIN30,
            .bkl_pin = GPIO_MAX_PINNO
        },
        .params = {
            .dir = DIR_CLK_0,
            .width = 320,
            .height = 480,
            .fore_color = 0xFFFF,
            .back_color = 0x0000,
            .cmd_set_x = 0x2A,
            .cmd_set_y = 0x2B,
            .cmd_wr_ram = 0x2C
        },
        .op = {
            .init = st7796_init,
            .send_point_data = st7796_send_point_data,
            .clear = st7796_clear,
            .rotate = st7796_set_dir,
        }
    };


static void st7796_send_point_data(spi_lcd_t *spi_lcd, uint16_t data)
{    
    spi_lcd_wr_data(spi_lcd, data >> 8);
    spi_lcd_wr_data(spi_lcd, data);

    // const uint8_t dat_buf[] = { data >> 8, data };
    // spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    // spi_lcd_spi_send_data(spi_lcd, dat_buf, sizeof(dat_buf));
}

static void st7796_clear(spi_lcd_t *spi_lcd, uint16_t color)
{
#if 1
    uint8_t dat_buf[2] = { color >> 8, color };
    uint32_t frame_size = spi_lcd->params.width * spi_lcd->params.height * 2;
    uint8_t *frame_buf = (uint8_t*)aiva_malloc(frame_size);
    if (!frame_buf) {
        LOGE(__func__, "alloc memory error");
        return;
    }
    for (uint32_t i = 0; i < frame_size; ) {
        frame_buf[i++] = dat_buf[0];
        frame_buf[i++] = dat_buf[1];
    }

    spi_lcd_set_window((spi_lcd_handle_t)spi_lcd, 0, 0, spi_lcd->params.width - 1, spi_lcd->params.height - 1);   
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    spi_lcd_spi_send_data(spi_lcd, frame_buf, frame_size);

    if (frame_buf) {
        aiva_free(frame_buf);
    }

#else

    uint8_t point_buf[2] = { color >> 8, color };
    spi_lcd_set_window((spi_lcd_handle_t)spi_lcd, 0, 0, spi_lcd->params.width - 1, spi_lcd->params.height - 1);    
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    for(uint32_t i = 0; i < lcddev.height; i++) {
        for(uint32_t m = 0; m < lcddev.width; m++) {
            spi_lcd_spi_send_data(spi_lcd, point_buf, sizeof(point_buf));
        }
    }
#endif
}


static void st7796_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction)
{ 
    if (DIR_MAX <= direction) {
        return;
    }
    
    spi_lcd_wr_reg(spi_lcd, 0x36);    // Memory Access Control 
    switch(direction) {
    case DIR_CLK_0:   spi_lcd_wr_data(spi_lcd, 0x48); break;
    case DIR_CLK_90:  spi_lcd_wr_data(spi_lcd, 0xE8); break;
    case DIR_CLK_180: spi_lcd_wr_data(spi_lcd, 0x88); break;
    case DIR_CLK_270: spi_lcd_wr_data(spi_lcd, 0x28); break;
    default:break;
    }

}     

static void st7796_init(spi_lcd_t *spi_lcd)
{    
    spi_lcd_reset(spi_lcd);
      SPI_LCD_DELAY_MS(100);
    
    //************* Start Initial Sequence **********//
    spi_lcd_wr_reg(spi_lcd, 0x11); //Sleep out 
    SPI_LCD_DELAY_MS(120);    //Delay 120ms 

    //************* Start Initial Sequence **********// 
    spi_lcd_wr_reg(spi_lcd, 0Xf0);
    spi_lcd_wr_data(spi_lcd, 0xc3);
    spi_lcd_wr_reg(spi_lcd, 0Xf0);
    spi_lcd_wr_data(spi_lcd, 0x96);
    spi_lcd_wr_reg(spi_lcd, 0x3A);
    spi_lcd_wr_data(spi_lcd, 0x05);
    spi_lcd_wr_reg(spi_lcd, 0Xe8);
    spi_lcd_wr_data(spi_lcd, 0x40);
    spi_lcd_wr_data(spi_lcd, 0x82);
    spi_lcd_wr_data(spi_lcd, 0x07);
    spi_lcd_wr_data(spi_lcd, 0x18);
    spi_lcd_wr_data(spi_lcd, 0x27);
    spi_lcd_wr_data(spi_lcd, 0x0a);
    spi_lcd_wr_data(spi_lcd, 0xb6);
    spi_lcd_wr_data(spi_lcd, 0x33);
    spi_lcd_wr_reg(spi_lcd, 0Xc5);
    spi_lcd_wr_data(spi_lcd, 0x27);
    spi_lcd_wr_reg(spi_lcd, 0Xc2);
    spi_lcd_wr_data(spi_lcd, 0xa7);
    spi_lcd_wr_reg(spi_lcd, 0Xe0);
    spi_lcd_wr_data(spi_lcd, 0xf0);
    spi_lcd_wr_data(spi_lcd, 0x01);
    spi_lcd_wr_data(spi_lcd, 0x06);
    spi_lcd_wr_data(spi_lcd, 0x0f);
    spi_lcd_wr_data(spi_lcd, 0x12);
    spi_lcd_wr_data(spi_lcd, 0x1d);
    spi_lcd_wr_data(spi_lcd, 0x36);
    spi_lcd_wr_data(spi_lcd, 0x54);
    spi_lcd_wr_data(spi_lcd, 0x44);
    spi_lcd_wr_data(spi_lcd, 0x0c);
    spi_lcd_wr_data(spi_lcd, 0x18);
    spi_lcd_wr_data(spi_lcd, 0x16);
    spi_lcd_wr_data(spi_lcd, 0x13);
    spi_lcd_wr_data(spi_lcd, 0x15);
    spi_lcd_wr_reg(spi_lcd, 0Xe1);
    spi_lcd_wr_data(spi_lcd, 0xf0);
    spi_lcd_wr_data(spi_lcd, 0x01);
    spi_lcd_wr_data(spi_lcd, 0x05);
    spi_lcd_wr_data(spi_lcd, 0x0a);
    spi_lcd_wr_data(spi_lcd, 0x0b);
    spi_lcd_wr_data(spi_lcd, 0x07);
    spi_lcd_wr_data(spi_lcd, 0x32);
    spi_lcd_wr_data(spi_lcd, 0x44);
    spi_lcd_wr_data(spi_lcd, 0x44);
    spi_lcd_wr_data(spi_lcd, 0x0c);
    spi_lcd_wr_data(spi_lcd, 0x18);
    spi_lcd_wr_data(spi_lcd, 0x17);
    spi_lcd_wr_data(spi_lcd, 0x13);
    spi_lcd_wr_data(spi_lcd, 0x16);
    spi_lcd_wr_reg(spi_lcd, 0Xf0);
    spi_lcd_wr_data(spi_lcd, 0x3c);
    spi_lcd_wr_reg(spi_lcd, 0Xf0);
    spi_lcd_wr_data(spi_lcd, 0x69);
    spi_lcd_wr_reg(spi_lcd, 0X29);    

    st7796_set_dir(spi_lcd, spi_lcd->params.dir);

    spi_lcd_set_pin(spi_lcd->hw_dev.bkl_pin, GPIO_PV_HIGH);
}

/* Export Dev: _id,                 _name,          _flag,  _driver,      	   _priv */
UDEVICE_EXPORT(UCLASS_DISPLAY_SPI,  spi_lcd_st7796,    0,      &spi_lcd_st7796,  NULL);