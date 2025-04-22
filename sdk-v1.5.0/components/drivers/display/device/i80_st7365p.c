#include <stdio.h>
#include "i80_ctl.h"
#include "i80_lcd_driver.h"
#include "syslog.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "gpio.h"
#include "udevice.h"

static int g_width          = 320;
static int g_height         = 480;


static int i80_st7365p_get_params(i80_lcd_driver_t *dev_driver, display_i80_params_t *param)
{
    (void)dev_driver;
    param->width   = g_width;
    param->height  = g_height;
    param->out_fmt = I80_OUT_FMT_8BIT_RGB565;

    return 0;
}

static int i80_st7365p_init(i80_lcd_driver_t *dev_driver)
{
    uint32_t cmd;
    uint16_t cmd_param[16];
    uint16_t cmd_result[16];

    dev_driver->set_reset_pin(dev_driver, 1);
    aiva_msleep(150);
    dev_driver->set_reset_pin(dev_driver, 0);
    aiva_msleep(150);
    dev_driver->set_reset_pin(dev_driver, 1);
    aiva_msleep(150);

    i80_ctl_reset_device();
    aiva_msleep(150);

    cmd             = 0x11;     //Exit Sleep
    i80_ctl_send_wr_cmd(cmd, NULL, 0);
    aiva_msleep(120);

    cmd = 0xD3;                 //read display identification information
    i80_ctl_send_rd_cmd(cmd, &cmd_result[0], 4);
    LOGI(__func__, "DISPLAY ID: 0x%x, 0x%x, 0x%x, 0x%x\n", (int)cmd_result[0], (int)cmd_result[1], (int)cmd_result[2], (int)cmd_result[3]);

    cmd             = 0xF0;
    cmd_param[0]    = 0xC3;     // enable command 2 part I.
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xF0;
    cmd_param[0]    = 0x96;     // enable command 2 part II.
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    /**
     * [4]:0,top->Bottom, 1,Bottom->top; [3]:0, RGB, 1, BGR;[2]:0,left->right, 1,right->left;
    */
    cmd             = 0x36;
    cmd_param[0]    = 0x40|0x08;     // Memory Data Access Control. RGB
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0x3A;     // Interface Pixel Format.
    cmd_param[0]    = 0x55;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xE8;     // Display Output Ctrl Adjust
    cmd_param[0]    = 0x40;
    cmd_param[1]    = 0x82;
    cmd_param[2]    = 0x07;
    cmd_param[3]    = 0x18;
    cmd_param[4]    = 0x27;
    cmd_param[5]    = 0x0A;
    cmd_param[6]    = 0xB6;
    cmd_param[7]    = 0x33;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 8);

    cmd             = 0xC5;     // VCOM Control
    cmd_param[0]    = 0x27;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xC2;     // Power Control 3
    cmd_param[0]    = 0xA7;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xB4;     // Display Inversion Control
    cmd_param[0]    = 0x01;     // 1-dot inversion
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xB6;     // Display Function Control 
    cmd_param[0]    = 0x0A;
    cmd_param[0]    = 0xA2;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 2);

    cmd             = 0xB7;     //  Entry Mode Set.
    cmd_param[0]    = 0xC6;     
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xC0;     // Power Control 1
    cmd_param[0]    = 0x80;
    cmd_param[1]    = 0x45;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 2);

    cmd             = 0xC1;     // Power Control 2
    cmd_param[0]    = 0x0F;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0x26; //Gamma curve selected
    cmd_param[0]    = 0x00;
    cmd_param[1]    = 0x01;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xE0;     // Positive Gamma Control.
    cmd_param[0]    = 0xf0;
    cmd_param[1]    = 0x01;
    cmd_param[2]    = 0x06;
    cmd_param[3]    = 0x0f;
    cmd_param[4]    = 0x12;
    cmd_param[5]    = 0x1D;
    cmd_param[6]    = 0x36;
    cmd_param[7]    = 0x54;
    cmd_param[8]    = 0x44;
    cmd_param[9]    = 0x0C;
    cmd_param[10]   = 0x18;
    cmd_param[11]   = 0x16;
    cmd_param[12]   = 0x13;
    cmd_param[13]   = 0x15;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 14);

    cmd             = 0xE1;     //  Negative Gamma Control.
    cmd_param[0]    = 0xf0;
    cmd_param[1]    = 0x01;
    cmd_param[2]    = 0x05;
    cmd_param[3]    = 0x0A;
    cmd_param[4]    = 0x0B;
    cmd_param[5]    = 0x07;
    cmd_param[6]    = 0x32;
    cmd_param[7]    = 0x44;
    cmd_param[8]    = 0x44;
    cmd_param[9]    = 0x0C;
    cmd_param[10]   = 0x18;
    cmd_param[11]   = 0x17;
    cmd_param[12]   = 0x13;
    cmd_param[13]   = 0x16;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 14);

    cmd             = 0xF0;     //  Command Set Control
    cmd_param[0]    = 0x3C;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    cmd             = 0xF0;     //  Command Set Control
    cmd_param[0]    = 0x69;
    i80_ctl_send_wr_cmd(cmd, &cmd_param[0], 1);

    aiva_busy_delay_ms(150);

    cmd             = 0x21;     // Display Inversion On
    i80_ctl_send_wr_cmd(cmd, NULL, 0);

    cmd             = 0x29;     // Display On
    i80_ctl_send_wr_cmd(cmd, NULL, 0);

    //TODO: Light on backlight

    return 0;
}

static int i80_st7365p_suspend(i80_lcd_driver_t *dev_driver)
{
    dev_driver->set_backlight(dev_driver, 0);

    return 0;
}

static int i80_st7365p_resume(i80_lcd_driver_t *dev_driver)
{
    dev_driver->set_backlight(dev_driver, 1);

    return 0;
}

static void i80_st7365p_set_reset_pin(i80_lcd_driver_t *dev_driver, uint32_t value)
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

static void i80_st7365p_set_backlight(i80_lcd_driver_t *dev_driver, uint32_t value)
{
	int backlight_pin = dev_driver->backlight_pin;

    if (value > 0) {
        gpio_set_drive_mode(backlight_pin, GPIO_DM_OUTPUT);
        gpio_set_pin(backlight_pin, GPIO_PV_LOW);
    } else {
        gpio_set_drive_mode(backlight_pin, GPIO_DM_OUTPUT);
        gpio_set_pin(backlight_pin, GPIO_PV_HIGH);
    }
}


static i80_lcd_driver_t i80_st7365p = {
    .name                   = "i80_st7365p",
    .context                = NULL,
    .backlight_pin          = GPIO_PIN2,
    .reset_pin              = GPIO_PIN2,

    .init                   = i80_st7365p_init,
    .suspend                = i80_st7365p_suspend,
    .resume                 = i80_st7365p_resume,
    .set_backlight          = i80_st7365p_set_backlight,
    .set_reset_pin          = i80_st7365p_set_reset_pin,

    .get_interface_param    = i80_st7365p_get_params,
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_DISPLAY_I80,     i80_st7365p,    0,      &i80_st7365p,    NULL);

