#include "cis_og0va1b.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define SENSOR_ADDR_WR              (0xc0 >> 1) 

#define TAG                      "cis_og0va1b"

#define W_VGA                    (640)
#define H_VGA                    (480)

#define REG_DLY                  (0xffff)

// frame length is set by {0x380E, 0x380F}
// maximum exposure time is frame length - 14
#define MAX_EXPOSURE (532 - 14)


typedef struct _OG0VA1B_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
}OG0VA1B_REG_T;


static OG0VA1B_REG_T og0va1b_start_regs[] __ATTR_ALIGN__(32) = {
	{0x0100, 0x01},
};

static OG0VA1B_REG_T og0va1b_stop_regs[] __ATTR_ALIGN__(32) = {
    {0x0100, 0x00},
};

static const OG0VA1B_REG_T og0va1b_common[] __ATTR_ALIGN__(32) = {
    {0x0103, 0x01},
    {0x0302, 0x31},
    {0x0304, 0x01},
    {0x0305, 0xe0},
    {0x0306, 0x00},
    {0x0326, 0xd8},
    {0x3006, 0x02},
    {0x3022, 0x01},
    {0x3107, 0x40},
    {0x3216, 0x01},
    {0x3217, 0x00},
    {0x3218, 0xc0},
    {0x3219, 0x55},
    {0x3500, 0x00},
    {0x3501, 0x01},
    {0x3502, 0xfe},
    {0x3506, 0x01},
    {0x3507, 0x50},
    {0x3508, 0x01},
    {0x3509, 0x00},
    {0x350a, 0x01},
    {0x350b, 0x00},
    {0x350c, 0x00},
    {0x3541, 0x00},
    {0x3542, 0x40},
    {0x3605, 0x90},
    {0x3606, 0x41},
    {0x3612, 0x00},
    {0x3620, 0x08},
    {0x3630, 0x17},
    {0x3631, 0x99},
    {0x3639, 0x88},
    {0x3668, 0x00},
    {0x3674, 0x00},
    {0x3677, 0x3f},
    {0x368f, 0x06},
    {0x36a4, 0xf1},
    {0x36a5, 0x2d},
    {0x3706, 0x30},
    {0x370d, 0x72},
    {0x3713, 0x86},
    {0x3715, 0x03},
    {0x3716, 0x00},
    {0x376d, 0x24},
    {0x3770, 0x3a},
    {0x3778, 0x00},
    {0x37a8, 0x03},
    {0x37a9, 0x00},
    {0x37df, 0x7d},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x02},
    {0x3805, 0x8f},
    {0x3806, 0x01},
    {0x3807, 0xef},
    {0x3808, 0x02},
    {0x3809, 0x80},
    {0x380a, 0x01},
    {0x380b, 0xe0},
    {0x380c, 0x01},
    {0x380d, 0x78},
    {0x380e, 0x02},
    {0x380f, 0x0c},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3816, 0x00},
    {0x3817, 0x01},
    {0x3818, 0x00},
    {0x3819, 0x05},
    {0x3820, 0x40},
    {0x3821, 0x04},
    {0x3823, 0x00},
    {0x3826, 0x00},
    {0x3827, 0x00},
    {0x382b, 0x52},
    {0x384a, 0xa2},
    {0x3858, 0x00},
    {0x3859, 0x00},
    {0x3860, 0x00},
    {0x3861, 0x00},
    {0x3866, 0x0c},
    {0x3867, 0x07},
    {0x3884, 0x00},
    {0x3885, 0x08},
    {0x3893, 0x6c},
    {0x3898, 0x00},
    {0x389a, 0x04},
    {0x389b, 0x01},
    {0x389c, 0x0b},
    {0x389d, 0xdc},
    {0x389f, 0x08},
    {0x38a0, 0x00},
    {0x38a1, 0x00},
    {0x38b1, 0x04},
    {0x38b2, 0x00},
    {0x38b3, 0x08},
    {0x38c9, 0x02},
    {0x38d4, 0x06},
    {0x38d5, 0x5a},
    {0x38d6, 0x08},
    {0x38d7, 0x3a},
    {0x391e, 0x00},
    {0x391f, 0x00},
    {0x3920, 0xa5},
    {0x3921, 0x00},
    {0x3922, 0x00},
    {0x3923, 0x00},
    {0x3924, 0x05},
    {0x3925, 0x00},
    {0x3926, 0x00},
    {0x3927, 0x00},
    {0x3928, 0x1a},
    {0x3929, 0x01},
    {0x392a, 0xb4},
    {0x392b, 0x00},
    {0x392c, 0x10},
    {0x392f, 0x40},
    {0x3a06, 0x06},
    {0x3a07, 0x78},
    {0x3a08, 0x08},
    {0x3a09, 0x80},
    {0x3a52, 0x00},
    {0x3a53, 0x01},
    {0x3a54, 0x0c},
    {0x3a55, 0x04},
    {0x3a58, 0x0c},
    {0x3a59, 0x04},
    {0x4000, 0xcf},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x13},
    {0x400a, 0x02},
    {0x400b, 0x34},
    {0x4010, 0x71},
    {0x4042, 0xc3},
    {0x4306, 0x04},
    {0x4307, 0x12},
    {0x4500, 0x70},
    {0x4509, 0x00},
    {0x450b, 0x83},
    {0x4604, 0x68},
    {0x481b, 0x44},
    {0x481f, 0x30},
    {0x4823, 0x44},
    {0x4825, 0x35},
    {0x4837, 0x11},
    {0x4f00, 0x04},
    {0x4f10, 0x04},
    {0x4f21, 0x01},
    {0x4f22, 0x00},
    {0x4f23, 0x54},
    {0x4f24, 0x51},
    {0x4f25, 0x41},
    {0x5000, 0x3f},
    {0x5001, 0x80},
    {0x500a, 0x00},
    {0x5100, 0x00},
    {0x5111, 0x20},
};


static void og0va1b_busy_delay (int32_t ms)
{
    aiva_msleep(ms);
}

static int og0va1b_write_reg(int i2c_num, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];

    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 3);

    return ret;
}

static int og0va1b_read_reg(int i2c_num, uint16_t reg_addr, uint8_t *reg_val)
{
    int ret;

    uint8_t addr_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xff;
    addr_buf[1] = (reg_addr >> 0) & 0xff;
    
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, &addr_buf[0], 2);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_recv_data(i2c_num, SENSOR_ADDR_WR, 0, 0, reg_val, 1);
    
    return ret;
}


static int og0va1b_program_regs(
        int             i2c_num,
        const OG0VA1B_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = og0va1b_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                        i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else {
            og0va1b_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static int og0va1b_i2c_test(int i2c_num)
{
    uint8_t id_h, id_l;
    int ret;

    ret = og0va1b_read_reg(i2c_num, 0x300A, &id_h); 
    if (ret < 0) {
        LOGE(__func__, "og0va1b_read_reg failed!");
        return ret;
    }

    ret = og0va1b_read_reg(i2c_num, 0x300B, &id_l); 
    if (ret < 0) {
        LOGE(__func__, "og0va1b_read_reg failed!");
        return ret;
    }
    LOGD(TAG, "og0va1b_i2c_test: readed id is 0x%x, 0x%x", id_h, id_l);

    if ((id_h != 0xc7) && (id_l != 0x56)) {
        LOGE(TAG, "og0va1b_i2c_test: read id fail! expected 0xc7, 0x56");
        return -1;
    }
   
    LOGD(TAG, "og0va1b_i2c_test: read id success!");
    return 0;
}

static int og0va1b_init(const OG0VA1B_REG_T *reg_list, int cnt, 
                       int i2c_num)
{
    int ret;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50*1000);

    if (og0va1b_i2c_test(i2c_num) < 0) {
        return -1;
    }

    ret = og0va1b_program_regs(i2c_num, reg_list, cnt);
    LOGD(TAG, "og0va1b_init() common return: %d.", ret);
    
    return ret;
}

static int og0va1b_vga_init(int i2c_num)
{
    int ret;
    ret = og0va1b_init(og0va1b_common,
            AIVA_ARRAY_LEN(og0va1b_common),
            i2c_num
            );
    return ret;
}

static int cis_og0va1b_init(cis_dev_driver_t *dev_driver)
{
    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    if (og0va1b_vga_init(dev_driver->i2c_num) != 0) {
        LOGE(TAG, "og0va1b vga init fail\n");
        return -1;
    }
    return 0;
}

static int cis_og0va1b_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    OG0VA1B_REG_T        *reg_list;
    int                 cnt;

    reg_list = og0va1b_start_regs;
    cnt     = AIVA_ARRAY_LEN(og0va1b_start_regs);
    ret     = og0va1b_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "og0va1b_start failed!");
    }
    return ret;
}

static int cis_og0va1b_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    OG0VA1B_REG_T        *reg_list;
    int                 cnt;

    reg_list = og0va1b_stop_regs;
    cnt     = AIVA_ARRAY_LEN(og0va1b_stop_regs);
    ret     = og0va1b_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "og0va1b_stop failed!");
    }
    return ret;
}

static void cis_og0va1b_power_on(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    og0va1b_busy_delay(50);
    gpio_set_pin(pow_pin, GPIO_PV_HIGH);
    og0va1b_busy_delay(50);
    return;
}

static void cis_og0va1b_power_off(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    og0va1b_busy_delay(50);
    return;
}

static void cis_og0va1b_reset(cis_dev_driver_t *dev_driver)
{
    return;
}

static int cis_og0va1b_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq                  = 960;
    param->mipi_param.lane_num              = 1;
    param->mipi_param.vc_num                = 1;
    param->mipi_param.virtual_channels[0]   = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type             = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

static int cis_og0va1b_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    exp_param->min_again    = 1.0;
    exp_param->max_again    = 31;
    exp_param->step_again   = 1.0;

    exp_param->min_dgain    = 1.0;
    exp_param->max_dgain    = 15.0;
    exp_param->step_dgain   = 1.0;

    exp_param->min_itime    = 1.0;
    exp_param->max_itime    = MAX_EXPOSURE;
    exp_param->step_itime   = 1.0;
    return 0;
}

static int cis_og0va1b_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    float again = exp->again;
    float dgain = exp->dgain;
    float itime = exp->itime;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 350*1000);


    // exposure
    uint8_t itime_h;
    uint8_t itime_l;
    uint32_t exposure = (int)itime;
    if (exposure > MAX_EXPOSURE)
        exposure = MAX_EXPOSURE;
    itime_l = exposure & 0xff;
    itime_h = (exposure >> 8) & 0xff;
    og0va1b_write_reg(i2c_num, 0x3502, itime_l);
    og0va1b_write_reg(i2c_num, 0x3501, itime_h);


    // again
    uint8_t again_coarse = (int)again;
    if (again_coarse > 0x1f)
        again_coarse = 0x1f;
    og0va1b_write_reg(i2c_num, 0x3508, again_coarse);


    // dgain
    uint8_t dgain_coarse = (int)dgain;
    if (dgain_coarse > 0xf)
        dgain_coarse = 0xf;
    og0va1b_write_reg(i2c_num, 0x350a, dgain_coarse);

    return 0;
}

static int cis_og0va1b_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = W_VGA;
    param->height = H_VGA;
    param->framerate = 30;
    param->format = CIS_FORMAT_RGB_BAYER10;

    return 0;
}

static cis_dev_driver_t og0va1b_dev0 = {
    .name                   = "og0va1b_dev0",
    .i2c_num                = I2C_DEVICE_0,
    .power_pin              = GPIO_PIN0,
    .reset_pin              = GPIO_PIN0,
    .mclk_id                = CIS_MCLK_ID_MCLK0,
    .context                = NULL,
    .init                   = cis_og0va1b_init,
    .start_stream           = cis_og0va1b_start_stream,
    .stop_stream            = cis_og0va1b_stop_stream,
    .power_on               = cis_og0va1b_power_on,
    .power_off              = cis_og0va1b_power_off,
    .reset                  = cis_og0va1b_reset,
    .get_interface_param    = cis_og0va1b_get_interface_param,
    .get_exposure_param     = cis_og0va1b_get_exposure_param,
    .set_exposure           = cis_og0va1b_set_exposure,
    .get_frame_parameter         = cis_og0va1b_get_frame_parameter
};

static cis_dev_driver_t og0va1b_dev1 = {
    .name                   = "og0va1b_dev1",
    .i2c_num                = I2C_DEVICE_1,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_og0va1b_init,
    .start_stream           = cis_og0va1b_start_stream,
    .stop_stream            = cis_og0va1b_stop_stream,
    .power_on               = cis_og0va1b_power_on,
    .power_off              = cis_og0va1b_power_off,
    .reset                  = cis_og0va1b_reset,
    .get_interface_param    = cis_og0va1b_get_interface_param,
    .get_exposure_param     = cis_og0va1b_get_exposure_param,
    .set_exposure           = cis_og0va1b_set_exposure,
    .get_frame_parameter         = cis_og0va1b_get_frame_parameter
};

static cis_dev_driver_t og0va1b_dev2 = {
    .name                   = "og0va1b_dev2",
    .i2c_num                = I2C_DEVICE_2,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_og0va1b_init,
    .start_stream           = cis_og0va1b_start_stream,
    .stop_stream            = cis_og0va1b_stop_stream,
    .power_on               = cis_og0va1b_power_on,
    .power_off              = cis_og0va1b_power_off,
    .reset                  = cis_og0va1b_reset,
    .get_interface_param    = cis_og0va1b_get_interface_param,
    .get_exposure_param     = cis_og0va1b_get_exposure_param,
    .set_exposure           = cis_og0va1b_set_exposure,
    .get_frame_parameter         = cis_og0va1b_get_frame_parameter
};

cis_dev_driver_t *cis_og0va1b[] = {
    &og0va1b_dev0,
    &og0va1b_dev1,
    &og0va1b_dev2
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og0va1b_dev0,    0,      &og0va1b_dev0,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og0va1b_dev1,    0,      &og0va1b_dev1,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og0va1b_dev2,    0,      &og0va1b_dev2,    NULL);
