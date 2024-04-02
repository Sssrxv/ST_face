#include "cis_ov02k10.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define SENSOR_ADDR_WR              (0x6c >> 1) 

#define TAG                      "cis_ov02k10"

#define W_VGA                    (640)
#define H_VGA                    (480)

#define W_1920                   (1920)
#define H_1080                   (1080)

#define REG_DLY                  (0xffff)

typedef struct _OV02k10_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
}OV02k10_REG_T;


static OV02k10_REG_T ov02k10_start_regs[] __ATTR_ALIGN__(32) = {
	{0x0100, 0x01},
};

static OV02k10_REG_T ov02k10_stop_regs[] __ATTR_ALIGN__(32) = {
    {0x0100, 0x00},
};

static const OV02k10_REG_T ov02k10_common[] __ATTR_ALIGN__(32) = {
    {0x302a, 0x00},
    {0x0103, 0x01},
    {0x0109, 0x01},
    {0x0104, 0x02},
    {0x0102, 0x00},
    {0x0305, 0x5c},
    {0x0306, 0x00},
    {0x0307, 0x00},
    {0x030a, 0x01},
    {0x0317, 0x09},
    {0x0323, 0x07},
    {0x0324, 0x01},
    {0x0325, 0xb0},
    {0x0327, 0x07},
    {0x032c, 0x02},
    {0x032d, 0x02},
    {0x032e, 0x05},
    {0x300f, 0x11},
    {0x3012, 0x11},
    {0x3026, 0x10},
    {0x3027, 0x08},
    {0x302d, 0x24},
    {0x3103, 0x29},
    {0x3106, 0x12},
    {0x3400, 0x00},
    {0x3406, 0x08},
    {0x3408, 0x05},
    {0x340c, 0x05},
    {0x3425, 0x51},
    {0x3426, 0x10},
    {0x3427, 0x14},
    {0x3428, 0x10},
    {0x3429, 0x10},
    {0x342a, 0x10},
    {0x342b, 0x04},
    {0x3504, 0x08},
    //{0x3508, 0x01},
    {0x3508, 0x03}, // L real gain
    {0x3509, 0x00},
    {0x3544, 0x08},
    //{0x3548, 0x01}, 
    {0x3548, 0x03}, // S real gain   
    {0x3549, 0x00},
    {0x3584, 0x08},
    //{0x3588, 0x01},
    {0x3588, 0x03}, // VS real gain
    {0x3589, 0x00},
    {0x3601, 0x70},
    {0x3604, 0xe3},
    {0x3605, 0x7f},
    {0x3606, 0x00},
    {0x3608, 0xa8},
    {0x360a, 0xd0},
    {0x360b, 0x08},
    {0x360e, 0xc8},
    {0x360f, 0x66},
    {0x3610, 0x81},
    {0x3611, 0x89},
    {0x3612, 0x4e},
    {0x3613, 0xbd},
    {0x362a, 0x0e},
    {0x362b, 0x0e},
    {0x362c, 0x0e},
    {0x362d, 0x0e},
    {0x362e, 0x0c},
    {0x362f, 0x1a},
    {0x3630, 0x32},
    {0x3631, 0x64},
    {0x3638, 0x00},
    {0x3643, 0x00},
    {0x3644, 0x00},
    {0x3645, 0x00},
    {0x3646, 0x00},
    {0x3647, 0x00},
    {0x3648, 0x00},
    {0x3649, 0x00},
    {0x364a, 0x04},
    {0x364c, 0x0e},
    {0x364d, 0x0e},
    {0x364e, 0x0e},
    {0x364f, 0x0e},
    {0x3650, 0xff},
    {0x3651, 0xff},
    {0x3661, 0x07},
    {0x3662, 0x02},
    {0x3663, 0x20},
    {0x3665, 0x12},
    {0x3667, 0xd4},
    {0x3668, 0x80},
    {0x366f, 0x00},
    {0x3670, 0xc7},
    {0x3671, 0x08},
    {0x3673, 0x2a},
    {0x3681, 0x80},
    {0x3700, 0x26},
    {0x3701, 0x1e},
    {0x3702, 0x25},
    {0x3703, 0x28},
    {0x3706, 0x3e},
    {0x3707, 0x0a},
    {0x3708, 0x36},
    {0x3709, 0x55},
    {0x370a, 0x00},
    {0x370b, 0xa3},
    {0x3714, 0x01},
    {0x371b, 0x16},
    {0x371c, 0x00},
    {0x371d, 0x08},
    {0x3756, 0x9b},
    {0x3757, 0x9b},
    {0x3762, 0x1d},
    {0x376c, 0x10},
    {0x3776, 0x05},
    {0x3777, 0x22},
    {0x3779, 0x60},
    {0x377c, 0x48},
    {0x3783, 0x02},
    {0x3784, 0x06},
    {0x3785, 0x0a},
    {0x3790, 0x10},
    {0x3793, 0x04},
    {0x3794, 0x07},
    {0x3796, 0x00},
    {0x3797, 0x02},
    {0x379c, 0x4d},
    {0x37a1, 0x80},
    {0x37bb, 0x88},
    {0x37be, 0x01},
    {0x37bf, 0x00},
    {0x37c0, 0x01},
    {0x37c7, 0x56},
    {0x37ca, 0x21},
    {0x37cc, 0x13},
    {0x37cd, 0x90},
    {0x37cf, 0x02},
    {0x37d1, 0x3e},
    {0x37d2, 0x00},
    {0x37d3, 0xa3},
    {0x37d5, 0x3e},
    {0x37d6, 0x00},
    {0x37d7, 0xa3},
    {0x37d8, 0x01},
    {0x37da, 0x00},
    {0x37db, 0x00},
    {0x37dc, 0x00},
    {0x37dd, 0x00},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x04},
    {0x3804, 0x07},
    {0x3805, 0x8f},
    {0x3806, 0x04},
    {0x3807, 0x43},
    {0x3808, 0x07},
    {0x3809, 0x80},
    {0x380a, 0x04},
    {0x380b, 0x38},
    {0x380c, 0x08},
    {0x380d, 0x70},
    {0x380e, 0x04},
    {0x380f, 0xe2},
    {0x3811, 0x08},
    {0x3813, 0x04},
    {0x3814, 0x01},
    {0x3815, 0x01},
    {0x3816, 0x01},
    {0x3817, 0x01},
    {0x381c, 0x00},
    {0x3820, 0x02},
    {0x3821, 0x00},
    {0x3822, 0x14},
    {0x384c, 0x08},
    {0x384d, 0x70},
    {0x3858, 0x0d},
    {0x3865, 0x00},
    {0x3866, 0xc0},
    {0x3867, 0x00},
    {0x3868, 0xc0},
    {0x3900, 0x13},
    {0x3940, 0x13},
    {0x3980, 0x13},
    {0x3c01, 0x11},
    {0x3c05, 0x00},
    {0x3c0f, 0x1c},
    {0x3c12, 0x0d},
    {0x3c14, 0x21},
    {0x3c19, 0x01},
    {0x3c21, 0x40},
    {0x3c3b, 0x18},
    {0x3c3d, 0xc9},
    {0x3c55, 0x08},
    {0x3c5d, 0xcf},
    {0x3c5e, 0xcf},
    {0x3ce0, 0x00},
    {0x3ce1, 0x00},
    {0x3ce2, 0x00},
    {0x3ce3, 0x00},
    {0x3d8c, 0x70},
    {0x3d8d, 0x10},
    {0x4001, 0x2f},
    {0x4033, 0x80},
    {0x4008, 0x02},
    {0x4009, 0x11},
    {0x4004, 0x00},
    {0x4005, 0x40},
    {0x400a, 0x01},
    {0x400b, 0x3c},
    {0x400e, 0x40},
    {0x4011, 0xbb},
    {0x410f, 0x01},
    {0x402e, 0x00},
    {0x402f, 0x40},
    {0x4030, 0x00},
    {0x4031, 0x40},
    {0x4032, 0x0f},
    {0x4050, 0x00},
    {0x4051, 0x07},
    {0x4288, 0xcf},
    {0x4289, 0x03},
    {0x428a, 0x46},
    {0x430b, 0x0f},
    {0x430c, 0xfc},
    {0x430d, 0x00},
    {0x430e, 0x00},
    {0x4310, 0xff},
    {0x4311, 0xff},
    {0x4312, 0x00},
    {0x4313, 0x00},
    {0x4314, 0x04},
    {0x4500, 0x18},
    {0x4501, 0x18},
    {0x4504, 0x00},
    {0x4507, 0x02},
    {0x4508, 0x1a},
    {0x4603, 0x00},
    {0x4640, 0x62},
    {0x4646, 0xaa},
    {0x4647, 0x55},
    {0x4648, 0x99},
    {0x4649, 0x66},
    {0x464a, 0x38},
    {0x464d, 0x00},
    {0x4654, 0x11},
    {0x4655, 0x22},
    {0x4680, 0x62},
    {0x4686, 0xaa},
    {0x4687, 0x55},
    {0x4688, 0x99},
    {0x4689, 0x66},
    {0x468a, 0x38},
    {0x468d, 0x00},
    {0x46c0, 0x62},
    {0x46c6, 0xaa},
    {0x46c7, 0x55},
    {0x46c8, 0x99},
    {0x46c9, 0x66},
    {0x46ca, 0x38},
    {0x46cd, 0x00},
    {0x4800, 0x04},
    {0x4810, 0xff},
    {0x4811, 0xff},
    {0x480e, 0x00},
    {0x4813, 0x00},
    {0x4837, 0x0e},
    {0x484b, 0x27},
    {0x4d00, 0x4e},
    {0x4d01, 0x0c},
    {0x4d09, 0x4f},
    {0x5000, 0x1f},
    {0x5080, 0x00},
    {0x50c0, 0x00},
    {0x5100, 0x00},
    {0x5200, 0x00},
    {0x5201, 0x70},
    {0x5202, 0x03},
    {0x5203, 0x7f},
    {0x5780, 0x53},
    {0x5786, 0x01},
    {0x0305, 0x50},
    {0x4837, 0x10},
    {0x380c, 0x0c},
    {0x380d, 0x80},
    {0x384c, 0x0c},
    {0x384d, 0x80},
    {0x380e, 0x04},
    {0x380f, 0x65},
    {0x3501, 0x04},
    {0x3502, 0x55},
    //{0x0100, 0x01},
};

static const OV02k10_REG_T ov02k10_1920x1080[] __ATTR_ALIGN__(32) = {
};


static const OV02k10_REG_T ov02k10_vga[] __ATTR_ALIGN__(32) = {
};


static void ov02k10_busy_delay (int32_t ms)
{
    aiva_msleep(ms);
}

static int ov02k10_write_reg(int i2c_num, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];

    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 3);

    return ret;
}

static int ov02k10_read_reg(int i2c_num, uint16_t reg_addr, uint8_t *reg_val)
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


static int ov02k10_program_regs(
        int             i2c_num,
        const OV02k10_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = ov02k10_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                        i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else {
            ov02k10_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static int ov02k10_i2c_test(int i2c_num)
{
    uint8_t ver;
    uint8_t val;

    ov02k10_read_reg(i2c_num, 0x302A, &ver); 
    LOGD(TAG, "ov02k10_i2c_test: revision is 0x%x.", ver);

    ov02k10_write_reg(i2c_num, 0x50C0, 0x80);
    ov02k10_read_reg(i2c_num,  0x50C0, &val);
    if (val != 0x80) {
        LOGE(TAG, "ov02k10_i2c_test: read/write reg(0x50C0) fail!, get 0x%x, expected 0x80", val);
        return -1;
    }
   
    ov02k10_write_reg(i2c_num, 0x50C0, 0x00);

    LOGD(TAG, "ov02k10_i2c_test: read/write reg(0x50C0) success!");
    return 0;
}

static int ov02k10_init(const OV02k10_REG_T *reg_list, int cnt, 
                       const OV02k10_REG_T *reg_list_spec, int cnt_spec, 
                       int i2c_num)
{
    int ret;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50*1000);

    if (ov02k10_i2c_test(i2c_num) < 0) {
        return -1;
    }

    ret = ov02k10_program_regs(i2c_num, reg_list, cnt);
    LOGD(TAG, "ov02k10_init() common return: %d.", ret);
    
    ret = ov02k10_program_regs(i2c_num, reg_list_spec, cnt_spec);
    LOGD(TAG, "ov02k10_init() spec return: %d.", ret);

    return ret;
}

static int ov02k10_1920x1080_init(int i2c_num)
{
    int ret;
    ret = ov02k10_init(ov02k10_common,
            AIVA_ARRAY_LEN(ov02k10_common),
            ov02k10_1920x1080,
            AIVA_ARRAY_LEN(ov02k10_1920x1080),
            i2c_num
            );
    return ret;
}

static int cis_ov02k10_init(cis_dev_driver_t *dev_driver)
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

    if (ov02k10_1920x1080_init(dev_driver->i2c_num) != 0) {
        LOGE(TAG, "ov02k10 1920x1080 init fail\n");
        return -1;
    }
    return 0;
}

static int cis_ov02k10_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    OV02k10_REG_T        *reg_list;
    int                 cnt;

    reg_list = ov02k10_start_regs;
    cnt     = AIVA_ARRAY_LEN(ov02k10_start_regs);
    ret     = ov02k10_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "ov02k10_start failed!");
    }
    return ret;
}

static int cis_ov02k10_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    OV02k10_REG_T        *reg_list;
    int                 cnt;

    reg_list = ov02k10_stop_regs;
    cnt     = AIVA_ARRAY_LEN(ov02k10_stop_regs);
    ret     = ov02k10_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "ov02k10_stop failed!");
    }
    return ret;
}

static void cis_ov02k10_power_on(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    ov02k10_busy_delay(50);
    gpio_set_pin(pow_pin, GPIO_PV_HIGH);
    ov02k10_busy_delay(50);
    return;
}

static void cis_ov02k10_power_off(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    ov02k10_busy_delay(50);
    return;
}

static void cis_ov02k10_reset(cis_dev_driver_t *dev_driver)
{
    return;
}

static int cis_ov02k10_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq                  = 800;
    param->mipi_param.lane_num              = 1;
    param->mipi_param.vc_num                = 1;
    param->mipi_param.virtual_channels[0]   = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type             = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

static int cis_ov02k10_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{

    return 0;
}

static int cis_ov02k10_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50*1000);

    return 0;
}

static int cis_ov02k10_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = W_1920;
    param->height = H_1080;
    param->framerate = 30;
    param->format = CIS_FORMAT_RGB_BAYER10;

    return 0;
}

static cis_dev_driver_t ov02k10_dev0 = {
    .name                   = "ov02k10_dev0",
    .i2c_num                = I2C_DEVICE_0,
    .power_pin              = GPIO_PIN0,
    .reset_pin              = GPIO_PIN0,
    .mclk_id                = CIS_MCLK_ID_MCLK0,
    .context                = NULL,
    .init                   = cis_ov02k10_init,
    .start_stream           = cis_ov02k10_start_stream,
    .stop_stream            = cis_ov02k10_stop_stream,
    .power_on               = cis_ov02k10_power_on,
    .power_off              = cis_ov02k10_power_off,
    .reset                  = cis_ov02k10_reset,
    .get_interface_param    = cis_ov02k10_get_interface_param,
    .get_exposure_param     = cis_ov02k10_get_exposure_param,
    .set_exposure           = cis_ov02k10_set_exposure,
    .get_frame_parameter    = cis_ov02k10_get_frame_parameter
};

static cis_dev_driver_t ov02k10_dev1 = {
    .name                   = "ov02k10_dev1",
    .i2c_num                = I2C_DEVICE_1,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_ov02k10_init,
    .start_stream           = cis_ov02k10_start_stream,
    .stop_stream            = cis_ov02k10_stop_stream,
    .power_on               = cis_ov02k10_power_on,
    .power_off              = cis_ov02k10_power_off,
    .reset                  = cis_ov02k10_reset,
    .get_interface_param    = cis_ov02k10_get_interface_param,
    .get_exposure_param     = cis_ov02k10_get_exposure_param,
    .set_exposure           = cis_ov02k10_set_exposure,
    .get_frame_parameter    = cis_ov02k10_get_frame_parameter
};

static cis_dev_driver_t ov02k10_dev2 = {
    .name                   = "ov02k10_dev2",
    .i2c_num                = I2C_DEVICE_2,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_ov02k10_init,
    .start_stream           = cis_ov02k10_start_stream,
    .stop_stream            = cis_ov02k10_stop_stream,
    .power_on               = cis_ov02k10_power_on,
    .power_off              = cis_ov02k10_power_off,
    .reset                  = cis_ov02k10_reset,
    .get_interface_param    = cis_ov02k10_get_interface_param,
    .get_exposure_param     = cis_ov02k10_get_exposure_param,
    .set_exposure           = cis_ov02k10_set_exposure,
    .get_frame_parameter    = cis_ov02k10_get_frame_parameter
};

cis_dev_driver_t *cis_ov02k10[] = {
    &ov02k10_dev0,
    &ov02k10_dev1,
    &ov02k10_dev2
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     ov02k10_dev0,    0,      &ov02k10_dev0,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     ov02k10_dev1,    0,      &ov02k10_dev1,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     ov02k10_dev2,    0,      &ov02k10_dev2,    NULL);
