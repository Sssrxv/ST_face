#include <math.h>
#include "cis_sc200.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define REG_DLY (0xffff)
#define SENSOR_ADDR_WR (0x30)

#define MAX_EXPOSURE              (0x04e2-4)     // unit: lines

typedef struct _SC200CS_REG_T
{
    uint16_t reg_addr;
    uint8_t data;
} SC200CS_REG_T;

typedef struct _SC200CS_GAIN_T {
    uint8_t coarse;
    uint8_t fine;
    float   gain;
} SC200CS_GAIN_T;

static const char *TAG = "cis_sc200";

static SC200CS_REG_T sc200_start_regs[] __ATTR_ALIGN__(32) = {

};

static SC200CS_REG_T sc200_stop_regs[] __ATTR_ALIGN__(32) = {

};

static const SC200CS_GAIN_T sc200cs_again_table[] = {
    {0x00, 0x00, 1.000},
    {0x01, 0x00, 2.000},
    {0x03, 0x00, 4.000},
    {0x07, 0x00, 8.000},
    {0x0f, 0x00, 16.000},
};

// 0.31 step dgain table, 1.0 ~ 7.87
static const SC200CS_GAIN_T sc200cs_dgain_table[] = {
    {0x00, 0x80,  1.000},
    {0x00, 0x84,  1.031},
    {0x00, 0x88,  1.063},
    {0x00, 0x8C,  1.094},
    {0x00, 0x90,  1.125},
    {0x00, 0x94,  1.156},
    {0x00, 0x98,  1.188},
    {0x00, 0x9C,  1.219},
    {0x00, 0xA0,  1.250},
    {0x00, 0xA4,  1.281},
    {0x00, 0xA8,  1.313},
    {0x00, 0xAC,  1.344},
    {0x00, 0xB0,  1.375},
    {0x00, 0xB4,  1.406},
    {0x00, 0xB8,  1.438},
    {0x00, 0xBC,  1.469},
    {0x00, 0xC0,  1.500},
    {0x00, 0xC4,  1.531},
    {0x00, 0xC8,  1.563},
    {0x00, 0xCC,  1.594},
    {0x00, 0xD0,  1.625},
    {0x00, 0xD4,  1.656},
    {0x00, 0xD8,  1.688},
    {0x00, 0xDC,  1.719},
    {0x00, 0xE0,  1.750},
    {0x00, 0xE4,  1.781},
    {0x00, 0xE8,  1.813},
    {0x00, 0xEC,  1.844},
    {0x00, 0xF0,  1.875},
    {0x00, 0xF4,  1.906},
    {0x00, 0xF8,  1.938},
    {0x00, 0xFC,  1.969},
    {0x01, 0x80,  2.000},
    {0x01, 0x84,  2.063},
    {0x01, 0x88,  2.125},
    {0x01, 0x8C,  2.188},
    {0x01, 0x90,  2.250},
    {0x01, 0x94,  2.313},
    {0x01, 0x98,  2.375},
    {0x01, 0x9C,  2.438},
    {0x01, 0xA0,  2.500},
    {0x01, 0xA4,  2.563},
    {0x01, 0xA8,  2.625},
    {0x01, 0xAC,  2.688},
    {0x01, 0xB0,  2.750},
    {0x01, 0xB4,  2.813},
    {0x01, 0xB8,  2.875},
    {0x01, 0xBC,  2.938},
    {0x01, 0xC0,  3.000},
    {0x01, 0xC4,  3.063},
    {0x01, 0xC8,  3.125},
    {0x01, 0xCC,  3.188},
    {0x01, 0xD0,  3.250},
    {0x01, 0xD4,  3.313},
    {0x01, 0xD8,  3.375},
    {0x01, 0xDC,  3.438},
    {0x01, 0xE0,  3.500},
    {0x01, 0xE4,  3.563},
    {0x01, 0xE8,  3.625},
    {0x01, 0xEC,  3.688},
    {0x01, 0xF0,  3.750},
    {0x01, 0xF4,  3.813},
    {0x01, 0xF8,  3.875},
    {0x01, 0xFC,  3.938},
    {0x03, 0x80,  4.000},
    {0x03, 0x84,  4.125},
    {0x03, 0x88,  4.250},
    {0x03, 0x8C,  4.375},
    {0x03, 0x90,  4.500},
    {0x03, 0x94,  4.625},
    {0x03, 0x98,  4.750},
    {0x03, 0x9C,  4.875},
    {0x03, 0xA0,  5.000},
    {0x03, 0xA4,  5.125},
    {0x03, 0xA8,  5.250},
    {0x03, 0xAC,  5.375},
    {0x03, 0xB0,  5.500},
    {0x03, 0xB4,  5.625},
    {0x03, 0xB8,  5.750},
    {0x03, 0xBC,  5.875},
    {0x03, 0xC0,  6.000},
    {0x03, 0xC4,  6.125},
    {0x03, 0xC8,  6.250},
    {0x03, 0xCC,  6.375},
    {0x03, 0xD0,  6.500},
    {0x03, 0xD4,  6.625},
    {0x03, 0xD8,  6.750},
    {0x03, 0xDC,  6.875},
    {0x03, 0xE0,  7.000},
    {0x03, 0xE4,  7.125},
    {0x03, 0xE8,  7.250},
    {0x03, 0xEC,  7.375},
    {0x03, 0xF0,  7.500},
    {0x03, 0xF4,  7.625},
    {0x03, 0xF8,  7.750},
    {0x03, 0xFC,  7.875},
};

// mclk 24Mhz
// window size 1600*1200
// mipiclk 696Mbps/lane
// framelength 1125
// rowtime 29.42529us
// pattern rggb
static const SC200CS_REG_T sc200_1600x1200[] __ATTR_ALIGN__(32) = {
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x320c,0x06},
    {0x320d,0xb4},
    {0x3253,0x18},
    {0x3301,0x09},
    {0x3304,0x58},
    {0x3306,0x48},
    {0x3308,0x10},
    {0x3309,0xa0},
    {0x330a,0x00},
    {0x330b,0xd4},
    {0x330d,0x38},
    {0x330e,0x40},
    {0x3310,0x06},
    {0x331c,0x02},
    {0x331e,0x49},
    {0x331f,0x91},
    {0x3320,0x0c},
    {0x3323,0x0e},
    {0x3328,0x0c},
    {0x3329,0x10},
    {0x332b,0x0a},
    {0x3364,0x57},
    {0x338c,0x03},
    {0x338d,0xff},
    {0x3390,0x01},
    {0x3391,0x03},
    {0x3392,0x07},
    {0x3393,0x0a},
    {0x3394,0x0b},
    {0x3395,0xff},
    {0x33ac,0x08},
    {0x33ae,0x14},
    {0x33af,0x38},
    {0x33b2,0x01},
    {0x33b3,0x1f},
    {0x3622,0xe0},
    {0x3624,0x41},
    {0x3625,0x08},
    {0x3626,0x01},
    {0x3630,0x08},
    {0x3633,0x44},
    {0x3636,0x44},
    {0x3637,0xe9},
    {0x3638,0x0d},
    {0x3670,0x0d},
    {0x3677,0x82},
    {0x3678,0x84},
    {0x3679,0x84},
    {0x367e,0x01},
    {0x367f,0x07},
    {0x3690,0x44},
    {0x3691,0x44},
    {0x3692,0x33},
    {0x369c,0x03},
    {0x369d,0x07},
    {0x369e,0xe0},
    {0x369f,0xee},
    {0x36a0,0xf0},
    {0x36a1,0x03},
    {0x36a2,0x07},
    {0x36ea,0x89},
    {0x36eb,0x85},
    {0x36ec,0x00},
    {0x36ed,0x14},
    {0x3904,0x04},
    {0x3948,0x02},
    {0x3949,0x58},
    {0x3e01,0x4d},
    {0x3e02,0xa0},
    {0x3e09,0x00},
    {0x3e26,0x00},
    {0x3f03,0x02},
    {0x4502,0x14},
    {0x4509,0x38},
    {0x4815,0x00},
    {0x36e9,0x01},
    {0x0100,0x01},
};

static void sc200_busy_delay(int32_t ms)
{
    aiva_msleep(ms);
}

static int sc200_write_reg(int i2c_num, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];
    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 3);
    return ret;
}

static int sc200_read_reg(int i2c_num, uint16_t reg_addr, uint8_t *reg_val)
{
    int ret;

    uint8_t addr_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xff;
    addr_buf[1] = (reg_addr >> 0) & 0xff;

    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, addr_buf, 2);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, &reg_addr, 1);*/
    if (ret < 0)
    {
        return ret;
    }
    ret = i2c_recv_data(i2c_num, SENSOR_ADDR_WR, 0, 0, reg_val, 1);
    /*ret = i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, i2c_num, 0, 0, reg_val, 1);*/

    return ret;
}

static int sc200_program_regs(
    int i2c_num,
    const SC200CS_REG_T *reg_list,
    int cnt)
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++)
    {
        if (reg_list[i].reg_addr != REG_DLY)
        {
            ret = sc200_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0)
            {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                     i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else
        {
            sc200_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static uint16_t sc200_get_sensor_id(i2c_device_number_t i2c_num)
{
    uint8_t id_h = 0;
    uint8_t id_l = 0;

    sc200_read_reg(i2c_num, 0x3107, &id_h);
    sc200_read_reg(i2c_num, 0x3108, &id_l);

    return ((id_h << 8) | id_l);
}

static int sc200_i2c_test(int i2c_num)
{
    uint16_t val;

    val = sc200_get_sensor_id(i2c_num);
    LOGD(TAG, "sc200 readed id is 0x%4x", val);

    if (val != 0xeb15)
    {
        LOGE(TAG, "sc200_i2c_test: read id fail!, get 0x%4x, expected 0xeb15", val);
        return -1;
    }

    LOGD(TAG, "sc200_i2c_test: read id success!");
    return 0;
}

static int sc200_init(const SC200CS_REG_T *reg_list, int cnt, int i2c_num)
{
    int ret;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50 * 1000);

    if (sc200_i2c_test(i2c_num) < 0)
    {
        LOGE(TAG, "sc200 i2c test fail\n");
        return -1;
    }

    ret = sc200_program_regs(i2c_num, reg_list, cnt);

    /*LOGD(TAG, "sc200_init() return: %d.", ret);*/
    if (ret < 0)
    {
        LOGE(__func__, "line %d, sc200_program_regs failed!", __LINE__);
        return ret;
    }
    return ret;
}

static int sc200_1600x1200_init(int i2c_num)
{
    int ret;
    ret = sc200_init(sc200_1600x1200, AIVA_ARRAY_LEN(sc200_1600x1200), i2c_num);
    return ret;
}

static int cis_sc200_init(cis_dev_driver_t *dev_driver)
{
    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0)
    {
        sysctl_set_sens_mclk(MCLK_ID0, 24 * 1000 * 1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    }
    else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1)
    {
        sysctl_set_sens_mclk(MCLK_ID1, 24 * 1000 * 1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    }
    else
    {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    if (sc200_1600x1200_init(dev_driver->i2c_num) != 0)
    {
        LOGE(TAG, "sc200_init_1600x1200 fail\n");
        return -1;
    }
    return 0;
}

static int cis_sc200_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    SC200CS_REG_T *reg_list;
    int cnt;

    reg_list = sc200_start_regs;
    cnt = AIVA_ARRAY_LEN(sc200_start_regs);
    ret = sc200_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0)
    {
        LOGE(TAG, "sc200_start failed!");
    }
    return ret;
}

static int cis_sc200_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    SC200CS_REG_T *reg_list;
    int cnt;

    reg_list = sc200_stop_regs;
    cnt = AIVA_ARRAY_LEN(sc200_stop_regs);
    ret = sc200_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0)
    {
        LOGE(TAG, "sc200_stop failed!");
    }
    return ret;
}

static void cis_sc200_power_on(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    sc200_busy_delay(3);
    gpio_set_pin(power_pin, GPIO_PV_HIGH);
    sc200_busy_delay(5);
    return;
}

static void cis_sc200_power_off(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    sc200_busy_delay(5);
    return;
}

static void cis_sc200_reset(cis_dev_driver_t *dev_driver)
{
    int reset_pin = dev_driver->reset_pin;

    gpio_set_drive_mode(reset_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(reset_pin, GPIO_PV_LOW);
    sc200_busy_delay(3);
    gpio_set_pin(reset_pin, GPIO_PV_HIGH);
    sc200_busy_delay(5);

    return;
}

static int cis_sc200_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq = 696 * 2;
    param->mipi_param.lane_num = 1;
    param->mipi_param.vc_num = 1;
    param->mipi_param.virtual_channels[0] = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

static int cis_sc200_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    exp_param->min_again = 1.0;
    exp_param->max_again = 16.0;
    exp_param->step_again = 1.0;

    exp_param->min_dgain = 1.0;
    exp_param->max_dgain = 7.875;
    exp_param->step_dgain = 1.0;

    exp_param->min_itime = 1.0;
    exp_param->max_itime = MAX_EXPOSURE; // vte - 6
    exp_param->step_itime = 1.0;

    return 0;
}

static int cis_sc200_split_dgain(float dgain, uint8_t *dgain_coarse, uint8_t *dgain_fine)
{
    int i;
    int items = sizeof(sc200cs_dgain_table)/sizeof(SC200CS_GAIN_T);

    for (i = 0; i < items; i++)
    {
        if (dgain <= sc200cs_dgain_table[i].gain)
        {
            *dgain_coarse = sc200cs_dgain_table[i].coarse;
            *dgain_fine   = sc200cs_dgain_table[i].fine;
            return 0;
        }
    }

    LOGE(__func__, "digital gain out of range: dgain=%f", dgain);
    *dgain_coarse = 0x00;
    *dgain_fine = 0x80;
    return -1;
}

static int cis_sc200_split_gain(float again, uint8_t *again_coarse, uint8_t *dgain_coarse, uint8_t *dgain_fine)
{
    int i;
    int items = sizeof(sc200cs_again_table)/sizeof(SC200CS_GAIN_T);

    float again_now;
    for (i = 0; i < items; i++)
    {
        if (i + 1 == items) {
            *again_coarse = sc200cs_again_table[i].coarse;
            again_now = sc200cs_again_table[i].gain;
            break;
        }

        if ((again >= sc200cs_again_table[i].gain) && (again < sc200cs_again_table[i+1].gain))
        {
            *again_coarse = sc200cs_again_table[i].coarse;
            again_now = sc200cs_again_table[i].gain;
            // LOGD(__func__, "coarse is %d", *again_coarse);
            break;
        }
    }

    float div = again / again_now;
    // LOGD(__func__, "again is %f, now is %f, div is %f", again, again_now, div);
    cis_sc200_split_dgain(div, dgain_coarse, dgain_fine);

    return 0;
}

static int cis_sc200_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    float again = exp->again;
    float dgain = exp->dgain;
    float itime = exp->itime;
    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50*1000);

    //-- group hold start
    sc200_write_reg(i2c_num, 0x3812, 0x00);

    // exposure
    uint8_t itime_h;
    uint8_t itime_m;    
    uint8_t itime_l;
    if (itime > MAX_EXPOSURE)
        itime = MAX_EXPOSURE;
    uint32_t exposure = (int)itime*16;

    uint8_t h,m,l;
    sc200_read_reg(i2c_num, 0x3e00, &h);
    sc200_read_reg(i2c_num, 0x3e01, &m);
    sc200_read_reg(i2c_num, 0x3e02, &l);

    itime_l = (exposure) & 0xff;
    itime_m = (exposure >> 8) & 0xff;
    itime_h = (exposure >> 16) & 0xf;
    sc200_write_reg(i2c_num, 0x3e02, itime_l);
    sc200_write_reg(i2c_num, 0x3e01, itime_m);
    sc200_write_reg(i2c_num, 0x3e00, itime_h);

    // gain
    uint8_t again_coarse, dgain_coarse, dgain_fine;
    cis_sc200_split_gain(again, &again_coarse, &dgain_coarse, &dgain_fine);
    sc200_write_reg(i2c_num, 0x3e09, again_coarse);
    sc200_write_reg(i2c_num, 0x3e06, dgain_coarse);
    sc200_write_reg(i2c_num, 0x3e07, dgain_fine);

    //-- group hold end
    sc200_write_reg(i2c_num, 0x3802, 0x00);
    sc200_write_reg(i2c_num, 0x3812, 0x30);

    return 0;
}

static int cis_sc200_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = 1600;
    param->height = 1200;
    param->framerate = 30;
    param->format = CIS_FORMAT_RGB_BAYER10;
    param->bayer_pat = CIS_BAYER_PAT_RGGB;

    return 0;
}

static int cis_sc200_wake(cis_dev_driver_t *dev_driver)
{
    return 0;
}

static int cis_sc200_sleep(cis_dev_driver_t *dev_driver)
{
    return 0;
}

static cis_dev_driver_t sc200_dev0 = {
    .name = "sc200_dev0",
    .i2c_num = I2C_DEVICE_0,
    .power_pin = GPIO_PIN0,
    .reset_pin = GPIO_PIN9,
    .mclk_id = CIS_MCLK_ID_MCLK0,
    .context = NULL,
    .init = cis_sc200_init,
    .start_stream = cis_sc200_start_stream,
    .stop_stream = cis_sc200_stop_stream,
    .power_on = cis_sc200_power_on,
    .power_off = cis_sc200_power_off,
    .reset = cis_sc200_reset,
    .get_interface_param = cis_sc200_get_interface_param,
    .get_exposure_param = cis_sc200_get_exposure_param,
    .set_exposure = cis_sc200_set_exposure,
    .get_frame_parameter = cis_sc200_get_frame_parameter};

static cis_dev_driver_t sc200_dev1 = {
    .name = "sc200_dev1",
    .i2c_num = I2C_DEVICE_1,
    .power_pin = GPIO_PIN1,
    .reset_pin = GPIO_PIN1,
    .mclk_id = CIS_MCLK_ID_MCLK1,
    .context = NULL,
    .init = cis_sc200_init,
    .start_stream = cis_sc200_start_stream,
    .stop_stream = cis_sc200_stop_stream,
    .wake = cis_sc200_wake,
    .sleep = cis_sc200_sleep,
    .power_on = cis_sc200_power_on,
    .power_off = cis_sc200_power_off,
    .reset = cis_sc200_reset,
    .get_interface_param = cis_sc200_get_interface_param,
    .get_exposure_param = cis_sc200_get_exposure_param,
    .set_exposure = cis_sc200_set_exposure,
    .get_frame_parameter = cis_sc200_get_frame_parameter};

static cis_dev_driver_t sc200_dev2 = {
    .name = "sc200_dev2",
    .i2c_num = I2C_DEVICE_2,
    .power_pin = GPIO_PIN1,
    .reset_pin = GPIO_PIN1,
    .mclk_id = CIS_MCLK_ID_MCLK1,
    .context = NULL,
    .init = cis_sc200_init,
    .start_stream = cis_sc200_start_stream,
    .stop_stream = cis_sc200_stop_stream,
    .wake                   = cis_sc200_wake,
    .sleep                  = cis_sc200_sleep,
    .power_on = cis_sc200_power_on,
    .power_off = cis_sc200_power_off,
    .reset = cis_sc200_reset,
    .get_interface_param = cis_sc200_get_interface_param,
    .get_exposure_param = cis_sc200_get_exposure_param,
    .set_exposure = cis_sc200_set_exposure,
    .get_frame_parameter = cis_sc200_get_frame_parameter};

cis_dev_driver_t *cis_sc200[] = {
    &sc200_dev0,
    &sc200_dev1,
    &sc200_dev2};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI, sc200_dev0, 0, &sc200_dev0, NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI, sc200_dev1, 0, &sc200_dev1, NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI, sc200_dev2, 0, &sc200_dev2, NULL);