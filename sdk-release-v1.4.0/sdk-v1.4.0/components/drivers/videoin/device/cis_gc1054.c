#include <math.h>
#include "cis_gc1054.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define REG_DLY                  (0xffff)
#define SENSOR_ADDR_WR           (0x42 >> 1)


typedef struct _GC1054_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
} GC1054_REG_T;


static const char* TAG = "cis_gc1054";

static GC1054_REG_T gc1054_start_regs[] __ATTR_ALIGN__(32) = {

};

static GC1054_REG_T gc1054_stop_regs[] __ATTR_ALIGN__(32) = {

};

// 1280x720: mclk = 24M, MIPI data rate=312Mhz, raw10
static const GC1054_REG_T gc1054_720p[] __ATTR_ALIGN__(32) = {
//Actual_window_size=1280*720,MIPI 1lane
//Mclk=24Mhz，MIPI data rate=312Mhz,Frame_rate=30fps
//HD=1726,VD=750,row_time=44.25us
	/////////////////////////////////////////////////////
	//////////////////////   SYS   //////////////////////
	/////////////////////////////////////////////////////
	{0xf2,0x00},
	{0xf6,0x00},
	{0xfc,0x04},
	{0xf7,0x01},
	{0xf8,0x0c},
	{0xf9,0x06},
	{0xfa,0x80},
	{0xfc,0x0e},
	///////////////////////////////////////////
	/////   ANALOG & CISCTL   ////////////////
	///////////////////////////////////////////
	{0xfe,0x00},
//	{0x03,0x02}, 
//	{0x04,0xa6}, 
	{0x03,0x04}, // exposure[12:8]
	{0x04,0xa6}, // exposure[7:0]

	{0x05,0x02}, //HB
	{0x06,0x07},
	{0x07,0x00}, //VB
	{0x08,0xa4}, 
	{0x09,0x00},
	{0x0a,0x04}, //row start
	{0x0b,0x00},
	{0x0c,0x00}, //col start
	{0x0d,0x02}, 
	{0x0e,0xd4}, //height 724
	{0x0f,0x05}, 
	{0x10,0x08}, //width 1288
	{0x17,0xc0},
	{0x18,0x02},
	{0x19,0x08},
	{0x1a,0x18},
	{0x1e,0x50},
	{0x1f,0x80},
	{0x21,0x30},
	{0x23,0xf8},
	{0x25,0x10},
	{0x28,0x20},
	{0x34,0x08}, //data low
	{0x3c,0x10},
	{0x3d,0x0e},
	{0xcc,0x8e},
	{0xcd,0x9a},
	{0xcf,0x70},
	{0xd0,0xa9},
	{0xd1,0xc5},
	{0xd2,0xed}, //data high
	{0xd8,0x3c}, //dacin offset
	{0xd9,0x7a},
	{0xda,0x12},
	{0xdb,0x50},
	{0xde,0x0c},
	{0xe3,0x60},
	{0xe4,0x78},
	{0xfe,0x01},
	{0xe3,0x01},
	{0xe6,0x10}, //ramps offset
	///////////////////////////////////////////
	////////////   ISP   //////////////////////
	///////////////////////////////////////////
	{0xfe,0x01},
	{0x80,0x50},
	{0x88,0x73},
	{0x89,0x03},
	{0x90,0x01}, 
	{0x92,0x02}, //crop win 2<=y<=4
	{0x94,0x03}, //crop win 2<=x<=5
	{0x95,0x02}, //crop win height
	{0x96,0xd0},
	{0x97,0x05}, //crop win width
	{0x98,0x00},
	///////////////////////////////////////////
	////////////   BLK   //////////////////////
	///////////////////////////////////////////
	{0xfe,0x01},
	{0x40,0x22},
	{0x43,0x03},
	{0x4e,0x3c},
	{0x4f,0x00},
	{0x60,0x00},
	{0x61,0x80},
	///////////////////////////////////////////
	////////////   GAIN   /////////////////////
	///////////////////////////////////////////
	{0xfe,0x01},
	{0xb0,0x48},
//	{0xb0,0x98}, // global gain

	{0xb1,0x01}, 
	{0xb2,0x00}, 
	{0xb6,0x00}, 
	{0xfe,0x02},
	{0x01,0x00},
	{0x02,0x01},
	{0x03,0x02},
	{0x04,0x03},
	{0x05,0x04},
	{0x06,0x05},
	{0x07,0x06},
	{0x08,0x0e},
	{0x09,0x16},
	{0x0a,0x1e},
	{0x0b,0x36},
	{0x0c,0x3e},
	{0x0d,0x56},
	{0xfe,0x02},
	{0xb0,0x00}, //col_gain[11:8]
	{0xb1,0x00},
	{0xb2,0x00},
	{0xb3,0x11},
	{0xb4,0x22},
	{0xb5,0x54},
	{0xb6,0xb8},
	{0xb7,0x60},
	{0xb9,0x00}, //col_gain[12]
	{0xba,0xc0},
	{0xc0,0x20}, //col_gain[7:0]
	{0xc1,0x2d},
	{0xc2,0x40},
	{0xc3,0x5b},
	{0xc4,0x80},
	{0xc5,0xb5},
	{0xc6,0x00},
	{0xc7,0x6a},
	{0xc8,0x00},
	{0xc9,0xd4},
	{0xca,0x00},
	{0xcb,0xa8},
	{0xcc,0x00},
	{0xcd,0x50},
	{0xce,0x00},
	{0xcf,0xa1},
	///////////////////////////////////////////
	//////////   DARKSUN   ////////////////////
	///////////////////////////////////////////
	{0xfe,0x02},
	{0x54,0xf7},
	{0x55,0xf0},
	{0x56,0x00},
	{0x57,0x00},
	{0x58,0x00},
	{0x5a,0x04},
	///////////////////////////////////////////
	/////////////   DD   //////////////////////
	///////////////////////////////////////////
	{0xfe,0x04},
	{0x81,0x8a},
	///////////////////////////////////////////
	////////////	 MIPI	/////////////////////
	///////////////////////////////////////////
	{0xfe,0x03},
	{0x01,0x03},
	{0x02,0x11},
	{0x03,0x90},
	{0x10,0x90},
	{0x11,0x2b},
	{0x12,0x40}, //lwc 1280*5/4
	{0x13,0x06},

	{0x15,0x02},
	{0x21,0x05},
	{0x22,0x01},
	{0x23,0x06},  //0x10
	{0x24,0x02},
	{0x25,0x10},
	{0x26,0x03},
	{0x29,0x01},
	{0x2a,0x05},
	{0x2b,0x03},
	{0xfe,0x00},
};

static void gc1054_busy_delay(int32_t ms)
{
    aiva_msleep(ms);
}

static int gc1054_write_reg(int i2c_num, uint8_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[2];
    data_buf[0] = reg_addr;
    data_buf[1] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 2);
    /*ret = i2c_send_data_dma(DMAC_CHANNEL0, i2c_num, data_buf, 2);*/
    return ret;
}

static int gc1054_read_reg(int i2c_num, uint8_t reg_addr, uint8_t *reg_val)
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

static int gc1054_program_regs(
        int             i2c_num,
        const GC1054_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = gc1054_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                        i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else {
            gc1054_busy_delay(reg_list[i].data);
        }
    }
    return ret;    
}

static uint16_t gc1054_get_sensor_id(i2c_device_number_t i2c_num)
{
    uint8_t id_h = 0;
    uint8_t id_l = 0;
    
    gc1054_read_reg(i2c_num, 0xf0, &id_h);
    gc1054_read_reg(i2c_num, 0xf1, &id_l);

    return ((id_h << 8) | id_l);
}

static int gc1054_i2c_test(int i2c_num)
{
    uint16_t val;

    val = gc1054_get_sensor_id(i2c_num);
    LOGD(TAG, "gc1054 readed id is 0x%4x", val);


    if (val != 0x1054) {
        LOGE(TAG, "gc1054_i2c_test: read id fail!, get 0x%4x, expected 0x1054", val);
        return -1;
    }
   
    LOGD(TAG, "gc1054_i2c_test: read id success!");
    return 0;
}


static int gc1054_init(const GC1054_REG_T *reg_list, int cnt, int i2c_num)
{
    int ret;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 350*1000);

    if (gc1054_i2c_test(i2c_num) < 0) {
        LOGE(TAG, "gc1054 i2c test fail\n");
        return -1;
    }

    ret = gc1054_program_regs(i2c_num, reg_list, cnt);

    /*LOGD(TAG, "gc1054_init() return: %d.", ret);*/
    if (ret < 0) {
        LOGE(__func__, "line %d, gc1054_program_regs failed!", __LINE__);
        return ret;
    }
    return ret;
}

static int gc1054_720p_init(int i2c_num)
{
    int ret;
    ret = gc1054_init(gc1054_720p, AIVA_ARRAY_LEN(gc1054_720p), i2c_num);
    return ret;
}

static int cis_gc1054_init(cis_dev_driver_t *dev_driver)
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

    if (gc1054_720p_init(dev_driver->i2c_num) != 0) {
        LOGE(TAG, "gc1054_init_1080p fail\n");
        return -1;
    }
    return 0;
}

static int cis_gc1054_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    GC1054_REG_T       *reg_list;
    int                 cnt;

    reg_list = gc1054_start_regs;
    cnt     = AIVA_ARRAY_LEN(gc1054_start_regs);
    ret     = gc1054_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(TAG, "gc1054_start failed!");
    }
    return ret;
}


static int cis_gc1054_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    GC1054_REG_T       *reg_list;
    int                 cnt;

    reg_list = gc1054_stop_regs;
    cnt     = AIVA_ARRAY_LEN(gc1054_stop_regs);
    ret     = gc1054_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(TAG, "gc1054_stop failed!");
    }
    return ret;
}


static void cis_gc1054_power_on(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    gc1054_busy_delay(3);
    gpio_set_pin(power_pin, GPIO_PV_HIGH);
    gc1054_busy_delay(5);
    return;
}


static void cis_gc1054_power_off(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    gc1054_busy_delay(5);
    return;
}

static void cis_gc1054_reset(cis_dev_driver_t *dev_driver)
{
    return;
}

static int cis_gc1054_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq                  = 312;
    param->mipi_param.lane_num              = 1;
    param->mipi_param.vc_num                = 1;
    param->mipi_param.virtual_channels[0]   = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type             = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

static int cis_gc1054_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    /*exp_param->min_again[0]    = 1.0;*/
    /*exp_param->max_again[0]    = 1.0;*/
    /*exp_param->step_again[0]   = 0;*/

    /*exp_param->min_dgain[0]    = 1.0;*/
    /*exp_param->max_dgain[0]    = 128;*/
    /*exp_param->step_dgain[0]   = 1/64.;*/
    
    exp_param->min_again    = 1.0;
    /*exp_param->max_again    = 128.;*/
    exp_param->max_again    = 16.;
    exp_param->step_again   = 1/64.;

    exp_param->min_dgain    = 1.0;
    exp_param->max_dgain    = 1.0;
    exp_param->step_dgain   = 1.0;

    exp_param->min_itime    = 0;
    exp_param->max_itime    = 0x1fff;
    exp_param->step_itime   = 1;

    return 0;
}

static int cis_gc1054_mapping_itime(float itime, uint8_t *itime_h, uint8_t *itime_l)
{
    int itime_lines;

    if (itime > 10.0) {
        // for itime > 10.0, we suppose it is represet in lines
        if (itime > 0x1fff) {
            itime = 0x1fff;
        }        
        itime_lines = itime;
    } else {
        // for itime <= 10.0l we suppose it is represet in seconds
        float mipi_data_rate = 312 * 1000000;       // 312Mbps
        float mipi_lane_num  = 1;                   // 1 lane
        float mipi_data_bit  = 10;                  // raw 10
        int   line_length    = 1726;
        float pclk           = mipi_data_rate * mipi_lane_num / mipi_data_bit;
        float T_pclk         = 1/pclk;
        float T_line         = line_length * T_pclk;
        itime_lines    = (int)round(itime / T_line);
    }

    *itime_l = (itime_lines >> 0) & 0xff;
    *itime_h = (itime_lines >> 8) & 0x1f;

    return 0;
}

static uint32_t m_gain_level_tbl[12] = {
								64, 
								91, 
								127, 
								182, 
								258, 
								369, 
								516, 
								738, 
								1032, 
								1491, 
								2084, 
								0xffffffff
};

static int m_gain_level_cnt = sizeof(m_gain_level_tbl) / sizeof(uint32_t);
					
static int cis_gc1054_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 350*1000);

    /*LOGD(__func__, "set again %f, dgain %f, itime %f", */
            /*exp->again[0], exp->dgain[0], exp->itime[0]);*/
    
    // re-calculate total gain, 6-bit fractional
    uint32_t gain = exp->again * exp->dgain * 64;
    if (gain > 128*64) {
        gain = 128*64;
    }
	int analog_idx;
	uint32_t tol_dig_gain = 0;	

	for(analog_idx = 0; analog_idx < m_gain_level_cnt; analog_idx++) {
		if((m_gain_level_tbl[analog_idx] <= gain)&&(gain < m_gain_level_tbl[analog_idx+1]))
			break;
	}	

	tol_dig_gain = gain * 64 / m_gain_level_tbl[analog_idx];	

    gc1054_write_reg(i2c_num, 0xfe, 0x01);
    // {0xb6},  again[3:0]
    gc1054_write_reg(i2c_num, 0xb6, analog_idx);
    // {0xb1},  dgain, integer part
    // {0xb2},  dgain, fractional part, 6-bits (64)
    gc1054_write_reg(i2c_num, 0xb1, (tol_dig_gain>>6));
    gc1054_write_reg(i2c_num, 0xb2, ((tol_dig_gain&0x3f)<<2));
    
    // expo
	// {0xfe,0x00},
	// {0x03}, // exposure[12:8]
	// {0x04}, // exposure[7:0]
    uint8_t itime_h;
    uint8_t itime_l;
    float itime = exp->itime;

    cis_gc1054_mapping_itime(itime, &itime_h, &itime_l);
    gc1054_write_reg(i2c_num, 0xfe, 0x00);
    gc1054_write_reg(i2c_num, 0x03, itime_h);
    gc1054_write_reg(i2c_num, 0x04, itime_l);

    return 0;
}

static int cis_gc1054_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = 1280;
    param->height = 720;
    param->framerate = 30;
    param->format = CIS_FORMAT_RGB_BAYER10;
    param->bayer_pat = CIS_BAYER_PAT_RGGB;

    return 0;
}

static cis_dev_driver_t gc1054_dev0 = {
    .name                   = "gc1054_dev0",
    .i2c_num                = I2C_DEVICE_0,
    .power_pin              = GPIO_PIN0,
    .reset_pin              = GPIO_PIN0,
    .mclk_id                = CIS_MCLK_ID_MCLK0,
    .context                = NULL,
    .init                   = cis_gc1054_init,
    .start_stream           = cis_gc1054_start_stream,
    .stop_stream            = cis_gc1054_stop_stream,
    .power_on               = cis_gc1054_power_on,
    .power_off              = cis_gc1054_power_off,
    .reset                  = cis_gc1054_reset,
    .get_interface_param    = cis_gc1054_get_interface_param,
    .get_exposure_param     = cis_gc1054_get_exposure_param,
    .set_exposure           = cis_gc1054_set_exposure,
    .get_frame_parameter         = cis_gc1054_get_frame_parameter
};

static cis_dev_driver_t gc1054_dev1 = {
    .name                   = "gc1054_dev1",
    .i2c_num                = I2C_DEVICE_1,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_gc1054_init,
    .start_stream           = cis_gc1054_start_stream,
    .stop_stream            = cis_gc1054_stop_stream,
    .power_on               = cis_gc1054_power_on,
    .power_off              = cis_gc1054_power_off,
    .reset                  = cis_gc1054_reset,
    .get_interface_param    = cis_gc1054_get_interface_param,
    .get_exposure_param     = cis_gc1054_get_exposure_param,
    .set_exposure           = cis_gc1054_set_exposure,
    .get_frame_parameter         = cis_gc1054_get_frame_parameter
};

static cis_dev_driver_t gc1054_dev2 = {
    .name                   = "gc1054_dev2",
    .i2c_num                = I2C_DEVICE_2,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_gc1054_init,
    .start_stream           = cis_gc1054_start_stream,
    .stop_stream            = cis_gc1054_stop_stream,
    .power_on               = cis_gc1054_power_on,
    .power_off              = cis_gc1054_power_off,
    .reset                  = cis_gc1054_reset,
    .get_interface_param    = cis_gc1054_get_interface_param,
    .get_exposure_param     = cis_gc1054_get_exposure_param,
    .set_exposure           = cis_gc1054_set_exposure,
    .get_frame_parameter         = cis_gc1054_get_frame_parameter
};

cis_dev_driver_t *cis_gc1054[] = {
    &gc1054_dev0,
    &gc1054_dev1,
    &gc1054_dev2
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     gc1054_dev0,    0,      &gc1054_dev0,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     gc1054_dev1,    0,      &gc1054_dev1,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     gc1054_dev2,    0,      &gc1054_dev2,    NULL);
