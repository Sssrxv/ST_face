#include "spi_lcd_driver.h"
#include "udevice.h"

static void ili9488_send_point_data(spi_lcd_t *spi_lcd, uint16_t data);
static void ili9488_clear(spi_lcd_t *spi_lcd, uint16_t color);
static void ili9488_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction);
static void ili9488_init(spi_lcd_t *spi_lcd);


spi_lcd_t spi_lcd_ili9488 = {
        .name = "ILI9488_320x480",
        .hw_dev = {
            .spi_dev = SPI_DEVICE_1,
            .spi_work_mode = SPI_WORK_MODE_0,
            .spi_cs = SPI_CHIP_SELECT_0,
            .spi_clk_rate = 33000000UL,
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
            .init = ili9488_init,
            .send_point_data = ili9488_send_point_data,
            .clear = ili9488_clear,
			.rotate = ili9488_set_dir,
        }
    };


static void ili9488_send_point_data(spi_lcd_t *spi_lcd, uint16_t data)
{    
    //18Bit RGB   rrrrrr00 gggggg00 bbbbbb00
    // spi_lcd_wr_data(spi_lcd, (data >> 8) & 0xF8);    //RED 5
    // spi_lcd_wr_data(spi_lcd, (data >> 3) & 0xFC);    //GREEN 6
    // spi_lcd_wr_data(spi_lcd, data << 3);             //BLUE 5

    const uint8_t dat_buf[] = { (data >> 8) & 0xF8, (data >> 3) & 0xFC, data << 3 };
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    spi_lcd_spi_send_data(spi_lcd, dat_buf, sizeof(dat_buf));
}

static void ili9488_clear(spi_lcd_t *spi_lcd, uint16_t color)
{
#if 1
    uint8_t rgb_buf[3] = { (color >> 8) & 0xF8, (color >> 3) & 0xFC, color << 3 };
    uint32_t frame_size = spi_lcd->params.width * spi_lcd->params.height * 3;
    uint8_t *frame_buf = (uint8_t*)aiva_malloc(frame_size);
    if (!frame_buf) {
        LOGE(__func__, "alloc memory error");
        return;
    }
    for (uint32_t i = 0; i < frame_size; ) {
        frame_buf[i++] = rgb_buf[0];
        frame_buf[i++] = rgb_buf[1];
        frame_buf[i++] = rgb_buf[2];
    }

    spi_lcd_set_window((spi_lcd_handle_t)spi_lcd, 0, 0, spi_lcd->params.width - 1, spi_lcd->params.height - 1);   
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    spi_lcd_spi_send_data(spi_lcd, frame_buf, frame_size);

    if (frame_buf) {
        aiva_free(frame_buf);
    }

#else

    uint8_t point_buf[3] = { (color >> 8) & 0xF8, (color >> 3) & 0xFC, color << 3 };
    spi_lcd_set_window((spi_lcd_handle_t)spi_lcd, 0, 0, spi_lcd->params.width - 1, spi_lcd->params.height - 1);    
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    for(uint32_t i = 0; i < lcddev.height; i++) {
        for(uint32_t m = 0; m < lcddev.width; m++) {
            spi_lcd_spi_send_data(spi_lcd, point_buf, sizeof(point_buf));
        }
    }
#endif
} 

static void ili9488_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction)
{ 
    // spi_lcd_direction_t direction = spi_lcd->params.dir;

    switch(direction) {
    case DIR_CLK_0:
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36, (1<<3)|(0<<6)|(0<<7)); //BGR==1,MY==0,MX==0,MV==0
        break;
    case DIR_CLK_90:
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36, (1<<3)|(0<<7)|(1<<6)|(1<<5));  //BGR==1,MY==1,MX==0,MV==1
        break;
    case DIR_CLK_180:                                          
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(1<<6)|(1<<7));  //BGR==1,MY==0,MX==0,MV==0
        break;
    case DIR_CLK_270:
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(1<<7)|(1<<5));  //BGR==1,MY==1,MX==0,MV==1
        break;    
    default:break;
    }
}

static void ili9488_init(spi_lcd_t *spi_lcd)
{
    spi_lcd_reset(spi_lcd);

//************* ILI9488 **********//     
    spi_lcd_wr_reg(spi_lcd, 0XF7);
    spi_lcd_wr_data(spi_lcd, 0xA9);
    spi_lcd_wr_data(spi_lcd, 0x51);
    spi_lcd_wr_data(spi_lcd, 0x2C);
    spi_lcd_wr_data(spi_lcd, 0x82);
    spi_lcd_wr_reg(spi_lcd, 0xC0);
    spi_lcd_wr_data(spi_lcd, 0x11);
    spi_lcd_wr_data(spi_lcd, 0x09);
    spi_lcd_wr_reg(spi_lcd, 0xC1);
    spi_lcd_wr_data(spi_lcd, 0x41);
    spi_lcd_wr_reg(spi_lcd, 0XC5);
    spi_lcd_wr_data(spi_lcd, 0x00);
    spi_lcd_wr_data(spi_lcd, 0x0A);
    spi_lcd_wr_data(spi_lcd, 0x80);
    spi_lcd_wr_reg(spi_lcd, 0xB1);
    // spi_lcd_wr_data(spi_lcd, 0xB0);
    spi_lcd_wr_data(spi_lcd, 0xC0);  //
    spi_lcd_wr_data(spi_lcd, 0x11);
    spi_lcd_wr_reg(spi_lcd, 0xB4);
    spi_lcd_wr_data(spi_lcd, 0x02);
    spi_lcd_wr_reg(spi_lcd, 0xB6);
    spi_lcd_wr_data(spi_lcd, 0x02);
    // spi_lcd_wr_data(spi_lcd, 0xF2);  // 
    spi_lcd_wr_data(spi_lcd, 0x42);
    spi_lcd_wr_reg(spi_lcd, 0xB7);
    spi_lcd_wr_data(spi_lcd, 0xc6);
    spi_lcd_wr_reg(spi_lcd, 0xBE);
    spi_lcd_wr_data(spi_lcd, 0x00);
    spi_lcd_wr_data(spi_lcd, 0x04);
    spi_lcd_wr_reg(spi_lcd, 0xE9);
    spi_lcd_wr_data(spi_lcd, 0x00);
    spi_lcd_wr_reg(spi_lcd, 0x36);
    spi_lcd_wr_data(spi_lcd, (1<<3)|(0<<7)|(1<<6)|(1<<5));
    spi_lcd_wr_reg(spi_lcd, 0x3A);
    spi_lcd_wr_data(spi_lcd, 0x66);
    spi_lcd_wr_reg(spi_lcd, 0xE0);
    spi_lcd_wr_data(spi_lcd, 0x00);
    spi_lcd_wr_data(spi_lcd, 0x07);
    spi_lcd_wr_data(spi_lcd, 0x10);
    spi_lcd_wr_data(spi_lcd, 0x09);
    spi_lcd_wr_data(spi_lcd, 0x17);
    spi_lcd_wr_data(spi_lcd, 0x0B);
    spi_lcd_wr_data(spi_lcd, 0x41);
    spi_lcd_wr_data(spi_lcd, 0x89);
    spi_lcd_wr_data(spi_lcd, 0x4B);
    spi_lcd_wr_data(spi_lcd, 0x0A);
    spi_lcd_wr_data(spi_lcd, 0x0C);
    spi_lcd_wr_data(spi_lcd, 0x0E);
    spi_lcd_wr_data(spi_lcd, 0x18);
    spi_lcd_wr_data(spi_lcd, 0x1B);
    spi_lcd_wr_data(spi_lcd, 0x0F);
    spi_lcd_wr_reg(spi_lcd, 0XE1);
    spi_lcd_wr_data(spi_lcd, 0x00);
    spi_lcd_wr_data(spi_lcd, 0x17);
    spi_lcd_wr_data(spi_lcd, 0x1A);
    spi_lcd_wr_data(spi_lcd, 0x04);
    spi_lcd_wr_data(spi_lcd, 0x0E);
    spi_lcd_wr_data(spi_lcd, 0x06);
    spi_lcd_wr_data(spi_lcd, 0x2F);
    spi_lcd_wr_data(spi_lcd, 0x45);
    spi_lcd_wr_data(spi_lcd, 0x43);
    spi_lcd_wr_data(spi_lcd, 0x02);
    spi_lcd_wr_data(spi_lcd, 0x0A);
    spi_lcd_wr_data(spi_lcd, 0x09);
    spi_lcd_wr_data(spi_lcd, 0x32);
    spi_lcd_wr_data(spi_lcd, 0x36);
    spi_lcd_wr_data(spi_lcd, 0x0F);
    spi_lcd_wr_reg(spi_lcd, 0x11);
    SPI_LCD_DELAY_MS(120);
    spi_lcd_wr_reg(spi_lcd, 0x29);
    
    ili9488_set_dir(spi_lcd, spi_lcd->params.dir);
    spi_lcd_set_pin(spi_lcd->hw_dev.bkl_pin, GPIO_PV_HIGH);
    ili9488_clear(spi_lcd, WHITE);
}
 


int ili9488_fill_rect(spi_lcd_handle_t handle, uint16_t x, uint16_t y, uint16_t window_w, uint16_t window_h, uint16_t color)
{
	spi_lcd_t *spi_lcd = (spi_lcd_t*)handle;
    uint8_t rgb_buf[3] = { (color >> 8) & 0xF8, (color >> 3) & 0xFC, color << 3 };
    uint32_t frame_size = window_w * window_h * 3;
    uint8_t *frame_buf = (uint8_t*)aiva_malloc(frame_size);
    if (!frame_buf) {
        LOGE(__func__, "alloc memory error");
        return -1;
    }

    for (uint32_t i = 0; i < frame_size; ) {
        frame_buf[i++] = rgb_buf[0];
        frame_buf[i++] = rgb_buf[1];
        frame_buf[i++] = rgb_buf[2];
    }

    spi_lcd_set_window(spi_lcd, x, y, x + window_w - 1, y + window_h - 1); 
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    spi_lcd_spi_send_data(spi_lcd, frame_buf, frame_size);

    if (frame_buf) {
        aiva_free(frame_buf);
    }

    return 0;
}

int ili9488_test_lcd_frame_rate(spi_lcd_handle_t handle, uint16_t window_w, uint16_t window_h)
{
	spi_lcd_t *spi_lcd = (spi_lcd_t*)handle;
    uint16_t Color = RED;
    uint8_t rgb_buf_r[3] = { (Color >> 8) & 0xF8, (Color >> 3) & 0xFC, Color << 3 };
    uint32_t frame_size = window_w * window_h * 3;
    uint8_t *frame_buf_r = (uint8_t*)aiva_malloc(frame_size);
    uint8_t *frame_buf_g = (uint8_t*)aiva_malloc(frame_size);
    if (!frame_buf_r || !frame_buf_g) {
        LOGE(__func__, "alloc memory error");
        return -1;
    }

    for (uint32_t i = 0; i < frame_size; ) {
        frame_buf_r[i++] = rgb_buf_r[0];
        frame_buf_r[i++] = rgb_buf_r[1];
        frame_buf_r[i++] = rgb_buf_r[2];
    }

    Color = GREEN;
    uint8_t rgb_buf_g[3] = { (Color >> 8) & 0xF8, (Color >> 3) & 0xFC, Color << 3 };
    for (uint32_t i = 0; i < frame_size; ) {
        frame_buf_g[i++] = rgb_buf_g[0];
        frame_buf_g[i++] = rgb_buf_g[1];
        frame_buf_g[i++] = rgb_buf_g[2];
    }

    spi_lcd_set_window(spi_lcd, 0, 0, window_w - 1, window_h - 1);   
    spi_lcd_set_pin(spi_lcd->hw_dev.rs_pin, GPIO_PV_HIGH);
    uint32_t tick_ms_num, tick_ms_in = aiva_get_curr_ms();
    uint8_t lop_cnt = 0;
    for (lop_cnt = 0; lop_cnt < 10; ++lop_cnt) {
        spi_lcd_spi_send_data(spi_lcd, frame_buf_r, frame_size);
        spi_lcd_spi_send_data(spi_lcd, frame_buf_g, frame_size);
    }
    tick_ms_num = aiva_get_curr_ms() - tick_ms_in;

    uint16_t frame_sum = lop_cnt << 1;
    float frame_ms = (float)tick_ms_num / frame_sum;
    float fps = 1000.0 / frame_ms;

    LOGI(__func__, "window_w: %d,  window_h: %d", window_w, window_h);
    LOGI(__func__, "flush %d frame used %d ms", frame_sum, tick_ms_num);
    LOGI(__func__, "flush one frame time %.3f ms", frame_ms);
    LOGI(__func__, "fps %.2f", fps);


    if (frame_buf_r) {
        aiva_free(frame_buf_r);
    }
    if (frame_buf_g) {
        aiva_free(frame_buf_g);
    }

    return 0;
}

/* Export Dev: _id,                 _name,          _flag,  _driver,      	   _priv */
UDEVICE_EXPORT(UCLASS_DISPLAY_SPI,  spi_lcd_ili9488,   0,      &spi_lcd_ili9488,  NULL);