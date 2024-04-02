#include "cis_og01a1b.h"
#include "cis_dev_driver.h"
#include "aiva_utils.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"
#include "aiva_sleep.h"

#define SENSOR_ADDR_WR_MASTER               (0xc0 >> 1) 
#define SENSOR_ADDR_WR_SLAVE                (0x20 >> 1) //0x6c

#define TAG                      "cis_og01a1b"

#define W_VGA                    (1280)
#define H_VGA                    (800)

#define REG_DLY                  (0xffff)

// #define OUT_PUT_SIZE           og01a1b_1280_1024
// frame length is set by {0x380E, 0x380F}
// maximum exposure time is frame length - 14
// #define MAX_EXPOSURE (1818 - 25)
#define MAX_EXPOSURE 259


typedef struct _og01a1b_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
}og01a1b_REG_T;

static uint32_t og01a1b_i2c_addr = SENSOR_ADDR_WR_MASTER;

static og01a1b_REG_T og01a1b_start_regs[] __ATTR_ALIGN__(32) = {
	{0x0100, 0x01},
};

static og01a1b_REG_T og01a1b_stop_regs[] __ATTR_ALIGN__(32) = {
    {0x0100, 0x00},
};

static const og01a1b_REG_T og01a1b_common_regs[] __ATTR_ALIGN__(32) = {
{0x0103, 0x01},

/**************pll*********************/
{0x0300, 0x01},
{0x0301, 0x00},
{0x0302, 0x20},
{0x0303, 0x00},
{0x0304, 0x03},
{0x0305, 0x02},
{0x0306, 0x01},
{0x030a, 0x00},
{0x030b, 0x04},
{0x030c, 0x00},
{0x030d, 0x51},
{0x030e, 0x04},
{0x030f, 0x07},
{0x0312, 0x07},
{0x0313, 0x01},
{0x0314, 0x00},
{0x4837, 0x1F},
{0x3103, 0x00},
/**************pll_end*********************/

{0x3001, 0x00},
{0x3004, 0x00},
{0x3005, 0x00},
{0x3006, 0x04},
{0x3011, 0x0a},
{0x3013, 0x18},
{0x301c, 0xf0},
{0x3022, 0x01},
{0x3030, 0x10},
{0x3039, 0x32},
{0x303a, 0x00},
{0x3500, 0x00},
{0x3501, 0x2a},
{0x3502, 0x90},
{0x3503, 0x08},
{0x3505, 0x8c},
{0x3507, 0x03},
{0x3508, 0x00},
{0x3509, 0x10},
{0x3610, 0x80},
{0x3611, 0xa0},
{0x3620, 0x6e},
{0x3632, 0x56},
{0x3633, 0x78},
{0x3666, 0x00},
{0x366f, 0x5a},
{0x3680, 0x84},
{0x3712, 0x80},
{0x372d, 0x22},
{0x3731, 0x80},
{0x3732, 0x30},
{0x3778, 0x00},
{0x377d, 0x22},
{0x3788, 0x02},
{0x3789, 0xa4},
{0x378a, 0x00},
{0x378b, 0x4a},
{0x3799, 0x20},
{0x3800, 0x00},
{0x3801, 0x00},
{0x3802, 0x00},
{0x3803, 0x00},
{0x3804, 0x05},
{0x3805, 0x0f},
{0x3806, 0x03},
{0x3807, 0x2f},
{0x3808, 0x05},
{0x3809, 0x00},
{0x380a, 0x03},
{0x380b, 0x20},
{0x380c, 0x03},
{0x380d, 0xDE},//Line_length_pclk 1980
{0x380e, 0x07},
{0x380f, 0x1A},//Frame_length_line 1818
{0x3810, 0x00},
{0x3811, 0x08},
{0x3812, 0x00},
{0x3813, 0x08},
{0x3814, 0x11},
{0x3815, 0x11},
{0x3820, 0x40},
{0x3821, 0x00},
{0x382c, 0x05},
{0x382d, 0xb0},
{0x389d, 0x00},
{0x3881, 0x42},
{0x3882, 0x01},
{0x3883, 0x00},
{0x3885, 0x02},
{0x38a8, 0x02},
{0x38a9, 0x80},
{0x38b1, 0x00},
{0x38b3, 0x02},
{0x38c4, 0x00},
{0x38c5, 0xc0},
{0x38c6, 0x04},
{0x38c7, 0x80},
{0x3920, 0xff},
{0x4003, 0x40},
{0x4008, 0x04},
{0x4009, 0x0b},
{0x400c, 0x00},
{0x400d, 0x07},
{0x4010, 0x40},
{0x4043, 0x40},
{0x4307, 0x30},
{0x4317, 0x00},
{0x4501, 0x00},
{0x4507, 0x00},
{0x4509, 0x00},
{0x450a, 0x08},
{0x4601, 0x04},
{0x470f, 0x00},
{0x4f07, 0x00},
{0x4800, 0x00},
{0x5000, 0x9f},
{0x5001, 0x00},
{0x5e00, 0x00},
{0x5d00, 0x07},
{0x5d01, 0x00},
{0x4f00, 0x04},
{0x4f10, 0x00},
{0x4f11, 0x98},
{0x4f12, 0x0f},
{0x4f13, 0xc4},
{0x3662, 0x01},
{0x5000, 0x87},
{0x0100, 0x01},

{0x3501, 0x10},
{0x3502, 0x00},

};
// // Master
static const og01a1b_REG_T og01a1b_master_regs[] __ATTR_ALIGN__(32) = {
{0x3006, 0x06},  //02 02 ;enable FSIN output, bit operation, set 0x3006[1]=1
//{0x3006, 0x04},
{0x3823, 0x00},
};
// // Slave
static const og01a1b_REG_T og01a1b_slave_regs[] __ATTR_ALIGN__(32) = {
{0x3006, 0x04},   //00 02 ;disable FSIN output, bit operation, set 0x3006[1]=0
{0x3666, 0x00},  //00 0F ;[3:0] FSIN_i input, bit operation, clear 0x3666[3:0]
{0x38B3, 0x07},
{0x3885, 0x07},
{0x382B, 0x3A},
{0x3670, 0x68},
{0x3823, 0x30},   //[5] ext_vs_en ;//[4] r_init_man_en
{0x3824, 0x00},   //sclk -> cs counter reset value at vs_ext
{0x3825, 0x08},   //set it to 8
{0x3826, 0x07},   //03   ;//sclk -> r counter reset value at vs_ext
{0x3827, 0x16},   //8A   ;//vts = 'h38E as default, set r counter to vts-4

// {0x3826, 0x03},   //03   ;//sclk -> r counter reset value at vs_ext
// {0x3827, 0x8A},   //8A   ;//其vts = 'h38E as default, set r counter to vts-4

{0x3740, 0x01},
{0x3741, 0x00},
{0x3742, 0x08},

};


static const og01a1b_REG_T og01a1b_strobe_regs[] __ATTR_ALIGN__(32) = {
{0x3006, 0x0A},//
{0x3210, 0x10},//
{0x3007, 0x02},//
{0x301c, 0x22},//
{0x3020, 0x20},//
{0x3025, 0x02},//

/*******************需要区读取380~380d确认hts的值 2HTS***************************/
{0x382c, 0x07},// ; hts*2=03*2=07
{0x382d, 0xbc},// ; hts*2=de*2=bc
/****************************************************************************/

{0x3920, 0xff},//
{0x3923, 0x00},//
{0x3924, 0x00},//

/****************************需要根据3500～3502确认曝光时间*********************************/
// {0x3925, 0x00},//
// {0x3926, 0x00},//
// {0x3927, 0x00},//
// {0x3928, 0x80},//
{0x3925, 0x00},
{0x3926, 0x00},
{0x3927, 0x01},
{0x3928, 0x03},
/****************************************************************************************/

{0x392b, 0x00},//
{0x392c, 0x00},//

/*******************需要区读取380~380d确认hts的值 1HTS***************************/
{0x392d, 0x03},// ;hts
{0x392e, 0xde},// ;hts
/*******************需要区读取380~380d确认hts的值 1HTS***************************/

{0x392f, 0xcb},//
{0x38b3, 0x07},//
{0x3885, 0x07},//
{0x382b, 0x5a},//
{0x3670, 0x68},//Z
{0x3208, 0x00},//

/*****************************目前一个t-lin为36.6666667us,曝光时间是30ms,30000/t-lin =818***************************************************/
// {0x3501, 0x12},//;texposure
// {0x3502, 0x40},//;texposure
{0x3501, 0x10},//;texposure
{0x3502, 0x30},//;texposure
/**************************************************************************************************************************************/

{0x3508, 0x00},//;gain
{0x3509, 0x10},//;gain
{0x3929, 0x05},//;VTS-Texposure-7=71a-124-7=5ef
{0x392a, 0xef},//;VTS-Texposure-7=71a-124-7=5ef
{0x3208, 0x10},//
{0x3208, 0xa0},//
};


static const og01a1b_REG_T og01a1b_strobe_regs1[] __ATTR_ALIGN__(32) = {
{0x3006, 0x08},//
{0x3210, 0x10},//
{0x3007, 0x02},//
{0x301c, 0x22},//
{0x3020, 0x20},//
{0x3025, 0x02},//

/*******************需要区读取380~380d确认hts的值 2HTS***************************/
{0x382c, 0x07},// ; hts*2=03*2=07
{0x382d, 0xbc},// ; hts*2=de*2=bc
/****************************************************************************/

{0x3920, 0xff},//
{0x3923, 0x00},//
{0x3924, 0x00},//

/****************************需要根据3500～3502确认曝光时间*********************************/
// {0x3925, 0x00},//
// {0x3926, 0x00},//
// {0x3927, 0x00},//
// {0x3928, 0x80},//
{0x3925, 0x00},
{0x3926, 0x00},
{0x3927, 0x01},
{0x3928, 0x03},
/****************************************************************************************/

{0x392b, 0x00},//
{0x392c, 0x00},//

/*******************需要区读取380~380d确认hts的值 1HTS***************************/
{0x392d, 0x03},// ;hts
{0x392e, 0xde},// ;hts
/*******************需要区读取380~380d确认hts的值 1HTS***************************/

{0x392f, 0xcb},//
{0x38b3, 0x07},//
{0x3885, 0x07},//
{0x382b, 0x5a},//
{0x3670, 0x68},//
{0x3208, 0x00},//

/*****************************目前一个t-lin为36.6666667us,曝光时间是30ms,30000/t-lin =818***************************************************/
// {0x3501, 0x12},//;texposure
// {0x3502, 0x40},//;texposure
{0x3501, 0x10},//;texposure
{0x3502, 0x30},//;texposure
/**************************************************************************************************************************************/

{0x3508, 0x00},//;gain
{0x3509, 0x10},//;gain
{0x3929, 0x05},//;VTS-Texposure-7=71a-124-7=5ef
{0x392a, 0xef},//;VTS-Texposure-7=71a-124-7=5ef
{0x3208, 0x10},//
{0x3208, 0xa0},//
};

static void og01a1b_busy_delay (int32_t ms)
{
    aiva_msleep(ms);
}

static int og01a1b_write_reg(int i2c_num, uint8_t i2c_addr, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];

    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, i2c_addr, data_buf, 3);

    return ret;
}

static int og01a1b_read_reg(int i2c_num, uint8_t i2c_addr, uint16_t reg_addr, uint8_t *reg_val)
{
    int ret;

    uint8_t addr_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xff;
    addr_buf[1] = (reg_addr >> 0) & 0xff;
    
    ret = i2c_send_data(i2c_num, og01a1b_i2c_addr, &addr_buf[0], 2);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_recv_data(i2c_num, og01a1b_i2c_addr, 0, 0, reg_val, 1);
    
    return ret;
}
static int og01a1b_read_regs(
        int             i2c_num,
        uint8_t             i2c_addr,
        const og01a1b_REG_T    *reg_list,
        int             cnt
    )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = og01a1b_read_reg(i2c_num, i2c_addr, reg_list[i].reg_addr, reg_list[i].data);       
            // LOGE(__func__, "reg_addr 0x%x, data 0x%x.",
            //             reg_list[i].reg_addr, reg_list[i].data);
        }
        else {
            og01a1b_busy_delay(reg_list[i].data);
        }
    }
    return ret;
    
}

static int og01a1b_program_regs(
        int             i2c_num,
        uint8_t         i2c_addr,
        const og01a1b_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = og01a1b_write_reg(i2c_num, i2c_addr, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                        i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        }
        else {
            og01a1b_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static int og01a1b_i2c_test(int i2c_num, uint8_t i2c_addr)
{
    uint8_t id_h=0, id_m=0, id_l=0;
    int ret;

    ret = og01a1b_read_reg(i2c_num, i2c_addr, 0x300A, &id_h);   // 0x92
    if (ret < 0) {
        LOGE(__func__, "og01a1b_read_reg failed! (%d)", i2c_num);
        return ret;
    }

    ret = og01a1b_read_reg(i2c_num, i2c_addr, 0x300B, &id_m);   // 0x81
    if (ret < 0) {
        LOGE(__func__, "og01a1b_read_reg failed! (%d)", i2c_num);
        return ret;
    }
    // ret = og01a1b_read_reg(i2c_num, i2c_addr, 0x300C, &id_l); 
    // if (ret < 0) {
    //     LOGE(__func__, "og01a1b_read_reg failed! (%d)", i2c_num);
    //     return ret;
    // }

    if ((id_h != 0x92) && (id_m != 0x81)) {
        LOGI(TAG, "og01a1b_i2c_test (%d): readed id is 0x%x, 0x%x, 0x%x", i2c_num, id_h, id_m, id_l);
        LOGE(TAG, "og01a1b_i2c_test (%d): read id fail! expected 0x92, 0x81", i2c_num);
        return -1;
    }
   
    LOGD(TAG, "og01a1b_i2c_test (%d): read id success!", i2c_num);
    return 0;
}


static int og01a1b_init(const og01a1b_REG_T *reg_list, int cnt, 
                       int i2c_num, uint8_t i2c_addr)
{
    int ret;
    uint8_t addr_value;

    i2c_init(i2c_num, i2c_addr, 7, 350*1000);

    // LOGI(__func__, "i2c:%d, addr:%d.", i2c_num, i2c_addr);

    if (og01a1b_i2c_test(i2c_num, i2c_addr) < 0) {
        return -1;
    }

    ret = og01a1b_program_regs(i2c_num, i2c_addr, reg_list, cnt);
    if(ret != 0) {
        LOGE(TAG, "og01a1b_program_regs FAILED.");
    }

    // ret = og01a1b_read_reg(i2c_num, i2c_addr, 0x3006, &addr_value); 
    // LOGD(TAG, "addr_value: 0x%x.", addr_value);

    return ret;
}

static int og01a1b_vga_init(int i2c_num, uint8_t i2c_addr)
{
    int ret;
    ret = og01a1b_init(og01a1b_common_regs,
            AIVA_ARRAY_LEN(og01a1b_common_regs),
            i2c_num,
            i2c_addr
            );
    return ret;
}

static int cis_og01a1b_init(cis_dev_driver_t *dev_driver)
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

    if (og01a1b_vga_init(dev_driver->i2c_num, dev_driver->i2c_tar_addr) != 0) {
        LOGE(TAG, "og01a1b vga init fail\n");
        return -1;
    }
    return 0;
}
static int cis_og01a1b_init_master(cis_dev_driver_t *dev_driver)
{
    LOGI(TAG, "cis_og01a1b_init_maste\n");
    uint8_t i,result;
    int8_t ret;

    // dev_driver->i2c_num = I2C_DEVICE_2;
    // gpio_set_drive_mode(GPIO_PIN2, GPIO_DM_INPUT);//GPIO2 to input
    //og01a1b_i2c_addr = SENSOR_ADDR_WR_MASTER;
    // og01a1b_i2c_addr = SENSOR_ADDR_WR_SLAVE;
    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 27*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 27*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "master invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    if (og01a1b_vga_init(dev_driver->i2c_num, dev_driver->i2c_tar_addr) != 0) {
        LOGE(TAG, "og01a1b vga master init fail\n");
        return -1;
    }

    if (og01a1b_program_regs(dev_driver->i2c_num, dev_driver->i2c_tar_addr, og01a1b_master_regs, AIVA_ARRAY_LEN(og01a1b_master_regs)) != 0)
    {
        LOGE(TAG, "og01a1b master init fail\n");
        return -1;        
    }

    if (og01a1b_program_regs(dev_driver->i2c_num, dev_driver->i2c_tar_addr, og01a1b_strobe_regs, AIVA_ARRAY_LEN(og01a1b_strobe_regs)) != 0)
    {
        LOGE(TAG, "og01a1b strobe init fail\n");
        return -1;        
    } else {
        LOGI(TAG, "og01a1b master strobe init Success\n");

        for (i = 0; i < (sizeof(og01a1b_strobe_regs) / sizeof(og01a1b_REG_T)); i++) {
            og01a1b_read_reg( dev_driver->i2c_num, 
                            dev_driver->i2c_tar_addr, 
                            og01a1b_strobe_regs[i].reg_addr,
                            &result);
            // LOGI(TAG, "i2c read reg 0x%02x", result);
        }
    }

    return 0;
}
static int cis_og01a1b_init_slave(cis_dev_driver_t *dev_driver)
{
    uint8_t val=0;
    int ret;
    LOGI(TAG, "cis_og01a1b_init_slave\n");
    uint8_t i2c_addr = dev_driver->i2c_tar_addr;

    // dev_driver->i2c_num = I2C_DEVICE_2;
    // og01a1b_i2c_addr = SENSOR_ADDR_WR_MASTER;
    // gpio_set_drive_mode(GPIO_PIN2, GPIO_DM_INPUT);//GPIO2 to input
    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 27*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 27*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "slave invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    if (og01a1b_vga_init(dev_driver->i2c_num, i2c_addr) != 0) {
        LOGE(TAG, "og01a1b vga slave init fail\n");
        return -1;
    }

    if (og01a1b_program_regs(dev_driver->i2c_num, i2c_addr, og01a1b_slave_regs, AIVA_ARRAY_LEN(og01a1b_slave_regs)) != 0)
    {
        LOGE(TAG, "og01a1b slave init fail\n");
        return -1;        
    }

    if (og01a1b_program_regs(dev_driver->i2c_num, i2c_addr, og01a1b_strobe_regs1, AIVA_ARRAY_LEN(og01a1b_strobe_regs1)) != 0)
    {
        LOGE(TAG, "og01a1b strobe init fail\n");
        return -1;        
    } else {
        LOGI(TAG, "og01a1b slave strobe init Success\n");
    }


    //test
    // ret = og01a1b_read_reg(dev_driver->i2c_num, i2c_addr, 0x3500, &val); 
    // LOGD(TAG, "test: readed 0x3500 addr  is 0x%x", val);
    // ret = og01a1b_read_reg(dev_driver->i2c_num, i2c_addr, 0x3501, &val); 
    // LOGD(TAG, "test: readed 0x3501 addr  is 0x%x", val);
    // ret = og01a1b_read_reg(dev_driver->i2c_num, i2c_addr, 0x3502, &val); 
    // LOGD(TAG, "test: readed 0x3502 addr  is 0x%x", val);

    // ret = og01a1b_read_reg(dev_driver->i2c_num, i2c_addr, 0x380c, &val); 
    // LOGD(TAG, "test: readed 0x380c addr  is 0x%x", val);
    // ret = og01a1b_read_reg(dev_driver->i2c_num, i2c_addr, 0x380d, &val); 
    // LOGD(TAG, "test: readed 0x380d addr  is 0x%x", val);

    return 0;
}

static int cis_ov01a1b_wake()
{
    return 0;
}

static int cis_ov01a1b_sleep()
{
    return 0;
}

static int cis_og01a1b_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    og01a1b_REG_T        *reg_list;
    int                 cnt;

    uint8_t i2c_num = dev_driver->i2c_num;
    uint8_t i2c_addr = dev_driver->i2c_tar_addr;

    reg_list = og01a1b_start_regs;
    cnt     = AIVA_ARRAY_LEN(og01a1b_start_regs);
    ret     = og01a1b_program_regs(i2c_num, i2c_addr, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "og01a1b_start failed!");
    }
    return ret;
}

static int cis_og01a1b_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    og01a1b_REG_T        *reg_list;
    int                 cnt;

    int i2c_num = dev_driver->i2c_num;
    int i2c_addr = dev_driver->i2c_tar_addr;

    reg_list = og01a1b_stop_regs;
    cnt     = AIVA_ARRAY_LEN(og01a1b_stop_regs);
    ret     = og01a1b_program_regs(i2c_num, i2c_addr, reg_list, cnt);
    if (ret < 0) {
        LOGE(__func__, "og01a1b_stop failed!");
    }
    return ret;
}
static void cis_og01a1b_power_on(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    og01a1b_busy_delay(50);
    gpio_set_pin(pow_pin, GPIO_PV_HIGH);
    og01a1b_busy_delay(50);
    return;
}

static void cis_og01a1b_power_off(cis_dev_driver_t *dev_driver)
{
    int pow_pin = dev_driver->power_pin;

    gpio_set_drive_mode(pow_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(pow_pin, GPIO_PV_LOW);
    og01a1b_busy_delay(50);
    return;
}

static void cis_og01a1b_reset(cis_dev_driver_t *dev_driver)
{
    return;
}

static int cis_og01a1b_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_MIPI;
    param->mipi_param.freq                  = 512;
    param->mipi_param.lane_num              = 1;
    param->mipi_param.vc_num                = 1;
    param->mipi_param.virtual_channels[0]   = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type             = MIPI_DATA_TYPE_RAW_10;
    return 0;
}

static int cis_og01a1b_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    exp_param->min_again    = 1.0;
    exp_param->max_again    = 248;
    exp_param->step_again   = 1;

    exp_param->min_dgain    = 1.0;
    exp_param->max_dgain    = 15.0;
    exp_param->step_dgain   = 1.0;

    exp_param->min_itime    = 1.0;
    exp_param->max_itime    = MAX_EXPOSURE;
    exp_param->step_itime   = 1.0;
    return 0;
}

static int cis_og01a1b_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    i2c_device_number_t i2c_num = dev_driver->i2c_num;
    uint8_t i2c_addr = dev_driver->i2c_tar_addr;
    // og01a1b_i2c_addr = SENSOR_ADDR_WR_MASTER;
    float again = exp->again;
    float dgain = exp->dgain;
    float itime = exp->itime;

    i2c_init(i2c_num, i2c_addr, 7, 350*1000);


    // exposure
    uint8_t itime_h;
    uint8_t itime_l;
    uint32_t exposure = (int)itime;
    if (exposure > MAX_EXPOSURE)
        exposure = MAX_EXPOSURE;
    itime_l = exposure & 0xff;
    itime_h = (exposure >> 8) & 0xff;
    og01a1b_write_reg(i2c_num, i2c_addr, 0x3502, itime_l);
    og01a1b_write_reg(i2c_num, i2c_addr, 0x3501, itime_h);

    // again
    uint8_t again_coarse = (int)again;
    if (again_coarse > 0xf8)
        again_coarse = 0xf8;
    og01a1b_write_reg(i2c_num, i2c_addr, 0x3509, again_coarse);


    // dgain no this.
    // uint8_t dgain_coarse = (int)dgain;
    // if (dgain_coarse > 0xf)
    //     dgain_coarse = 0xf;
    // og01a1b_write_reg(i2c_num, i2c_addr, 0x350a, dgain_coarse);

    return 0;
}

static int cis_og01a1b_get_resolution(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = W_VGA;
    param->height = H_VGA;
    param->framerate = 15;
    param->format = CIS_FORMAT_RGB_BAYER10;

    return 0;
}

static cis_dev_driver_t og01a1b_dev0 = {
    .name                   = "og01a1b_dev0",
    .i2c_num                = I2C_DEVICE_0,
    .power_pin              = GPIO_PIN2,
    .reset_pin              = GPIO_PIN2,
    .mclk_id                = CIS_MCLK_ID_MCLK0,
    .context                = NULL,
    .init                   = cis_og01a1b_init,
    .start_stream           = cis_og01a1b_start_stream,
    .stop_stream            = cis_og01a1b_stop_stream,
    .wake                   = cis_ov01a1b_wake,
    .sleep                  = cis_ov01a1b_sleep,
    .power_on               = cis_og01a1b_power_on,
    .power_off              = cis_og01a1b_power_off,
    .reset                  = cis_og01a1b_reset,
    .get_interface_param    = cis_og01a1b_get_interface_param,
    .get_exposure_param     = cis_og01a1b_get_exposure_param,
    .set_exposure           = cis_og01a1b_set_exposure,
    .get_frame_parameter    = cis_og01a1b_get_resolution
};

static cis_dev_driver_t og01a1b_dev1 = {
    .name                   = "og01a1b_dev1",
    .i2c_num                = I2C_DEVICE_2,
    .i2c_tar_addr           = SENSOR_ADDR_WR_SLAVE,
    .power_pin              = GPIO_PIN2,
    .reset_pin              = GPIO_PIN2,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    .init                   = cis_og01a1b_init_slave,
    .start_stream           = cis_og01a1b_start_stream,
    .stop_stream            = cis_og01a1b_stop_stream,
    .wake                   = cis_ov01a1b_wake,
    .sleep                  = cis_ov01a1b_sleep,
    .power_on               = cis_og01a1b_power_on,
    .power_off              = cis_og01a1b_power_off,
    .reset                  = cis_og01a1b_reset,
    .get_interface_param    = cis_og01a1b_get_interface_param,
    .get_exposure_param     = cis_og01a1b_get_exposure_param,
    .set_exposure           = cis_og01a1b_set_exposure,
    .get_frame_parameter    = cis_og01a1b_get_resolution
};


static cis_dev_driver_t og01a1b_dev2 = {
    .name                   = "og01a1b_dev2",
    .i2c_num                = I2C_DEVICE_2,
	.i2c_tar_addr           = SENSOR_ADDR_WR_MASTER,
    .power_pin              = GPIO_PIN2,
    .reset_pin              = GPIO_PIN2,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .mclk_freq              = 24 * 1000 * 1000,
    .fps                    = 30,
    .mf_mode                = 0,
    .context                = NULL,
    .init                   = cis_og01a1b_init_master,
    .start_stream           = cis_og01a1b_start_stream,
    .stop_stream            = cis_og01a1b_stop_stream,
    .wake                   = cis_ov01a1b_wake,
    .sleep                  = cis_ov01a1b_sleep,
    .power_on               = cis_og01a1b_power_on,
    .power_off              = cis_og01a1b_power_off,
    .reset                  = cis_og01a1b_reset,
    .get_interface_param    = cis_og01a1b_get_interface_param,
    .get_exposure_param     = cis_og01a1b_get_exposure_param,
    .set_exposure           = cis_og01a1b_set_exposure,
    .get_frame_parameter    = cis_og01a1b_get_resolution
};

cis_dev_driver_t *cis_og01a1b[] = {
    &og01a1b_dev0,
    &og01a1b_dev1,
    &og01a1b_dev2
};

/* L18/L18-MINI */
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og01a1b_dev0,    0,      &og01a1b_dev0,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og01a1b_dev1,    0,      &og01a1b_dev1,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     og01a1b_dev2,    0,      &og01a1b_dev2,    NULL);