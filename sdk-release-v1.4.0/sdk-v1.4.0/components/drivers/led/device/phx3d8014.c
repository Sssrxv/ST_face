#include "led_driver.h"

#include "phx3d8014.h"
#include "aiva_utils.h"
#include "syslog.h"
#include "gpio.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include <string.h>
#include <math.h>
#include "udevice.h"
#include "pwm.h"


/* =================================================================================== */
#define TAG                             "PHX3D8014"
#define phx3d8014_SLAVE_ADDR              (0xD8>>1)
#define phx3d8014_DEVICE_ID               (0xC1)

#define phx3d8014_CH1_CURRENT_REG           0x80        //对应led ch0
#define phx3d8014_CH2_CURRENT_REG           0x48        //对应led ch1
#define STEP_CURRENT                        12.8  

// #define phx3d8014_CURRENT_CFG             4000
#define phx3d8014_CURRENT_CFG             300

#define phx3d8014_IIC_NUM                 (I2C_DEVICE_1)
#define phx3d8014_SCL                     (350000)
#define phx3d8014_IIC_ADDR_WIDTH          (7)
/* =================================================================================== */
#define phx3d8014_REG_ADDR00              (0x00)
#define phx3d8014_REG_ADDR01              (0x01)
#define phx3d8014_REG_ADDR02              (0x02)
#define phx3d8014_REG_ADDR03              (0x03)
#define phx3d8014_REG_ADDR04              (0x04)
#define phx3d8014_REG_ADDR05              (0x05)
#define phx3d8014_REG_ADDR06              (0x06)
#define phx3d8014_REG_ADDR07              (0x07)
#define phx3d8014_REG_ADDR08              (0x08)
#define phx3d8014_REG_ADDR09              (0x09)
#define phx3d8014_REG_ADDR0A              (0x0A)
#define phx3d8014_REG_ADDR0B              (0x0B)
#define phx3d8014_REG_ADDR0C              (0x0C)
#define phx3d8014_REG_ADDR0D              (0x0D)
#define phx3d8014_REG_ADDR0E              (0x0E)
#define phx3d8014_REG_ADDR0F              (0x0F)
#define phx3d8014_REG_ADDR10              (0x10)
#define phx3d8014_REG_ADDR11              (0x11)
#define phx3d8014_REG_ADDR12              (0x12)
#define phx3d8014_REG_ADDR13              (0x13)
#define phx3d8014_REG_ADDR14              (0x14)
#define phx3d8014_REG_ADDR15              (0x15)
#define phx3d8014_REG_ADDR16              (0x16)
#define phx3d8014_REG_ADDR17              (0x17)
#define phx3d8014_REG_ADDR18              (0x18)
#define phx3d8014_REG_ADDR19              (0x19)
/* =================================================================================== */
#define IB_FIX_MAX          (0x34)//212mA
#define IB_FIX_2_CURRENT(x) ((x + 1) * 4)
#define CURRENT_2_IB_FIX(x) ((x) / 4 - 1)
// #define CURRENT_2_IB_FIX(x) (x / 4 - 1)

#define ISW_MAX             (0xFF)//4096mA
#define ISW_2_CURRENT(x)    ((x + 1) * 16)
#define CURRENT_2_ISW(x)    ((x) / 16 - 1)
// #define CURRENT_2_ISW(x)    (x / 16 - 1)
/* =================================================================================== */
typedef enum _led_num_t {
    LED_0 = 0,
    LED_1,
    LED_MAX,
} led_num_t;

typedef struct _phx3d8014_CONFIG_T {
    uint8_t     reg_addr;                             // Register address
    uint8_t     data;                                 // Data
}phx3d8014_CONFIG_T;

enum phx3d8014_time_out_val {
    PHX3D8014_TIMEOUT_10MS   = 0,
    PHX3D8014_TIMEOUT_20MS   = 1,
    PHX3D8014_TIMEOUT_30MS   = 2,
    PHX3D8014_TIMEOUT_40MS   = 3,
    PHX3D8014_TIMEOUT_50MS   = 4,
    PHX3D8014_TIMEOUT_60MS   = 5,
    PHX3D8014_TIMEOUT_70MS   = 6,
    PHX3D8014_TIMEOUT_80MS   = 7,
    PHX3D8014_TIMEOUT_90MS   = 8,
    PHX3D8014_TIMEOUT_100MS  = 9,
    PHX3D8014_TIMEOUT_150MS  = 10,
    PHX3D8014_TIMEOUT_200MS  = 11,
    PHX3D8014_TIMEOUT_250MS  = 12,
    PHX3D8014_TIMEOUT_300MS  = 13,
    PHX3D8014_TIMEOUT_350MS  = 14,
    PHX3D8014_TIMEOUT_400MS  = 15,
};

const static phx3d8014_CONFIG_T gphx3d8014RegInitTbl[] = {
{0x13, 0x38}, ///pin D4 is used as SDA / Pin D4 is used as XEMOUT
{0x3B, 0xFF}, //Mask ch1 error
{0x3C, 0xFF}, //Mask ch1 error
{0x6B, 0xFF}, //Mask ch2 error
{0x6C, 0xFF}, //Mask ch2 error
{0x2F, 0x10}, //Enable LVDS Termination. Enable Fast Ramp-up ch1

{0x00, 0x10}, //ACC Mode ch1
{0x40, 0x10}, //ACC Mode ch2
{0x07, 0x00}, //ibias_fix_ch1
{0x08, 0xc8}, //isw_fix_ch1 12.8mA*0x90=1843mA

{0x47, 0x00}, //ibias_fix_ch2

{0x48, 0xc8}, //isw_fix_ch2
{0x7F, 0x08}  //adc reset 
};

#define phx3d8014_INIT_TBL_SIZE   (sizeof(gphx3d8014RegInitTbl) / sizeof(phx3d8014_CONFIG_T))
/* =================================================================================== */
// static i2c_device_number_t gphx3d8014I2cNum = phx3d8014_IIC_NUM;
static int gphx3d8014PowerPin = GPIO_PIN8;
static uint8_t gphx3d8014InitFlag = 0;
static uint8_t gStrobeEnable = 0;
static uint8_t gLedEnable[] = {0, 0};
static pwm_device_number_t m_pwm_dev = PWM_DEVICE_1;

static int gSuspendFlag = 0;
/* =================================================================================== */

static int phx3d8014_get_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_lock(&(((phx3d8014_led_t*)dev->priv)->mutex));
    }
#endif
    return ret;
}

static int phx3d8014_release_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_unlock(&(((phx3d8014_led_t*)dev->priv)->mutex));
    }
#endif
    return ret;
}

/* =================================================================================== */
static int phx3d8014_WriteReg(i2c_device_number_t dev, uint8_t reg, uint8_t data)
{
    int ret;
    uint8_t data_buf[2];

    data_buf[0] = reg;
    data_buf[1] = data;
    ret = i2c_send_data(dev, phx3d8014_SLAVE_ADDR, data_buf, 2);
    // configASSERT(ret == 0);
    if (ret < 0) {
        LOGE(TAG, "i2c write reg 0x%02x error", reg);
    }

    return ret;
}
/* =================================================================================== */
static uint8_t phx3d8014_readReg(i2c_device_number_t dev, uint8_t reg_addr)
{
    int ret;
    uint8_t reg_val;

    ret = i2c_send_data(dev, phx3d8014_SLAVE_ADDR, &reg_addr, 1);
    // configASSERT(ret == 0);
    if (ret < 0) {
        LOGE(TAG, "i2c read reg 0x%02x error", reg_addr);
    }

    ret = i2c_recv_data(dev, phx3d8014_SLAVE_ADDR, 0, 0, &reg_val, 1);
    // configASSERT(ret == 0);
    if (ret < 0) {
        LOGE(TAG, "i2c read reg 0x%02x error", reg_addr);
    }

    return reg_val;
}
/* =================================================================================== */

static int phx3d8014_pwm_init(void)
{
    pwm_init(m_pwm_dev);
    pwm_set_enable(m_pwm_dev, 0);
    pwm_set_frequency(m_pwm_dev, 6*1000*1000, 0.5);

    return 0;
}

static uint8_t phx3d8014_get_device_id(void)
{
    uint8_t regVal;

    regVal = phx3d8014_readReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR14);

    // configASSERT(regVal == phx3d8014_DEVICE_ID);
    if (regVal != phx3d8014_DEVICE_ID) {
        LOGE(TAG, "i2c get device id error!");
    }

    return regVal;
}
/* =================================================================================== */
static uint8_t phx3d8014_get_detection(i2c_device_number_t dev)
{
    return phx3d8014_readReg(dev, phx3d8014_REG_ADDR11);
}
/* =================================================================================== */
int phx3d8014_init_test()
{
    uint8_t tblSize = phx3d8014_INIT_TBL_SIZE;
    int8_t ret;
    uint8_t i;
    uint8_t result;
    uint8_t pulse_in_ms = 2;
    uint16_t pulse_in_us = 2000;

    gpio_set_drive_mode(GPIO_PIN9, GPIO_DM_OUTPUT);//EN_BL TO HIGH
    gpio_set_pin(GPIO_PIN9, GPIO_PV_LOW);
    aiva_msleep(10);


    i2c_init(I2C_DEVICE_1, phx3d8014_SLAVE_ADDR, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);

    //寄存器上写入时许
 
    ret = phx3d8014_WriteReg(I2C_DEVICE_1, 0x2d, 0x01);
    if (ret < 0)
    {
        LOGI(TAG, "IIC fial");
    }
    else
    {
        LOGI(TAG, "IIC success new");
    }
    aiva_msleep(100);

    
    phx3d8014_WriteReg(I2C_DEVICE_1, 0x2d, 0x00);
    aiva_msleep(100);

    for (i = 0; i < tblSize; i++) {
        phx3d8014_WriteReg(I2C_DEVICE_1, 
                        gphx3d8014RegInitTbl[i].reg_addr, 
                        gphx3d8014RegInitTbl[i].data
                        );
    }

    // for (i = 0; i < tblSize; i++) {
    //     result = phx3d8014_readReg(I2C_DEVICE_1, 
    //                     gphx3d8014RegInitTbl[i].reg_addr
    //                     );
    //     LOGI(TAG, "i2c read reg 0x%02x", result);
    // }
    
    gpio_set_pin(GPIO_PIN9, GPIO_PV_HIGH);//EN_BL TO HIGH  of 8681
    gStrobeEnable = 1;

    gphx3d8014InitFlag = 1;
    LOGI(TAG, "phx3d8014 done");

    return 0;

}


int phx3d8014_init(led_dev_t *dev)
{
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    uint8_t pin = _dev->power_pin;
    int timeout_ms = dev->timeout_ms;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    uint8_t i;
    uint8_t tblSize = phx3d8014_INIT_TBL_SIZE;

    uint8_t ret;

    if(gphx3d8014InitFlag) {
        LOGW(TAG, "phx3d8014 has been inited test!");
        return 0;
    }

    gpio_set_drive_mode(GPIO_PIN9, GPIO_DM_OUTPUT);//EN_BL TO HIGH
    gpio_set_pin(GPIO_PIN9, GPIO_PV_LOW);
    aiva_msleep(10);

    /* 硬件上面有改动 */
    // gpio_set_drive_mode(pin, GPIO_DM_OUTPUT);
    // gpio_set_pin(pin, GPIO_PV_HIGH);//5V power on vcsel

    // // gpio_set_pin(pin, GPIO_PV_LOW);
    // // aiva_msleep(50);
    // aiva_msleep(10);
    LOGI(TAG, "phx3d8014 begin new");
   
    i2c_init(i2c_num, _dev->i2c_addr, 7, _dev->i2c_clk);

    //寄存器上写入时许
    ret = phx3d8014_WriteReg(i2c_num, 0x2d, 0x01);
    if (ret < 0) {
        LOGI(TAG,"IIC fial");
    }
    aiva_msleep(100);

    phx3d8014_WriteReg(i2c_num, 0x2d, 0x00);
    aiva_msleep(100);

    for (i = 0; i < tblSize; i++) {
        phx3d8014_WriteReg(i2c_num, 
                        gphx3d8014RegInitTbl[i].reg_addr, 
                        gphx3d8014RegInitTbl[i].data
                        );
    }

    gpio_set_pin(GPIO_PIN9, GPIO_PV_HIGH);//EN_BL TO HIGH  of 8681
    gStrobeEnable = 1;

    gphx3d8014InitFlag = 1;
    LOGI(TAG, "phx3d8014 done");

    return 0;
}
/* =================================================================================== */
/*
    Fixed bias current setting.
    current = （setVal + 1） * 4 (mA)    
*/
static int phx3d8014_set_IB_FIX(uint8_t setVal)
{
    UNION_REG(03) reg03;

    // i2c_init(phx3d8014_IIC_NUM, phx3d8014_SLAVE_ADDR, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);

    reg03.data.IB_FIX = setVal;

    return phx3d8014_WriteReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR00, reg03.bytes);
}
/* =================================================================================== */
/*
    Base current of output current setting.
    current = (setVal + 1) * 16 (mA)
*/
static int phx3d8014_set_ISW(uint8_t setVal)
{
    UNION_REG(02) reg02;

    // i2c_init(phx3d8014_IIC_NUM, phx3d8014_SLAVE_ADDR, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);

    reg02.data.ISW = setVal;

    return phx3d8014_WriteReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR02, setVal);
}
/* =================================================================================== */
/*
    Over current setting.
    setVal -->  000 : 4.5A
                001 : 4.0A
                010 : 3.5A
                011 : 3.0A
                100 : 2.5A
                101 : 2.0A
                110 : 1.5A
                111 : Funtion OFF
*/
static int phx3d8014_set_OC(uint8_t setVal)
{
    UNION_REG(0B) reg0B;

    i2c_init(phx3d8014_IIC_NUM, phx3d8014_SLAVE_ADDR, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);

    reg0B.bytes = phx3d8014_readReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR0B);
    reg0B.data.UV = setVal;

    return phx3d8014_WriteReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR0B, reg0B.bytes);
}
/* =================================================================================== */
/*
    Under voltage setting.
    setVal -->  00 : 2.5V
                01 : 2.6V
                10 : 2.7V
                11 : 2.8V
*/
static int phx3d8014_set_UV(uint8_t setVal)
{
    UNION_REG(0B) reg0B;

    i2c_init(phx3d8014_IIC_NUM, phx3d8014_SLAVE_ADDR, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);

    reg0B.bytes = phx3d8014_readReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR0B);
    reg0B.data.UV = setVal;

    return phx3d8014_WriteReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR0B, reg0B.bytes);
}
/* =================================================================================== */
void phx3d8014_power_on(void)
{
    if (!gphx3d8014InitFlag) {
        return;
    }

    gpio_set_pin(gphx3d8014PowerPin, GPIO_PV_HIGH);
}
/* =================================================================================== */
void phx3d8014_power_off(void)
{
    if (!gphx3d8014InitFlag) {
        return;
    }

    gpio_set_pin(gphx3d8014PowerPin, GPIO_PV_LOW);
}
/* =================================================================================== */
/*
    output current = ISW + IB_FIX
    IB_FIX default 212mA

    ISW = output current - IB_FIX
*/
int phx3d8014_set_current(led_num_t led, float current_ma)
{
    int rstIbFix    = -1;
    int rstIsw      = -1;

    if(CURRENT_2_IB_FIX(current_ma - 16) < IB_FIX_MAX)
    {
        rstIbFix = phx3d8014_set_IB_FIX(CURRENT_2_IB_FIX(current_ma - 16));
        rstIsw = phx3d8014_set_ISW(00);
    }
    else
    {
        rstIbFix = phx3d8014_set_IB_FIX(IB_FIX_MAX);
        // int max_fix_ma = IB_FIX_2_CURRENT(IB_FIX_MAX);
        rstIsw = phx3d8014_set_ISW(CURRENT_2_ISW(current_ma - IB_FIX_2_CURRENT(IB_FIX_MAX)));
    }

    return ((rstIbFix == 0 && rstIsw == 0)?0:-1);
}
/* =================================================================================== */
int phx3d8014_set_flash_timeout(int timeout_level)
{
    (void)timeout_level;
    return 0;
}

/* =================================================================================== */
int phx3d8014_set_flash_timeout_ms(led_dev_t *dev, int timeout_ms)
{
    // int timeout_level = 0;
    return phx3d8014_set_flash_timeout(timeout_ms);
}
/* =================================================================================== */
int phx3d8014_enable(led_dev_t *dev)
{
    pwm_set_enable(m_pwm_dev, 1);
    return 0;
}
/* =================================================================================== */
int phx3d8014_disable(led_dev_t *dev)
{
    pwm_set_enable(m_pwm_dev, 0);
    return 0;
}
/* =================================================================================== */
int phx3d8014_release(led_dev_t *dev)
{
    return 0;
}
/* =================================================================================== */
int phx3d8014_suspend(led_dev_t *dev)
{
    gSuspendFlag = 1;
    dev->is_suspend = 1;
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    //phx3d8014 only support LD0
    // if (gLedEnable[0])
    // {
    //     phx3d8014_disable(0);
    // }
    // LOGI(TAG, "phx3d8014_suspend\r\n");
    i2c_init(i2c_num, _dev->i2c_addr, 7, _dev->i2c_clk);
    if (gphx3d8014InitFlag) {
        uint8_t rst = phx3d8014_readReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR00);
        rst |= 0x01;
        phx3d8014_WriteReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR00, rst);
    }
    else {
        LOGE(__func__, "phx3d8014 has not been inited!");
    }
    return 0;
}

/* =================================================================================== */
int phx3d8014_resume(led_dev_t *dev)
{
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;

    gSuspendFlag = 0;
    dev->is_suspend = 0;
    i2c_init(i2c_num, _dev->i2c_addr, 7, _dev->i2c_clk);
    phx3d8014_set_current(LED_0, phx3d8014_CURRENT_CFG);
    // if (gLedEnable[0])
    // {
    //     phx3d8014_enable(0);
    // }
    // LOGI(TAG, "phx3d8014_resume\r\n");
    if (gphx3d8014InitFlag) {
        uint8_t rst = phx3d8014_readReg(i2c_num, phx3d8014_REG_ADDR00);
        rst = rst & 0xfe;
        phx3d8014_WriteReg(i2c_num, phx3d8014_REG_ADDR00, rst);
    }
    else {
        LOGE(__func__, "phx3d8014 has not been inited!");
    }
    return 0;
}

/* =================================================================================== */
int phx3d8014_get_current_range_wrapper(led_dev_t *dev, float *min, float *max, float *step)
{
    uint8_t rst;
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;

    if(gphx3d8014InitFlag) {
        rst = phx3d8014_readReg(i2c_num, phx3d8014_CH1_CURRENT_REG);
        LOGI(TAG, "register0B: %d", rst);
    }

    // if(chn == CHANNEL_1 && gphx3d8014InitFlag) {
    //     rst = phx3d8014_readReg(i2c_num, phx3d8014_CH2_CURRENT_REG);
    //     LOGI(TAG, "register0B: %d", rst);
    
    // }
    
    *step = rst*STEP_CURRENT; 

    return 0;
}
/* =================================================================================== */
int phx3d8014_set_current_wrapper(led_dev_t *dev, float current_ma)
{
    phx3d8014_set_current(LED_0, current_ma);
    // uint8_t rst = phx3d8014_readReg(phx3d8014_IIC_NUM, phx3d8014_REG_ADDR02);
    // LOGE(TAG, "register02: %d\n", rst);
    return 0;
}
/* =================================================================================== */
int phx3d8014_flash_bright(led_dev_t *dev)                                /** trigger led to make it work again, device must have this api */
{
    configASSERT(dev->priv != NULL);
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    uint8_t pin = _dev->power_pin;
    const led_trig_param_t *trig = dev->trig_param;

    if (_dev->use_strobe)
    {
        return 0;
    }

    // led_channel_t chn_idx = CHANNEL_0;
    // if (chn == CHANNEL_ALL) {
    //     chn_idx = CHANNEL_0;
    // } else {
    //     chn_idx = chn;
    // }

    //TODO: use per channel!!!
    if (!dev->is_suspend)
    {
        if (dev->is_enable)
        {
            gpio_set_pin(pin, GPIO_PV_LOW);
            //
            int pulse_in_ms = 2;
            if (trig != NULL && trig->pulse_in_us >= 2000 && trig->pulse_in_us <= 10000)
            {
                pulse_in_ms = (trig->pulse_in_us / 1000);
            }
#if USE_RTOS
            TimerHandle_t timer = _dev->timer;
            TickType_t period = xTimerGetPeriod(timer);
            TickType_t expect_period = pulse_in_ms / portTICK_PERIOD_MS;
            if (xPortIsInISR())
            {
                if (period != expect_period)
                {
                    xTimerChangePeriodFromISR(timer, expect_period, NULL);
                }
                xTimerStartFromISR(timer, NULL);
            }
            else
            {
                if (period != expect_period)
                {
                    xTimerChangePeriod(timer, expect_period, 0);
                }
                xTimerStart(timer, 0);
            }
#else
            aiva_busy_delay_ms(pulse_in_ms);
            gpio_set_pin(pin, GPIO_PV_HIGH);
#endif
        }
    }

    return 0;
}

int phx3d8014_test(led_dev_t *dev)//(i2c_device_number_t i2c_num, uint8_t pin, bool gpio_control)
{    
    uint8_t deviceId = 0;
    uint8_t rst = 0;
    phx3d8014_led_t* _dev = (phx3d8014_led_t*)dev->priv;
    uint8_t pin = _dev->power_pin;
    int timeout_ms = dev->timeout_ms;
 
    i2c_device_number_t i2c_num = _dev->i2c_num;
    // int ret = i2c_scan_dev_addr(phx3d8014_IIC_NUM, phx3d8014_IIC_ADDR_WIDTH, phx3d8014_SCL);
    // LOGI(TAG, "i2c_scan_dev_addr ret : %d\n", ret);
    // LOGI(TAG, "slave id is : %d\n", phx3d8014_SLAVE_ADDR);
    // LOGI(TAG, "phx3d8014_test");
    
    //phx3d8014_pwm_init();

    phx3d8014_init(dev);

    if(dev->is_initialized == 1) {
        return 0;
    }
    phx3d8014_pwm_init();
    rst = phx3d8014_get_detection(i2c_num);
    LOGI(TAG, "register11-: %d", rst);

    // deviceId = phx3d8014_get_device_id();
    // LOGI(TAG, "device id is : %d", deviceId);

    // phx3d8014_WriteReg(i2c_num, phx3d8014_REG_ADDR00, 0x44);
    // phx3d8014_WriteReg(i2c_num, phx3d8014_REG_ADDR0B, 0x05);
    // phx3d8014_WriteReg(i2c_num, phx3d8014_REG_ADDR02, 0x00);
    // phx3d8014_WriteReg(i2c_num, phx3d8014_REG_ADDR12, 0x00);

    rst = phx3d8014_readReg(i2c_num, 0x08);
    LOGI(TAG, "register0x08: %d", rst);

    // rst = phx3d8014_readReg(i2c_num, phx3d8014_REG_ADDR02);
    // LOGI(TAG, "register02: %d", rst);

    // rst = phx3d8014_readReg(i2c_num, phx3d8014_REG_ADDR0B);
    // LOGI(TAG, "register0B: %d", rst);

    // phx3d8014_set_current(LED_0, phx3d8014_CURRENT_CFG);

    // rst = phx3d8014_readReg(i2c_num, phx3d8014_REG_ADDR02);
    // LOGI(TAG, "register02: %d", rst);
    
    dev->is_initialized = 1;

    return 0;
}


/****************************************超时函数**********************************************/

static enum phx3d8014_time_out_val timeout_mslevel(int timeout_ms)
{
    enum phx3d8014_time_out_val timeout_level = PHX3D8014_TIMEOUT_10MS;

    if (timeout_ms > 0 && timeout_ms < 5) {
        timeout_level = PHX3D8014_TIMEOUT_10MS;
    } 
    else if (timeout_ms >= 5 && timeout_ms <= 100)
    {
        timeout_level = (enum phx3d8014_time_out_val)((timeout_ms + 5) / 10 - 1);
    }
    else if (timeout_ms <= 400)
    {
        timeout_level = (enum phx3d8014_time_out_val)((timeout_ms + 25) / 50 - 2 + PHX3D8014_TIMEOUT_100MS);
    }
    else
    {
        timeout_level = PHX3D8014_TIMEOUT_400MS;
    }

    return timeout_level;
}

static int PHX3D8014_set_flash_timeout(led_dev_t *dev, int timeout_level)
{
    phx3d8014_get_lock(dev);

    phx3d8014_led_t *_dev = (phx3d8014_led_t *)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    i2c_init(i2c_num, _dev->i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    // phx3d8014_write_reg(i2c_num,
    //         _dev->i2c_addr,
    //         TIMING_REG, 
    //         timeout_level);

    phx3d8014_release_lock(dev);

    return 0;
}


int phx3d8014_set_timeout_ms(led_dev_t *dev,  int timeout_ms)
{
    dev->timeout_ms = timeout_ms;
    enum phx3d8014_time_out_val timeout_level = timeout_mslevel(timeout_ms);

    return PHX3D8014_set_flash_timeout(dev, timeout_level);
}


/********************************************************************************************/
static led_ops_t phx3d8014_led_ops = {
    .init = phx3d8014_test,
    .release = phx3d8014_release,
    .enable = phx3d8014_enable,
    .disable = phx3d8014_disable,
    .suspend = phx3d8014_disable,
    .resume = phx3d8014_enable,
    .get_current_range = phx3d8014_get_current_range_wrapper,
    .set_current = phx3d8014_set_current_wrapper,
    .set_timeout = phx3d8014_set_timeout_ms,
    .trigger = phx3d8014_flash_bright,
};
static phx3d8014_led_t led0 = {
    .i2c_num = I2C_DEVICE_1,
    .i2c_addr = phx3d8014_SLAVE_ADDR,
    .i2c_addr_width = phx3d8014_IIC_ADDR_WIDTH,
    .i2c_clk = phx3d8014_SCL,
    .power_pin = GPIO_PIN8,
    .led_index = 0,
// TODO: decoupling from sensor???
    .use_strobe = 1, // use strobe or gpio
#if USE_RTOS
    .timer = NULL,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
#endif
};
static led_trig_param_t trig_param = {
    .pulse_in_us = 2000,
};

static led_dev_t phx3d8014 = {
    .name = "phx3d8014",
    .ops = &phx3d8014_led_ops,
    .trig_param = &trig_param,      //
    .is_initialized = 0,
    .is_enable = 0,
    .is_suspend = 0,
    // .timeout_ms = 30,
    .timeout_ms = 40, // this vlaue should be the same with {TIMING_REG, 0x13}
    .priv = &led0,
};

UDEVICE_EXPORT(UCLASS_LED,  phx3d8014,  0,  &phx3d8014,  NULL);
