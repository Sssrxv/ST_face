#include <string.h>
#include <stdint.h>
#include "aiva_sleep.h"
#include "aiva_printf.h"
#include "dvpout_lcd_driver.h"
#include "syslog.h"
#include "aiva_utils.h"

#include "spi.h"
#include "gpio.h"
#include "sysctl.h"
#include "udevice.h"

#define REGFLAG_DELAY             		    0xFD
#define REGFLAG_END_OF_TABLE      		    0xFE   // END OF REGISTERS MARKER
#define TABLE_LEN                           AIVA_ARRAY_LEN

typedef struct {
    uint8_t cmd;
    uint8_t count;
    uint8_t para_list[6];
} __ATTR_ALIGN__(32) lcd_setting_table_t;

static int lcd_send_cmd_data(dvpout_lcd_driver_t *dev_driver, uint32_t cmd, uint8_t *data, uint32_t count);

static lcd_setting_table_t ili9806e_sleep_out_setting[] = {
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0

	// Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 150, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	//{0x2C, 1, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static lcd_setting_table_t ili9806e_sleep_in_setting[] = {
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
	
	// Display off sequence
	{0x28, 1, {0x00}},
    {REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


/* EPL:  DE polarity, 0 - low enable 1 - high enable
 * DPL:  PCLK polarity, 0 - rising edge, 1 - falling edge
 * HSPL: HS polarity, 0 - low level sync clock, 1 - high level sync clock
 * VSPL: VS polarity, 0 - low level sync clock, 1 - high level sync clock
 */

static lcd_setting_table_t ili9806e_init_setting[] = {
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1
    {0x08,	1,	{0x08}},	// output SDA
	/*{0x21,	1,	{0x00}},	// EPL[0]: 0 - DE low enable, 1 - DE high enable, DPL[1], HSPL[2], VSPL[3]*/
	{0x21,	1,	{0x05}},	// EPL[0]: 0 - DE low enable, 1 - DE high enable, DPL[1], HSPL[2], VSPL[3]
    {0x25,	1,	{0x02}},	// VFP[6:0]
    {0x26,	1,	{0x14}},	// VBP[6:0] = (VLW+VBP)
    /*{0x26,	1,	{0x20}},	// VBP[6:0] = (VLW+VBP)*/
    {0x27,	1,	{0x28}},	// HBP[7:0] = (HLW+HBP)
	{0x28,	1,	{0x00}},	// HBP[9:8] = (HLW+HBP)
    {0x30,	1,	{0x01}},	// 480 X 854
	{0x31,	1,	{0x02}},	// 2-dot Inversion
	{0x40,	1,	{0x18}},	// DDVDH/L BT
	{0x41,	1,	{0x66}},	// DVDDH DVDDL clamp 
	{0x42,	1,	{0x02}},	// VGH/VGL 
	{0x43,	1,	{0x0a}},	// VGH/VGL
	{0x50,	1,	{0x78}},	// VGMP 4.6
	{0x51,	1,	{0x78}},	// VGMN 4.6
	{0x52,	1,	{0x00}},	// Flicker 
    {0x53,	1,	{0x1f}},	// Flicker
	{0x57,	1,	{0x50}},	//
	{0x60,	1,	{0x07}},	// SDTI
	{0x61,	1,	{0x01}},	// CRTI
	{0x62,	1,	{0x07}},	// EQTI
	{0x63,	1,	{0x00}},	// PCTI
	//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1
	{0xA0,	1,	{0x00}},	// Gamma 0
	{0xA1,	1,	{0x0A}},	// Gamma 4
	{0xA2,	1,	{0x12}},	// Gamma 8
	{0xA3,	1,	{0x0d}},	// Gamma 16
	{0xA4,	1,	{0x07}},	// Gamma 24
	{0xA5,	1,	{0x09}},	// Gamma 52
	{0xA6,	1,	{0x06}},	// Gamma 80
	{0xA7,	1,	{0x03}},	// Gamma 108
	{0xA8,	1,	{0x09}},	// Gamma 147
	{0xA9,	1,	{0x0d}},	// Gamma 175
	{0xAA,	1,	{0x15}},	// Gamma 203
	{0xAB,	1,	{0x07}},	// Gamma 231
	{0xAC,	1,	{0x0d}},	// Gamma 239
	{0xAD,	1,	{0x0d}},	// Gamma 247  
	{0xAE,	1,	{0x06}},	// Gamma 251
	{0xAF,	1,	{0x00}},	// Gamma 255
    //============== Nagitive ==============================
	{0xC0,	1,	{0x00}},	// Gamma 0
	{0xC1,	1,	{0x0A}},	// Gamma 4
	{0xC2,	1,	{0x12}},	// Gamma 8
	{0xC3,	1,	{0x0E}},	// Gamma 16
	{0xC4,	1,	{0x07}},	// Gamma 24
	{0xC5,	1,	{0x09}},	// Gamma 52
	{0xC6,	1,	{0x07}},	// Gamma 80
	{0xC7,	1,	{0x03}},	// Gamma 108
	{0xC8,	1,	{0x09}},	// Gamma 147
	{0xC9,	1,	{0x0c}},	// Gamma 175
	{0xCA,	1,	{0x15}},	// Gamma 203
	{0xCB,	1,	{0x07}},	// Gamma 231
	{0xCC,	1,	{0x0C}},	// Gamma 239
	{0xCD,	1,	{0x0d}},	// Gamma 247
	{0xCE,	1,	{0x06}},	// Gamma 251
	{0xCF,	1,	{0x00}},	// Gamma 255
    // ================== page 6 command =======================
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x06}},// Change to Page 6
	{0x00,	1,	{0x21}},	//
	{0x01,	1,	{0x06}},	//
	{0x02,	1,	{0xa0}},	//
	{0x03,	1,	{0x02}},	//
	{0x04,	1,	{0x01}},	//
	{0x05,	1,	{0x01}},	//
	{0x06,	1,	{0x80}},	//
	{0x07,	1,	{0x04}},	//
	{0x08,	1,	{0x00}},	//
	{0x09,	1,	{0x80}},	//
	{0x0A,	1,	{0x00}},	//
	{0x0B,	1,	{0x00}},	//
	{0x0C,	1,	{0x2c}},	//
	{0x0D,	1,	{0x2c}},	//
	{0x0E,	1,	{0x1c}},	//
	{0x0F,	1,	{0x00}},	//
	{0x10,	1,	{0xff}},	//
	{0x11,	1,	{0xF0}},	//
	{0x12,	1,	{0x00}},	//
	{0x13,	1,	{0x00}},	//
	{0x14,	1,	{0x00}},	//
	{0x15,	1,	{0xC0}},	//
	{0x16,	1,	{0x08}},	//
	{0x17,	1,	{0x00}},	//
	{0x18,	1,	{0x00}},	//
	{0x19,	1,	{0x00}},	//
	{0x1A,	1,	{0x00}},	//
	{0x1B,	1,	{0x00}},	//
	{0x1C,	1,	{0x00}},	//
	{0x1D,	1,	{0x00}},	//
	{0x20,	1,	{0x01}},	//
	{0x21,	1,	{0x23}},	//
	{0x22,	1,	{0x45}},	//
	{0x23,	1,	{0x67}},	//
	{0x24,	1,	{0x01}},	//
	{0x25,	1,	{0x23}},	//
	{0x26,	1,	{0x45}},	//
	{0x27,	1,	{0x67}},	//

	{0x30,	1,	{0x12}},	//
	{0x31,	1,	{0x22}},	//
	{0x32,	1,	{0x22}},	//
	{0x33,	1,	{0x22}},	//
	{0x34,	1,	{0x87}},	//	
	{0x35,	1,	{0x96}},	//	
	{0x36,	1,	{0xAA}},	//
	{0x37,	1,	{0xDB}},	//
	{0x38,	1,	{0xCC}},	//
	{0x39,	1,	{0xBD}},	//		
	{0x3A,	1,	{0x78}},	//
	{0x3B,	1,	{0x69}},	//		
	{0x3C,	1,	{0x22}},	//
	{0x3D,	1,	{0x22}},	//	
	{0x3E,	1,	{0x22}},	//
	{0x3F,	1,	{0x22}},	//
	{0x40,	1,	{0x22}},	//
	{0x52,	1,	{0x10}},	//
	{0x53,	1,	{0x10}},	//VGLO tie to VGL;
	
    {0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x07}},// Change to Page 7
	{0x17,	1,	{0x22}},	// 
	{0x18,	1,	{0x1D}},	// 
	{0x02,	1,	{0x77}},	// 
	{0xE1,	1,	{0x79}},	// 
	{0x06,	1,	{0x11}},	// 

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
        //{0x3a,	1,	{0x77}},	// RGB888
	{0x3a,	1,	{0x57}},	// RGB565
	{0x11,	1,	{0x00}},	// Sleep-Out
	{REGFLAG_DELAY, 150, {}},
	{0x29,	1,	{0x00}},	// Display on

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static void push_table(dvpout_lcd_driver_t *dev_driver, lcd_setting_table_t *table, uint32_t count)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
        unsigned cmd;
        cmd = table[i].cmd;
        switch (cmd) {
            case REGFLAG_DELAY :
                aiva_msleep(table[i].count);
                break;
            case REGFLAG_END_OF_TABLE :
                break;
            default:
		        lcd_send_cmd_data(dev_driver, cmd, table[i].para_list, table[i].count);
                break;
       	}
    }
}

static int ili9806e_init(dvpout_lcd_driver_t *dev_driver)
{
    dev_driver->set_reset_pin(dev_driver, 1);
    aiva_msleep(10);
    dev_driver->set_reset_pin(dev_driver, 0);
    aiva_msleep(10);
    dev_driver->set_reset_pin(dev_driver, 1);
    aiva_msleep(120);
    // TODO
    push_table(dev_driver, ili9806e_init_setting, TABLE_LEN(ili9806e_init_setting));
    dev_driver->set_backlight(dev_driver, 1);
    LOGD(__func__, " OK");

    return 0;
}

static int ili9806e_suspend(dvpout_lcd_driver_t *dev_driver)
{
    push_table(dev_driver, ili9806e_sleep_in_setting, TABLE_LEN(ili9806e_sleep_in_setting));
    dev_driver->set_backlight(dev_driver, 0);

    return 0;
}

static int ili9806e_resume(dvpout_lcd_driver_t *dev_driver)
{
    push_table(dev_driver, ili9806e_sleep_out_setting, TABLE_LEN(ili9806e_sleep_out_setting));
    dev_driver->set_backlight(dev_driver, 1);

    return 0;
}

static int ili9806e_get_params(dvpout_lcd_driver_t *dev_driver, display_dvpout_params_t *param)
{
    (void)dev_driver;
    param->width       = 480;
    param->height      = 854;
    param->hlw         = 20;
    param->vlw         = 10;
    param->hbp         = 20;
    param->vbp         = 10;
    param->hfp         = 12;
    param->vfp         = 2;
    param->vs_pol      = DVPO_VS_POL_PULSE;
    param->hs_pol      = DVPO_HS_POL_LEVEL;
    param->de_pol      = DVPO_DE_POL_LEVEL;
    param->vsync_dis   = DVPO_VSYNC_DIS_DIS;
    param->hsync_dis   = DVPO_HSYNC_DIS_DIS;
    param->de_dis      = DVPO_DE_DIS_DIS;
    param->dvpo_fmt    = DVPO_FMT_RGBA8888;
    param->hsync_vsb   = DVPO_HS_VSB_EN;
    param->de_vsb      = DVPO_DE_VSB_EN;

    return 0;
}


static void lcd_spi_init(dvpout_lcd_driver_t *dev_driver)
{
    spi_init(dev_driver->spi_dev, SPI_WORK_MODE_0, SPI_FF_STANDARD, 9);
}

static int lcd_spi_send_cmd(dvpout_lcd_driver_t *dev_driver, uint32_t cmd)
{
    int ret;
    uint16_t cmd_9bit = (0 << 8) | (cmd & 0xff);
    lcd_spi_init(dev_driver);

    ret = spi_send_data_normal(dev_driver->spi_dev, dev_driver->spi_cs,
            (const uint8_t *)&cmd_9bit, sizeof(cmd_9bit));
    CHECK_RET(ret);
    return ret;
}

static int lcd_spi_send_data(dvpout_lcd_driver_t *dev_driver, uint8_t data)
{
    int ret;
    uint16_t data_9bit = (1 << 8) | data;
    lcd_spi_init(dev_driver);
    ret = spi_send_data_normal(dev_driver->spi_dev, dev_driver->spi_cs, 
            (const uint8_t *)&data_9bit, sizeof(data_9bit));
    CHECK_RET(ret);
    return ret;
}

#if 0
static int lcd_spi_recv_data(dvpout_lcd_driver_t *dev_driver, uint32_t cmd, uint8_t *data, uint32_t count)
{
    int ret;
    uint32_t cmd_9bit  = (0 << 8) | cmd;
    uint16_t data_9bit;

    lcd_spi_init(dev_driver);
    ret = spi_receive_data_multiple(dev_driver->spi_dev, dev_driver->spi_cs, 
            &cmd_9bit, 1, (uint8_t *)&data_9bit, 2);
    CHECK_RET(ret);
    
    LOGD(__func__, "lcd_spi_recv_data: data_9bit is 0x%x.\n", data_9bit);
    if (count == 1) {
        *data = (data_9bit & 0xff);
    }
    else {
        // TODO
    }
    return ret;
}
#endif

static int lcd_send_cmd_data(dvpout_lcd_driver_t *dev_driver, uint32_t cmd, uint8_t *data, uint32_t count)
{
    int ret;
    uint8_t i;
    
    if (data == NULL) {
        return -1;
    }

    ret = lcd_spi_send_cmd(dev_driver, cmd);
    CHECK_RET(ret);
    for (i = 0; i < count; i++) {
        ret = lcd_spi_send_data(dev_driver, data[i]);
        CHECK_RET(ret);
    }
    return ret;
}

static void lcd_set_reset_pin(dvpout_lcd_driver_t *dev_driver, uint32_t value)
{
	int reset_pin = dev_driver->reset_pin;

    gpio_set_drive_mode(reset_pin, GPIO_DM_OUTPUT);
    if (value == 0) {
        gpio_set_pin(reset_pin, GPIO_PV_LOW);
    }
    else {
        gpio_set_pin(reset_pin, GPIO_PV_HIGH);
    }
}


static void lcd_set_backlight(dvpout_lcd_driver_t *dev_driver, uint32_t value)
{
	int backlight_pin = dev_driver->backlight_pin;

    gpio_set_drive_mode(backlight_pin, GPIO_DM_OUTPUT);
    if (value == 0) {
        gpio_set_pin(backlight_pin, GPIO_PV_LOW);
    }
    else {
        gpio_set_pin(backlight_pin, GPIO_PV_HIGH);
    }
}

static dvpout_lcd_driver_t dvpout_ili9806e = {
    .name                   = "dvpout_ili9806e",
    .context                = NULL,
    .backlight_pin          = GPIO_PIN2,
	.reset_pin              = GPIO_PIN5,

    .spi_dev                = SPI_DEVICE_1,
    .spi_cs                 = SPI_CHIP_SELECT_0,
    .spi_clk                = SYSCTL_CLOCK_SPI1,

    .init                   = ili9806e_init,
    .suspend                = ili9806e_suspend,
    .resume                 = ili9806e_resume,
    .set_backlight          = lcd_set_backlight,
    .set_reset_pin          = lcd_set_reset_pin,
    .get_interface_param    = ili9806e_get_params,
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_DISPLAY_DVPO,     dvpout_ili9806e,    0,      &dvpout_ili9806e,    NULL);
