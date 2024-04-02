#include "spi_lcd_driver.h"
#include "udevice.h"

static void ili9341_wr_point_data(spi_lcd_t *spi_lcd, uint16_t data);
static void ili9341_clear(spi_lcd_t *spi_lcd, uint16_t color);
static void ili9341_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction);
static void ili9341_init(spi_lcd_t *spi_lcd);



spi_lcd_t spi_lcd_ili9341 = {
        .name = "ILI9341_240x320",
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
            .width = 240,
            .height = 320,
            .fore_color = 0xFFFF,
            .back_color = 0x0000,
            .cmd_set_x = 0x2A,
            .cmd_set_y = 0x2B,
            .cmd_wr_ram = 0x2C
        },
        .op = {
            .init = ili9341_init,
            .send_point_data = ili9341_wr_point_data,
            .clear = ili9341_clear,
			.rotate = ili9341_set_dir,
        }
    };


static void ili9341_wr_point_data(spi_lcd_t *spi_lcd, uint16_t data)
{    
    spi_lcd_wr_data(spi_lcd, data >> 8);
    spi_lcd_wr_data(spi_lcd, data);
}

static void ili9341_clear(spi_lcd_t *spi_lcd, uint16_t color)
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

static void ili9341_set_dir(spi_lcd_t *spi_lcd, spi_lcd_direction_t direction)
{ 
    // spi_lcd_direction_t direction = spi_lcd->params.dir;

    switch(direction) {
    case DIR_CLK_0:                                          
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
        break;
    case DIR_CLK_90:
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
        break;
    case DIR_CLK_180:                                      
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
        break;
    case DIR_CLK_270:
        spi_lcd_wr_data_2_reg(spi_lcd, 0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
        break;    
    default:break;
    }
}     

static void ili9341_init(spi_lcd_t *spi_lcd)
{                   
    spi_lcd_reset(spi_lcd);

	//*************3.2inch ILI9341**********//
	spi_lcd_wr_reg(spi_lcd, 0xCF);  
	spi_lcd_wr_data(spi_lcd, 0x00); 
	spi_lcd_wr_data(spi_lcd, 0xD9); //C1 
	spi_lcd_wr_data(spi_lcd, 0X30); 
	spi_lcd_wr_reg(spi_lcd, 0xED);  
	spi_lcd_wr_data(spi_lcd, 0x64); 
	spi_lcd_wr_data(spi_lcd, 0x03); 
	spi_lcd_wr_data(spi_lcd, 0X12); 
	spi_lcd_wr_data(spi_lcd, 0X81); 
	spi_lcd_wr_reg(spi_lcd, 0xE8);  
	spi_lcd_wr_data(spi_lcd, 0x85); 
	spi_lcd_wr_data(spi_lcd, 0x10); 
	spi_lcd_wr_data(spi_lcd, 0x7A); 
	spi_lcd_wr_reg(spi_lcd, 0xCB);  
	spi_lcd_wr_data(spi_lcd, 0x39); 
	spi_lcd_wr_data(spi_lcd, 0x2C); 
	spi_lcd_wr_data(spi_lcd, 0x00); 
	spi_lcd_wr_data(spi_lcd, 0x34); 
	spi_lcd_wr_data(spi_lcd, 0x02); 
	spi_lcd_wr_reg(spi_lcd, 0xF7);  
	spi_lcd_wr_data(spi_lcd, 0x20); 
	spi_lcd_wr_reg(spi_lcd, 0xEA);  
	spi_lcd_wr_data(spi_lcd, 0x00); 
	spi_lcd_wr_data(spi_lcd, 0x00); 
	spi_lcd_wr_reg(spi_lcd, 0xC0);    //Power control 
	spi_lcd_wr_data(spi_lcd, 0x1B);   //VRH[5:0] 
	spi_lcd_wr_reg(spi_lcd, 0xC1);    //Power control 
	spi_lcd_wr_data(spi_lcd, 0x12);   //SAP[2:0];BT[3:0] //0x01
	spi_lcd_wr_reg(spi_lcd, 0xC5);    //VCM control 
	spi_lcd_wr_data(spi_lcd, 0x26); 	 //3F
	spi_lcd_wr_data(spi_lcd, 0x26); 	 //3C
	spi_lcd_wr_reg(spi_lcd, 0xC7);    //VCM control2 
	spi_lcd_wr_data(spi_lcd, 0XB0); 
	spi_lcd_wr_reg(spi_lcd, 0x36);    // Memory Access Control 
	spi_lcd_wr_data(spi_lcd, 0x08); 
	spi_lcd_wr_reg(spi_lcd, 0x3A);   
	spi_lcd_wr_data(spi_lcd, 0x55); 
	spi_lcd_wr_reg(spi_lcd, 0xB1);   
	spi_lcd_wr_data(spi_lcd, 0x00);   
	spi_lcd_wr_data(spi_lcd, 0x1A); 
	spi_lcd_wr_reg(spi_lcd, 0xB6);    // Display Function Control 
	spi_lcd_wr_data(spi_lcd, 0x0A); 
	spi_lcd_wr_data(spi_lcd, 0xA2); 
	spi_lcd_wr_reg(spi_lcd, 0xF2);    // 3Gamma Function Disable 
	spi_lcd_wr_data(spi_lcd, 0x00); 
	spi_lcd_wr_reg(spi_lcd, 0x26);    //Gamma curve selected 
	spi_lcd_wr_data(spi_lcd, 0x01); 
	spi_lcd_wr_reg(spi_lcd, 0xE0); //Set Gamma
	spi_lcd_wr_data(spi_lcd, 0x1F);
	spi_lcd_wr_data(spi_lcd, 0x24);
	spi_lcd_wr_data(spi_lcd, 0x24);
	spi_lcd_wr_data(spi_lcd, 0x0D);
	spi_lcd_wr_data(spi_lcd, 0x12);
	spi_lcd_wr_data(spi_lcd, 0x09);
	spi_lcd_wr_data(spi_lcd, 0x52);
	spi_lcd_wr_data(spi_lcd, 0xB7);
	spi_lcd_wr_data(spi_lcd, 0x3F);
	spi_lcd_wr_data(spi_lcd, 0x0C);
	spi_lcd_wr_data(spi_lcd, 0x15);
	spi_lcd_wr_data(spi_lcd, 0x06);
	spi_lcd_wr_data(spi_lcd, 0x0E);
	spi_lcd_wr_data(spi_lcd, 0x08);
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_reg(spi_lcd, 0XE1); //Set Gamma
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0x1B);
	spi_lcd_wr_data(spi_lcd, 0x1B);
	spi_lcd_wr_data(spi_lcd, 0x02);
	spi_lcd_wr_data(spi_lcd, 0x0E);
	spi_lcd_wr_data(spi_lcd, 0x06);
	spi_lcd_wr_data(spi_lcd, 0x2E);
	spi_lcd_wr_data(spi_lcd, 0x48);
	spi_lcd_wr_data(spi_lcd, 0x3F);
	spi_lcd_wr_data(spi_lcd, 0x03);
	spi_lcd_wr_data(spi_lcd, 0x0A);
	spi_lcd_wr_data(spi_lcd, 0x09);
	spi_lcd_wr_data(spi_lcd, 0x31);
	spi_lcd_wr_data(spi_lcd, 0x37);
	spi_lcd_wr_data(spi_lcd, 0x1F);

	spi_lcd_wr_reg(spi_lcd, 0x2B); 
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0x01);
	spi_lcd_wr_data(spi_lcd, 0x3f);
	spi_lcd_wr_reg(spi_lcd, 0x2A); 
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0x00);
	spi_lcd_wr_data(spi_lcd, 0xef);	 
	spi_lcd_wr_reg(spi_lcd, 0x11); //Exit Sleep
	SPI_LCD_DELAY_MS(120);
	spi_lcd_wr_reg(spi_lcd, 0x29); //display on		
	ili9341_set_dir(spi_lcd, spi_lcd->params.dir);
	spi_lcd_set_pin(spi_lcd->hw_dev.bkl_pin, GPIO_PV_HIGH);
	ili9341_clear(spi_lcd, WHITE);
}


/* Export Dev: _id,                 _name,          _flag,  _driver,      	   _priv */
UDEVICE_EXPORT(UCLASS_DISPLAY_SPI,  spi_lcd_ili9341,   0,      &spi_lcd_ili9341,  NULL);
