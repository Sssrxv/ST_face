#include <math.h>
#include "cis_gc2053.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define REG_DLY (0xff)
#define SENSOR_ADDR_WR (0x6f >> 1)

typedef struct _GC2053_REG_T
{
    uint8_t reg_addr;
    uint8_t data;
} GC2053_REG_T;

static const char *TAG = "cis_gc2053";

static GC2053_REG_T gc2053_start_regs[] __ATTR_ALIGN__(32) = {

};

static GC2053_REG_T gc2053_stop_regs[] __ATTR_ALIGN__(32) = {

};

// mclk 24Mhz
// window size 1920*1080
// mipiclk 696Mbps/lane
// framelength 1125
// rowtime 29.42529us
// pattern rggb
static const GC2053_REG_T gc2053_1080p[] __ATTR_ALIGN__(32) = {
    {0xfe, 0x80},
    {0xfe, 0x80},
    {0xfe, 0x80},
    {0xfe, 0x00},
    {0xf2, 0x00},
    {0xf3, 0x00},
    {0xf4, 0x36},
    {0xf5, 0xc0},
    {0xf6, 0x44},
    {0xf7, 0x01},
    {0xf8, 0x3a},
    {0xf9, 0x42},
    {0xfc, 0x8e},
    {0xfe, 0x00},
    {0x87, 0x18},
    {0xee, 0x30},
    {0xd0, 0xb7},
    {0x03, 0x04},
    {0x04, 0x10},
    {0x05, 0x05},
    {0x06, 0x00},
    {0x07, 0x00},
    {0x08, 0x11},
    {0x09, 0x00},
    {0x0a, 0x02},
    {0x0b, 0x00},
    {0x0c, 0x02},
    {0x0d, 0x04},
    {0x0e, 0x40},
    {0x12, 0xe2},
    {0x13, 0x12},
    {0x19, 0x0a},
    {0x21, 0x1c},
    {0x28, 0x0a},
    {0x29, 0x24},
    {0x2b, 0x04},
    {0x32, 0xf8},
    {0x37, 0x03},
    {0x39, 0x15},
    {0x43, 0x07},
    {0x44, 0x40},
    {0x46, 0x0b},
    {0x4b, 0x20},
    {0x4e, 0x08},
    {0x55, 0x20},
    {0x66, 0x05},
    {0x67, 0x05},
    {0x77, 0x01},
    {0x78, 0x00},
    {0x7c, 0x93},
    {0x8c, 0x12},
    {0x8d, 0x92},
    {0x90, 0x00},
    {0x41, 0x05},
    // {0x42, 0x46},
    {0x42, 0x65},   // change frame length, set 25 fps to fix flicker
    {0x9d, 0x10},
    {0xce, 0x7c},
    {0xd2, 0x41},
    {0xd3, 0xdc},
    {0xe6, 0x50},
    {0xb6, 0xc0},
    {0xb0, 0x70},
    {0xb1, 0x01},
    {0xb2, 0x00},
    {0xb3, 0x00},
    {0xb4, 0x00},
    {0xb8, 0x01},
    {0xb9, 0x00},
    {0x26, 0x30},
    {0xfe, 0x01},
    {0x40, 0x23},
    {0x55, 0x07},
    {0x60, 0x40},
    {0xfe, 0x04},
    {0x14, 0x78},
    {0x15, 0x78},
    {0x16, 0x78},
    {0x17, 0x78},
    {0xfe, 0x01},
    {0x92, 0x00},
    {0x94, 0x03},
    {0x95, 0x04},
    {0x96, 0x38},
    {0x97, 0x07},
    {0x98, 0x80},
    {0xfe, 0x01},
    {0x01, 0x05},
    {0x02, 0x89},
    {0x04, 0x01},
    {0x07, 0xa6},
    {0x08, 0xa9},
    {0x09, 0xa8},
    {0x0a, 0xa7},
    {0x0b, 0xff},
    {0x0c, 0xff},
    {0x0f, 0x00},
    {0x50, 0x1c},
    {0x89, 0x03},
    {0xfe, 0x04},
    {0x28, 0x86},
    {0x29, 0x86},
    {0x2a, 0x86},
    {0x2b, 0x68},
    {0x2c, 0x68},
    {0x2d, 0x68},
    {0x2e, 0x68},
    {0x2f, 0x68},
    {0x30, 0x4f},
    {0x31, 0x68},
    {0x32, 0x67},
    {0x33, 0x66},
    {0x34, 0x66},
    {0x35, 0x66},
    {0x36, 0x66},
    {0x37, 0x66},
    {0x38, 0x62},
    {0x39, 0x62},
    {0x3a, 0x62},
    {0x3b, 0x62},
    {0x3c, 0x62},
    {0x3d, 0x62},
    {0x3e, 0x62},
    {0x3f, 0x62},
    {0xfe, 0x01},
    {0x9a, 0x06},
    {0xfe, 0x00},
    {0x7b, 0x2a},
    {0x23, 0x2d},
    {0xfe, 0x03},
    {0x01, 0x27},
    {0x02, 0x56},

    //    {0x03, 0xb6},
    {0x03, 0x8e},
    //    {0x03, 0xce},

    {0x12, 0x80},
    {0x13, 0x07},
    {0x15, 0x10},
    {0xfe, 0x00},
    {0x3e, 0x90},
};

static void gc2053_busy_delay(int32_t ms)
{
    aiva_msleep(ms);
}

static int gc2053_write_reg(int i2c_num, uint32_t i2c_tar_addr, uint8_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[2];
    data_buf[0] = reg_addr;
    data_buf[1] = reg_val;
    ret = i2c_send_data(i2c_num, i2c_tar_addr, data_buf, 2);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, data_buf, 2);*/
    return ret;
}

static int gc2053_read_reg(int i2c_num, uint32_t i2c_tar_addr, uint8_t reg_addr, uint8_t *reg_val)
{
    int ret;

    ret = i2c_send_data(i2c_num, i2c_tar_addr, &reg_addr, 1);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, &reg_addr, 1);*/
    if (ret < 0)
    {
        return ret;
    }
    ret = i2c_recv_data(i2c_num, i2c_tar_addr, 0, 0, reg_val, 1);
    /*ret = i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, i2c_num, 0, 0, reg_val, 1);*/

    return ret;
}

static int gc2053_program_regs(
    int i2c_num,
    uint32_t i2c_tar_addr,
    const GC2053_REG_T *reg_list,
    int cnt)
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++)
    {
        if (reg_list[i].reg_addr != REG_DLY)
        {
            ret = gc2053_write_reg(i2c_num, i2c_tar_addr, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0)
            {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                     i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else
        {
            gc2053_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static uint16_t gc2053_get_sensor_id(i2c_device_number_t i2c_num, uint32_t i2c_tar_addr)
{
    uint8_t id_h = 0;
    uint8_t id_l = 0;

    gc2053_read_reg(i2c_num, i2c_tar_addr, 0xf0, &id_h);
    gc2053_read_reg(i2c_num, i2c_tar_addr, 0xf1, &id_l);

    return ((id_h << 8) | id_l);
}

static int gc2053_i2c_test(int i2c_num, uint32_t i2c_tar_addr)
{
    uint16_t val;

    val = gc2053_get_sensor_id(i2c_num, i2c_tar_addr);
    LOGD(TAG, "gc2053 readed id is 0x%4x", val);

    if (val != 0x2053)
    {
        LOGE(TAG, "gc2053_i2c_test: read id fail!, i2c:%d get 0x%4x, expected 0x2053", i2c_num, val);
        return -1;
    }

    LOGD(TAG, "gc2053_i2c_test: read id success!");
    return 0;
}

static int gc2053_init(const GC2053_REG_T *reg_list, int cnt, int i2c_num, uint32_t i2c_tar_addr)
{
    int ret;

    i2c_init(i2c_num, i2c_tar_addr, 7, 350 * 1000);

    if (gc2053_i2c_test(i2c_num, i2c_tar_addr) < 0)
    {
        LOGE(TAG, "gc2053 i2c test fail\n");
        return -1;
    }

    ret = gc2053_program_regs(i2c_num, i2c_tar_addr, reg_list, cnt);

    /*LOGD(TAG, "gc2053_init() return: %d.", ret);*/
    if (ret < 0)
    {
        LOGE(__func__, "line %d, gc2053_program_regs failed!", __LINE__);
        return ret;
    }
    return ret;
}

static int gc2053_1080p_init(int i2c_num, uint32_t i2c_tar_addr)
{
    int ret;
    ret = gc2053_init(gc2053_1080p, AIVA_ARRAY_LEN(gc2053_1080p), i2c_num, i2c_tar_addr);
    return ret;
}

static int cis_gc2053_init(cis_dev_driver_t *dev_driver)
{
    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0)
    {
        sysctl_set_sens_mclk(MCLK_ID0, dev_driver->mclk_freq);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    }
    else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1)
    {
        sysctl_set_sens_mclk(MCLK_ID1, dev_driver->mclk_freq);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    }
    else
    {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    // LOGI(__func__, "dev_driver->i2c_num:%d dev_driver->i2c_tar_addr:0x%02x, dev_driver->mclk_id:%d, dev_driver->mclk_freq:%d, dev_driver->power_pin:%d",
    // dev_driver->i2c_num, dev_driver->i2c_tar_addr, dev_driver->mclk_id, dev_driver->mclk_freq, dev_driver->power_pin);
    if (gc2053_1080p_init(dev_driver->i2c_num, dev_driver->i2c_tar_addr) != 0)
    {
        LOGE(TAG, "gc2053_init_1080p fail\n");
        return -1;
    }
    return 0;
}

static int cis_gc2053_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    (void)config;

    int ret;
    GC2053_REG_T *reg_list;
    int cnt;

    reg_list = gc2053_start_regs;
    cnt = AIVA_ARRAY_LEN(gc2053_start_regs);
    ret = gc2053_program_regs(dev_driver->i2c_num, dev_driver->i2c_tar_addr, reg_list, cnt);
    if (ret < 0)
    {
        LOGE(TAG, "gc2053_start failed!");
    }
    return ret;
}

static int cis_gc2053_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    GC2053_REG_T *reg_list;
    int cnt;

    reg_list = gc2053_stop_regs;
    cnt = AIVA_ARRAY_LEN(gc2053_stop_regs);
    ret = gc2053_program_regs(dev_driver->i2c_num, dev_driver->i2c_tar_addr, reg_list, cnt);
    if (ret < 0)
    {
        LOGE(TAG, "gc2053_stop failed!");
    }
    return ret;
}

static void cis_gc2053_power_on(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    gc2053_busy_delay(3);
    gpio_set_pin(power_pin, GPIO_PV_HIGH);
    gc2053_busy_delay(5);
    return;
}

static void cis_gc2053_power_off(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    gc2053_busy_delay(5);
    return;
}

static void cis_gc2053_reset(cis_dev_driver_t *dev_driver)
{
    int reset_pin = dev_driver->reset_pin;

    gpio_set_drive_mode(reset_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(reset_pin, GPIO_PV_LOW);
    gc2053_busy_delay(3);
    gpio_set_pin(reset_pin, GPIO_PV_HIGH);
    gc2053_busy_delay(5);

    return;
}

static int cis_gc2053_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    (void)dev_driver;

    param->interface_type = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq = 696 * 2;
    param->mipi_param.lane_num = 1;
    param->mipi_param.vc_num = 1;
    param->mipi_param.virtual_channels[0] = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

uint8_t regValTable[29][4] = {
    // 0xb4  0xb3 0xb8 0xb9
    {0x00, 0x00, 0x01, 0x00},
    {0x00, 0x10, 0x01, 0x0c},
    {0x00, 0x20, 0x01, 0x1b},
    {0x00, 0x30, 0x01, 0x2c},
    {0x00, 0x40, 0x01, 0x3f},
    {0x00, 0x50, 0x02, 0x16},
    {0x00, 0x60, 0x02, 0x35},
    {0x00, 0x70, 0x03, 0x16},
    {0x00, 0x80, 0x04, 0x02},
    {0x00, 0x90, 0x04, 0x31},
    {0x00, 0xa0, 0x05, 0x32},
    {0x00, 0xb0, 0x06, 0x35},
    {0x00, 0xc0, 0x08, 0x04},
    {0x00, 0x5a, 0x09, 0x19},
    {0x00, 0x83, 0x0b, 0x0f},
    {0x00, 0x93, 0x0d, 0x12},
    {0x00, 0x84, 0x10, 0x00},
    {0x00, 0x94, 0x12, 0x3a},
    {0x01, 0x2c, 0x1a, 0x02},
    {0x01, 0x3c, 0x1b, 0x20},
    {0x00, 0x8c, 0x20, 0x0f},
    {0x00, 0x9c, 0x26, 0x07},
    {0x02, 0x64, 0x36, 0x21},
    {0x02, 0x74, 0x37, 0x3a},
    {0x00, 0xc6, 0x3d, 0x02},
    {0x00, 0xdc, 0x3f, 0x3f},
    {0x02, 0x85, 0x3f, 0x3f},
    {0x02, 0x95, 0x3f, 0x3f},
    {0x00, 0xce, 0x3f, 0x3f},
};

uint32_t gainLevelTable[30] = {
    64,
    74,
    89,
    102,
    127,
    147,
    177,
    203,
    260,
    300,
    361,
    415,
    504,
    581,
    722,
    832,
    1027,
    1182,
    1408,
    1621,
    1990,
    2291,
    2850,
    3282,
    4048,
    5180,
    5500,
    6744,
    7073,
    0xffffffff,
};

static int total = sizeof(gainLevelTable) / sizeof(uint32_t);

static int cis_gc2053_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    (void)dev_driver;

    exp_param->min_again = 1.0;
    exp_param->max_again = 10.0;
    exp_param->step_again = 1.0;

    exp_param->min_dgain = 1.0;
    exp_param->max_dgain = 1.0;
    exp_param->step_dgain = 1.0;

    // exp_param->min_itime = 1.0;
    // exp_param->max_itime = 1125 - 6; // vte - 6
    // exp_param->step_itime = 1.0;

    exp_param->min_itime = 0.0005; // 10ms is minimum time to avoid flicker
    exp_param->max_itime = 0.24;
    exp_param->step_itime = 0.0005;

    return 0;
}

static int cis_gc2053_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    uint32_t i2c_tar_addr = dev_driver->i2c_tar_addr;
    i2c_init(i2c_num, i2c_tar_addr, 7, 350 * 1000);

    /*LOGD(__func__, "set again %f, dgain %f, itime %f", */
    /*exp->again[0], exp->dgain[0], exp->itime[0]);*/

    uint32_t gain = exp->again * exp->dgain * 64;

    int analog_idx;
    uint32_t tol_dig_gain = 0;

    for (analog_idx = 0; analog_idx < total; analog_idx++)
    {
        if ((gainLevelTable[analog_idx] <= gain) && (gain < gainLevelTable[analog_idx + 1]))
            break;
    }

    tol_dig_gain = gain * 64 / gainLevelTable[analog_idx];
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xfe, 0x00);
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb4, regValTable[analog_idx][0]);
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb3, regValTable[analog_idx][1]);
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb8, regValTable[analog_idx][2]);
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb9, regValTable[analog_idx][3]);

    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb1, (tol_dig_gain >> 6));
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xb2, ((tol_dig_gain & 0x3f) << 2));

    float itime = exp->itime;
    int itime_lines;
    if (itime < 1) {
        itime_lines = itime / 0.01 * 282;
    }
    else {
        itime_lines = (int)itime;
    }

    // 0.01->282
    //max 0.04
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0xfe, 0x00);
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0x03, (itime_lines >> 8));
    gc2053_write_reg(i2c_num, i2c_tar_addr, 0x04, (itime_lines & 0xff));

    int framelength = 0x546;
    // itime_lines > 1125  decrease framerate
    if (itime_lines > 0x546 - 6) {
        framelength = itime_lines+6;
        gc2053_write_reg(i2c_num, i2c_tar_addr, 0x41, (framelength >> 8));
        gc2053_write_reg(i2c_num, i2c_tar_addr, 0x42, (framelength & 0xff));
    }
    // LOGE(TAG, "framelength 0x%02x, itime lines 0x%02x", framelength, itime_lines);

    return 0;
}

static int cis_gc2053_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = 1920;
    param->height = 1080;
    param->framerate = dev_driver->fps;
    param->format = CIS_FORMAT_RGB_BAYER10;
    param->bayer_pat = CIS_BAYER_PAT_RGGB;

    return 0;
}

static int cis_gc2053_wake(cis_dev_driver_t *dev_driver)
{
    (void)dev_driver;
    return 0;
}

static int cis_gc2053_sleep(cis_dev_driver_t *dev_driver)
{
    (void)dev_driver;
    return 0;
}

static cis_dev_driver_t gc2053_dev0 = {
    .name = "gc2053_dev0",
    .i2c_num = I2C_DEVICE_0,
    .i2c_tar_addr = SENSOR_ADDR_WR,
    .power_pin = GPIO_PIN0,
    .reset_pin = GPIO_PIN9,
    .mclk_id = CIS_MCLK_ID_MCLK0,
    .mclk_freq = 24 * 1000 * 1000,
    .fps = 30,
    .mf_mode = 0,
    .context = NULL,
    .init = cis_gc2053_init,
    .start_stream = cis_gc2053_start_stream,
    .stop_stream = cis_gc2053_stop_stream,
    .wake = cis_gc2053_wake,
    .sleep = cis_gc2053_sleep,
    .power_on = cis_gc2053_power_on,
    .power_off = cis_gc2053_power_off,
    .reset = cis_gc2053_reset,
    .get_interface_param = cis_gc2053_get_interface_param,
    .get_exposure_param = cis_gc2053_get_exposure_param,
    .set_exposure = cis_gc2053_set_exposure,
    .get_frame_parameter = cis_gc2053_get_frame_parameter};

static cis_dev_driver_t gc2053_dev1 = {
    .name = "gc2053_dev1",
    .i2c_num = I2C_DEVICE_1,
    .i2c_tar_addr = SENSOR_ADDR_WR,
    .power_pin = GPIO_PIN1,
    .reset_pin = GPIO_PIN1,
    .mclk_id = CIS_MCLK_ID_MCLK1,
    .mclk_freq = 24 * 1000 * 1000,
    .fps = 30,
    .mf_mode = 0,
    .context = NULL,
    .init = cis_gc2053_init,
    .start_stream = cis_gc2053_start_stream,
    .stop_stream = cis_gc2053_stop_stream,
    .wake = cis_gc2053_wake,
    .sleep = cis_gc2053_sleep,
    .power_on = cis_gc2053_power_on,
    .power_off = cis_gc2053_power_off,
    .reset = cis_gc2053_reset,
    .get_interface_param = cis_gc2053_get_interface_param,
    .get_exposure_param = cis_gc2053_get_exposure_param,
    .set_exposure = cis_gc2053_set_exposure,
    .get_frame_parameter = cis_gc2053_get_frame_parameter};

static cis_dev_driver_t gc2053_dev2 = {
    .name = "gc2053_dev2",
    .i2c_num = I2C_DEVICE_2,
    .i2c_tar_addr = SENSOR_ADDR_WR,
    .power_pin = GPIO_PIN1,
    .reset_pin = GPIO_PIN1,
    .mclk_id = CIS_MCLK_ID_MCLK1,
    .mclk_freq = 24 * 1000 * 1000,
    .fps = 30,
    .mf_mode = 0,
    .context = NULL,
    .init = cis_gc2053_init,
    .start_stream = cis_gc2053_start_stream,
    .stop_stream = cis_gc2053_stop_stream,
    .wake = cis_gc2053_wake,
    .sleep = cis_gc2053_sleep,
    .power_on = cis_gc2053_power_on,
    .power_off = cis_gc2053_power_off,
    .reset = cis_gc2053_reset,
    .get_interface_param = cis_gc2053_get_interface_param,
    .get_exposure_param = cis_gc2053_get_exposure_param,
    .set_exposure = cis_gc2053_set_exposure,
    .get_frame_parameter = cis_gc2053_get_frame_parameter};

cis_dev_driver_t *cis_gc2053[] = {
    &gc2053_dev0,
    &gc2053_dev1,
    &gc2053_dev2};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI, gc2053_dev0, 0, &gc2053_dev0, NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI, gc2053_dev1, 0, &gc2053_dev1, NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI, gc2053_dev2, 0, &gc2053_dev2, NULL);