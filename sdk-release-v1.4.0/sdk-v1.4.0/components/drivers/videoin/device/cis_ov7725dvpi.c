#include <stdint.h>
#include "sysctl.h"
#include "dvpin_ctl.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "i2c.h"
#include "gpio.h"
/*#include "aiva_printf.h"*/
#include "syslog.h"
#include "cis_ov7725dvpi.h"
#include "udevice.h"

#define SENSOR_ADDR_WR              (0x42 >> 1) /* Slave address used to write sensor registers. */
/*#define SENSOR_ADDR_RD              0x43 [> Slave address used to read from sensor registers. <]*/
#define TAG                         "cis_ov7725dvpi"

#define OV7725_W                    (640)
#define OV7725_H                    (480)

#define REG_DLY                     0xffff

typedef struct _OV7725_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
}OV7725_REG_T;

static OV7725_REG_T sensor_ov7725_vga_regs[] __ATTR_ALIGN__(32)= {
    {REG_DLY, 50},
    {0x12, 0x80}, //Reset
    {REG_DLY, 10},
    {0x3d, 0x03},
    {0x12, 0x00}, // COM7:   0x00(VGA), 0x40(QVGA)
    {0x17, 0x22}, // HSTART: 0x26(VGA), 0x3F(QVGA) 
    {0x18, 0xa4}, // HSIZE:  0xA0(VGA), 0x50(QVGA)
    {0x19, 0x07}, // VSTART: 0x07(VGA), 0x03(QVGA)
    {0x1a, 0xf0}, // VSIZE:  0xF0(VGA), 0x78(QVGA)
    {0x32, 0x00}, // HREF
    {0x29, 0xa0}, // HOutSize: 0xA0(VGA), 0x50(QVGA)
    {0x2c, 0xf0}, // VOutSize: 0xF0(VGA), 0x78(QVGA)
    {0x2a, 0x00}, // EXHCH
    {0x11, 0x40}, // Use external clock directly
    /*{0x11, 0x00}, // Input clock / 2*/
    {0x11, 0x01}, // Input clock / 4
    /*{0x11, 0x02}, // Input clock / 6*/
    /*{0x11, 0x03}, // Input clock / 8*/
    /*{0x11, 0x0f}, // Input clock /32*/
    {0x42, 0x7f},
    {0x4d, 0x09}, // FixGain
    {0x63, 0xe0}, // AWB_Ctrl0
    {0x64, 0xff}, 
    {0x65, 0x20},
    {0x66, 0x00},
    {0x67, 0x48},
    {0x13, 0xf0}, // COM8
    /*{0x0d, 0x01}, // Bypass PLL, 0x51/0x61/0x71 for different AEC/AGC window*/
    {0x0d, 0xc1}, // 0x41: PLL 4x, 0x81: PLL 6x, 0xc1: PLL 8x
    /*{0x0c, 0x50}, // 0x10: default, 0x50: horizontal mirror*/
    {0x0f, 0xc5},
    {0x14, 0x11}, // COM9
    {0x22, 0x7f},
    {0x23, 0x03},
    {0x24, 0x40},
    {0x25, 0x30},
    {0x26, 0xa1},
    {0x2b, 0x00},
    {0x6b, 0xaa},
    {0x13, 0xff}, // COM8
    {0x90, 0x05},
    {0x91, 0x01},
    {0x92, 0x03},
    {0x93, 0x00},
    {0x94, 0xb0},
    {0x95, 0x9d},
    {0x96, 0x13},
    {0x97, 0x16},
    {0x98, 0x7b},
    {0x99, 0x91},
    {0x9a, 0x1e},
    {0x9b, 0x08},
    {0x9c, 0x20},
    {0x9e, 0x81},
    {0xa6, 0x06},
    // Gamma
    {0x7e, 0x0c},
    {0x7f, 0x16},
    {0x80, 0x2a},
    {0x81, 0x4e},
    {0x82, 0x61},
    {0x83, 0x6f},
    {0x84, 0x7b},
    {0x85, 0x86},
    {0x86, 0x8e},
    {0x87, 0x97},
    {0x88, 0xa4},
    {0x89, 0xaf},
    {0x8a, 0xc5},
    {0x8b, 0xd7},
    {0x8c, 0xe8},
    {0x8d, 0x20},
    // Light band filter
    //{0x33, 0x3f},
    //{0x22, 0x1c},
    {0x33, 0x00},
    {0x22, 0x7f},
    {0x23, 0x03},
    // Lens Correction
    {0x4a, 0x10},
    {0x49, 0x10},
    {0x4b, 0x14},
    {0x4c, 0x17},
    {0x46, 0x05},
    /*{0x0e, 0x65},*/
    // Debug
    /*{0x13, 0xf0},*/
    /*{0x00, 0x00},*/
    /*{0x10, 0x80},*/
    /*{0x08, 0x00},*/
};

static OV7725_REG_T sensor_ov7725_start_regs[] __ATTR_ALIGN__(32) = {
    {0x0e, 0x65},
};

static void sensor_ov7725_busy_delay (int32_t ms)
{
    aiva_msleep(ms);
}

static int sensor_ov7725_write_reg(int i2c_num, uint8_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[2];
    data_buf[0] = reg_addr;
    data_buf[1] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 2);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, data_buf, 2);*/
    return ret;
}

static int sensor_ov7725_read_reg(int i2c_num, uint8_t reg_addr, uint8_t *reg_val)
{
    int ret;
    
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, &reg_addr, 1);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, &reg_addr, 1);*/
    if (ret < 0) {
        return ret;
    }
    ret = i2c_recv_data(i2c_num, SENSOR_ADDR_WR, 0, 0, reg_val, 1);
    /*ret = i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, i2c_num, 0, 0, reg_val, 1);*/
    
    return ret;
}


static int sensor_ov7725_program_regs(
        int             i2c_num,
        OV7725_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = sensor_ov7725_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                break;
            }
        }
        else {
            sensor_ov7725_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static void sensor_ov7725_power_on(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;
    LOGD(TAG, "sensor_ov7725_power_on pow_pin %d", pow_pin);

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_HIGH);
    sensor_ov7725_busy_delay(50);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    sensor_ov7725_busy_delay(50);
}

static void sensor_ov7725_power_off(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;
    LOGD(TAG, "sensor_ov7725_power_off pow_pin %d", pow_pin);

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_HIGH);
    sensor_ov7725_busy_delay(50);
}

static void sensor_ov7725_reset(cis_dev_driver_t *dev_driver)
{
}

static int sensor_i2c_test(int i2c_num)
{
    uint8_t pid, ver;
    uint8_t val;

    sensor_ov7725_read_reg(i2c_num, 0x0a, &pid); 
    sensor_ov7725_read_reg(i2c_num, 0x0b, &ver); 

    LOGD(TAG, "sensor_i2c_test: pid is 0x%x, ver is 0x%x.", pid, ver);

    sensor_ov7725_write_reg(i2c_num, 0x17, 0x22);
    sensor_ov7725_read_reg(i2c_num, 0x17, &val);
    if (val != 0x22) {
        LOGE(TAG, "sensor_i2c_test: read/write fail!");
        return -1;
    }

    LOGD(TAG, "sensor_i2c_test: read/write success!");
    return 0;
}

static int sensor_ov7725_init(int i2c_num, int mclk_id)
{
    int ret;
    OV7725_REG_T        *reg_list;
    int                 cnt;

    if (mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    }

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 50*1000);

    if (sensor_i2c_test(i2c_num) < 0) {
        return -1;
    }

    reg_list = sensor_ov7725_vga_regs;
    cnt = AIVA_ARRAY_LEN(sensor_ov7725_vga_regs);
    ret = sensor_ov7725_program_regs(i2c_num, reg_list, cnt);
    LOGD(TAG, "sensor_ov7725_init() return: %d.", ret);
    
    return ret;
}

static int cis_dvpi_ov7725_init(cis_dev_driver_t *dev_driver)
{
    if (sensor_ov7725_init(dev_driver->i2c_num, dev_driver->mclk_id) != 0) {
        LOGE(TAG, "ov9282 init fail\n");
        return -1;
    }

    return 0;
}

static int sensor_ov7725_start(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    OV7725_REG_T        *reg_list;
    int                 cnt;

    reg_list = sensor_ov7725_start_regs;
    cnt     = AIVA_ARRAY_LEN(sensor_ov7725_start_regs);
    ret     = sensor_ov7725_program_regs(dev_driver->i2c_num, reg_list, cnt);
    return ret;
}

static cis_dvpi_param_t m_mono_param = {
    .format  = DVPIN_FMT_YUYV,
    .h_start = 0,
    .v_start = 0,
    .h_stop  = OV7725_W*2 - 1,
    .v_stop  = OV7725_H - 1,
    .h_size  = OV7725_W*2,
    .v_size  = OV7725_H,
    .vs_pol  = (int)DVPI_VS_POL_TYPE_PULSE,
    .x_stride= OV7725_W,             // stride of YUYV
    .y_stride= OV7725_W*OV7725_H,    // stride of YUYV
};

static int dvpi_ov7725_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_DVPI;
    param->dvpi_param.format                = m_mono_param.format;
    param->dvpi_param.h_start               = m_mono_param.h_start;
    param->dvpi_param.v_start               = m_mono_param.v_start;
    param->dvpi_param.h_stop                = m_mono_param.h_stop;
    param->dvpi_param.v_stop                = m_mono_param.v_stop;
    param->dvpi_param.h_size                = m_mono_param.h_size;
    param->dvpi_param.v_size                = m_mono_param.v_size;
    param->dvpi_param.vs_pol                = m_mono_param.vs_pol;
    param->dvpi_param.x_stride              = m_mono_param.x_stride;
    param->dvpi_param.y_stride              = m_mono_param.y_stride;

    return 0;
}

static int dvpi_ov7725_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = OV7725_W;
    param->height = OV7725_H;

    param->framerate = 30;

    param->format = CIS_FORMAT_RGB_YUYV;

    return 0;
}

static cis_dev_driver_t ov7725dvpi = {
    .name            = "ov7725dvpi",
    .i2c_num         = I2C_DEVICE_3,
    .power_pin       = GPIO_PIN0,
    .reset_pin       = GPIO_PIN0,
    .mclk_id         = CIS_MCLK_ID_MCLK0,
    .init            = cis_dvpi_ov7725_init,
    .start_stream    = sensor_ov7725_start,
    .power_on        = sensor_ov7725_power_on,
    .power_off       = sensor_ov7725_power_off,
    .reset           = sensor_ov7725_reset,
    .get_interface_param  = dvpi_ov7725_get_interface_param,
    .get_frame_parameter  = dvpi_ov7725_get_frame_parameter,
};

cis_dev_driver_t *cis_ov7725dvpi = &ov7725dvpi;

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_DVPI,     ov7725dvpi,    0,      &ov7725dvpi,    NULL);
