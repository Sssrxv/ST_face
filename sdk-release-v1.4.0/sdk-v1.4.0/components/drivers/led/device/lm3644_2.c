#include "lm3644_2.h"
#include "led_driver.h"
#include "gpio.h"
#include "sysctl.h"
#include "syslog.h"
#include "udevice.h"
#if USE_RTOS
#include <stdio.h>
#else
#include "aiva_sleep.h"
#endif
#include <math.h>

#define TAG                             "LM3644"
#define LM3644_SLAVE_ADDR               (0x63 << 0)

#define ENABLE_REG                      (0X01)
#define IVFM_REG                        (0x02)
#define LED1_FLASH_REG                  (0x03)
#define LED2_FLASH_REG                  (0x04)
#define LED1_TORCH_REG                  (0x05)
#define LED2_TORCH_REG                  (0x06)
#define BOOST_REG                       (0x07)
#define TIMING_REG                      (0x08)
#define TEMP_REG                        (0x09)
#define FLAGS1_REG                      (0x0A)
#define FLAGS2_REG                      (0x0B)
#define DEVICE_ID_REG                   (0x0C)
#define LAST_FLASH_REG                  (0x0D)

enum lm3644_time_out_val {
    LM3644_TIMEOUT_10MS   = 0,
    LM3644_TIMEOUT_20MS   = 1,
    LM3644_TIMEOUT_30MS   = 2,
    LM3644_TIMEOUT_40MS   = 3,
    LM3644_TIMEOUT_50MS   = 4,
    LM3644_TIMEOUT_60MS   = 5,
    LM3644_TIMEOUT_70MS   = 6,
    LM3644_TIMEOUT_80MS   = 7,
    LM3644_TIMEOUT_90MS   = 8,
    LM3644_TIMEOUT_100MS  = 9,
    LM3644_TIMEOUT_150MS  = 10,
    LM3644_TIMEOUT_200MS  = 11,
    LM3644_TIMEOUT_250MS  = 12,
    LM3644_TIMEOUT_300MS  = 13,
    LM3644_TIMEOUT_350MS  = 14,
    LM3644_TIMEOUT_400MS  = 15,
};

typedef struct _LM3644_CONFIG_T {
    uint8_t     reg_addr;                             // Register address
    uint8_t     data;                                 // Data
}LM3644_CONFIG_T;

#define LED_BRIGHTNESS_INIT             (0x20)
#define MAX_BRIGHTNESS                  (0x7f)

#define CURRENT_MIN_MA                  (10.9f)
#define CURRENT_STEP_MA                 (11.725f)
#define CURRENT_MAX_MA                  (MAX_BRIGHTNESS * CURRENT_STEP_MA + CURRENT_MIN_MA)

/* 
   if only LED_2 is connected to lm3644, setting reg 0x1 to 0x27, LED_2 won't work;
   if LED_1 and LED_2 is connected to lm3644, setting reg 0x1 to 0x27, LED_1 and LED_2 works.
   */   
static LM3644_CONFIG_T lm3644_init_settings[] = {
/*#ifdef USE_SL18*/
    /*// IR mode, 0x26, enable LED2,disable LED1.*/
    /*{0x1, 0x26},       // Reg 1  ()    */
/*#else*/
    /*// IR mode, 0x25, enable LED1,disable LED2.*/
    /*{0x1, 0x25},       // Reg 1  ()    */
/*#endif*/
    // IR mode, 0x27, disable LED2, LED1.
    {ENABLE_REG, 0x24},       // Reg 1  ()

    {IVFM_REG, 0x01},       // Reg 2  ()
    {LED1_FLASH_REG, LED_BRIGHTNESS_INIT},       // Reg 3  ()
    {LED2_FLASH_REG, LED_BRIGHTNESS_INIT},       // Reg 4  ()
    {LED1_TORCH_REG, 0x8F},       // Reg 5  ()
    {LED2_TORCH_REG, 0x0F},       // Reg 6  ()
    {BOOST_REG, 0x09},       // Reg 7  ()
    /*{TIMING_REG, 0x1F},       // Reg 8  ()*/
    {TIMING_REG, 0x13},       // Reg 8  ()  40ms
    /*{TIMING_REG, 0x14},       // Reg 8  ()  50ms*/
    /*{TIMING_REG, 0x11},       // Reg 8  ()  20ms*/
    {TEMP_REG, 0x08},       // Reg 9  ()
};

typedef struct _LED_STATUS {
    uint8_t     brightness_reg_addr;
    uint8_t     brightness;                             
} LED_STATUS;

static LED_STATUS led_status[] = {
    {LED1_FLASH_REG, LED_BRIGHTNESS_INIT},
    {LED2_FLASH_REG, LED_BRIGHTNESS_INIT}
};

static int lm3644_write_reg(i2c_device_number_t dev, uint8_t i2c_addr, uint8_t reg, uint8_t data)
{
    int ret;
    uint8_t data_buf[2];

    data_buf[0] = reg;
    data_buf[1] = data;
    ret = i2c_send_data(dev, i2c_addr, data_buf, 2);
    CHECK_RET(ret);

    return ret;
}

static uint8_t lm3644_read_reg(i2c_device_number_t dev, uint8_t i2c_addr, uint8_t reg_addr)
{
    int ret;
    uint8_t reg_val;

    ret = i2c_send_data(dev, i2c_addr, &reg_addr, 1);
    CHECK_RET(ret);

    ret = i2c_recv_data(dev, i2c_addr, 0, 0, &reg_val, 1);
    CHECK_RET(ret);

    return reg_val;
}

static int lm3644_get_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_lock(&(((lm3644_led_t*)dev->priv)->mutex));
    }
#endif
    return ret;
}

static int lm3644_release_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_unlock(&(((lm3644_led_t*)dev->priv)->mutex));
    }
#endif
    return ret;
}

static enum lm3644_time_out_val timeout_ms2level(int timeout_ms)
{
    enum lm3644_time_out_val timeout_level = LM3644_TIMEOUT_10MS;

    if (timeout_ms > 0 && timeout_ms < 5) {
        timeout_level = LM3644_TIMEOUT_10MS;
    } 
    else if (timeout_ms >= 5 && timeout_ms <= 100)
    {
        timeout_level = (enum lm3644_time_out_val)((timeout_ms + 5) / 10 - 1);
    }
    else if (timeout_ms <= 400)
    {
        timeout_level = (enum lm3644_time_out_val)((timeout_ms + 25) / 50 - 2 + LM3644_TIMEOUT_100MS);
    }
    else
    {
        timeout_level = LM3644_TIMEOUT_400MS;
    }

    return timeout_level;
}

static int lm3644_set_flash_timeout(led_dev_t *dev, int timeout_level)
{
    lm3644_get_lock(dev);

    lm3644_led_t *_dev = (lm3644_led_t *)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    i2c_init(i2c_num, _dev->i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    lm3644_write_reg(i2c_num,
            _dev->i2c_addr,
            TIMING_REG, 
            timeout_level);

    lm3644_release_lock(dev);

    return 0;
}

// void lm3644_test(void)
// {    
//     lm3644_init(I2C_DEVICE_2, GPIO_PIN3, true);
//     lm3644_set_flash_timeout(LM3644_TIMEOUT_10MS);

//     while (1)
//     {
//         lm3644_power_off();        
//         lm3644_power_on();
//         lm3644_busy_delay(3000);
//     }
// }


#ifdef __cplusplus
extern "C" {
#endif

#if USE_RTOS
static void timeout_cb(TimerHandle_t timer)
{
    lm3644_led_t* _dev = (lm3644_led_t*)pvTimerGetTimerID(timer);
    gpio_set_pin(_dev->power_pin, GPIO_PV_HIGH);
}
#endif

int lm3644_init2(led_dev_t *dev)
{
    lm3644_led_t* _dev = (lm3644_led_t*)dev->priv;
    uint8_t pin = _dev->power_pin;
    int timeout_ms = dev->timeout_ms;
 
    i2c_device_number_t i2c_num = _dev->i2c_num;

    lm3644_get_lock(dev);

    i2c_init(i2c_num, _dev->i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    for (int i = 0; i < (int)AIVA_ARRAY_LEN(lm3644_init_settings); i++)
    {
        lm3644_write_reg(i2c_num,
                _dev->i2c_addr,
                lm3644_init_settings[i].reg_addr, 
                lm3644_init_settings[i].data);
    }

    // enum lm3644_time_out_val timeout_level = timeout_ms2level(timeout_ms);
    // lm3644_write_reg(i2c_num,
    //     _dev->i2c_addr, 
    //     TIMING_REG, 
    //     timeout_level);

    lm3644_release_lock(dev);

    sysctl_io_switch_t io_switch = IO_SWITCH_GPIO0 + pin;
    sysctl_set_io_switch(io_switch, 1);

    if (_dev->use_strobe)
    {
        gpio_set_drive_mode(pin, GPIO_DM_INPUT);
    }
    else
    {
        gpio_set_drive_mode(pin, GPIO_DM_OUTPUT);
#if USE_RTOS
        int pulse_in_ms = 2;
        const led_trig_param_t *trig = dev->trig_param;
        if (trig != NULL && trig->pulse_in_us >= 2000 && trig->pulse_in_us <= 10000)
        {
            pulse_in_ms = (trig->pulse_in_us / 1000);
        }
        char timer_name[20] = {0};
        snprintf(timer_name, sizeof(timer_name) / sizeof(timer_name[0]), "LM3644%d_LED_TIMER", (int)pin);
        // create software timer
        _dev->timer = xTimerCreate
                    (timer_name,
                    pdMS_TO_TICKS(pulse_in_ms),
                    0,
                    (void *)_dev,
                    timeout_cb);
#endif
    }

    LOGI(TAG, "lm3644 done\n");

    return 0;
 }

int lm3644_release2(led_dev_t *dev)
{
#if USE_RTOS
    lm3644_led_t* _dev = (lm3644_led_t*)dev->priv;
    if (_dev->timer)
    {
        // if (xPortIsInISR())
        // {
        //     xTimerStopFromISR(_dev->timer, NULL);
        // }
        // else
        {
            xTimerStop(_dev->timer, pdMS_TO_TICKS(10));
        }
        xTimerDelete(_dev->timer, pdMS_TO_TICKS(10));
        _dev->timer = NULL;
    }
#endif

    return 0;
}

int lm3644_enable2(led_dev_t *dev)
{
    lm3644_led_t* _dev = (lm3644_led_t*)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    uint8_t i2c_addr = _dev->i2c_addr;
    uint8_t chn = _dev->led_index;
    uint8_t data; 

    lm3644_get_lock(dev);

    i2c_init(i2c_num, i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    data = lm3644_read_reg(i2c_num, i2c_addr, ENABLE_REG);

    if (chn == CHANNEL_0) {
        data |= 0x1;
    } else if (chn == CHANNEL_1) {
        data |= 0x2;
    } else if (chn == CHANNEL_ALL) {
        data |= 0x11;
    } else {
        return -1;
    }

    lm3644_write_reg(i2c_num, i2c_addr, ENABLE_REG, data);

    lm3644_release_lock(dev);

    dev->is_enable = 1;

    return 0;
}

int lm3644_disable2(led_dev_t *dev)
{
    lm3644_led_t* _dev = (lm3644_led_t*)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    uint8_t i2c_addr = _dev->i2c_addr;
    uint8_t chn = _dev->led_index;
    uint8_t data; 

    lm3644_get_lock(dev);

    i2c_init(i2c_num, i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    data = lm3644_read_reg(i2c_num, i2c_addr, ENABLE_REG);

    if (chn == CHANNEL_0) {
        data &= ~0x1;
    } else if (chn == CHANNEL_1) {
        data &= ~0x2;
    } else {
        return -1;
    }

    lm3644_write_reg(i2c_num, i2c_addr, ENABLE_REG, data);

    lm3644_release_lock(dev);

    dev->is_enable = 0;

    return 0;
}

int lm3644_suspend2(led_dev_t *dev)
{
#if (defined USE_SENSOR_OV02B_STROBE)
    int ret = lm3644_disable2(dev);
    if (ret == 0) {
        dev->is_suspend = 1;
    }
#else
    dev->is_suspend = 1;
#endif

    return 0;
}

int lm3644_resume2(led_dev_t *dev)
{
#if (defined USE_SENSOR_OV02B_STROBE)
    int ret = lm3644_enable2(dev);
    if (ret == 0) {
        dev->is_suspend = 0;
    }
#else
    dev->is_suspend = 0;
#endif

    return 0;
}

int lm3644_get_current_range2(led_dev_t *dev, float *min, float *max, float *step)
{
    lm3644_get_lock(dev);
    *min = CURRENT_MIN_MA;
    *max = CURRENT_MAX_MA;
    *step = CURRENT_STEP_MA;
    lm3644_release_lock(dev);

    return 0;
}

int lm3644_set_current_level2(led_dev_t *dev, led_current_level_t level)
{
    lm3644_get_lock(dev);

    lm3644_led_t *_dev = (lm3644_led_t *)dev->priv;
    i2c_device_number_t i2c_num = _dev->i2c_num;
    uint8_t led_index = _dev->led_index;
    i2c_init(i2c_num, _dev->i2c_addr, _dev->i2c_addr_width, _dev->i2c_clk);

    led_status[led_index].brightness = ((uint8_t)level) & MAX_BRIGHTNESS;

    uint8_t flag1, flag2;

    flag1 = lm3644_read_reg(i2c_num, _dev->i2c_addr, FLAGS1_REG);
    flag2 = lm3644_read_reg(i2c_num, _dev->i2c_addr, FLAGS2_REG);

    if (flag1 || flag2)
    {
        LOGW(__func__, "Fault detected: flag1 [0x%x], flag2 [0x%x]", flag1, flag2);
    }
    uint8_t data = lm3644_read_reg(i2c_num, _dev->i2c_addr, ENABLE_REG);
    data |= 0x24;
    lm3644_write_reg(i2c_num, _dev->i2c_addr, ENABLE_REG, data);

    lm3644_write_reg(i2c_num,
            _dev->i2c_addr,
            led_status[led_index].brightness_reg_addr, 
            led_status[led_index].brightness);

    lm3644_release_lock(dev);

    return 0;
}

int lm3644_set_current2(led_dev_t *dev, float current_ma)
{
    int ret = -1;

    //
    int level = (int)(roundf((current_ma - CURRENT_MIN_MA) / CURRENT_STEP_MA));

    if (level < 0 || level > MAX_BRIGHTNESS)
    {
        LOGE(TAG, "invalid current:%f\n", current_ma);
        return ret;
    }

    /*LOGD(TAG, "current %f mA, level val:%d\n", current_ma, level);*/

    ret = lm3644_set_current_level2(dev, level);

    return ret;
}

int lm3644_set_timeout_ms2(led_dev_t *dev, int timeout_ms)
{
    dev->timeout_ms = timeout_ms;
    enum lm3644_time_out_val timeout_level = timeout_ms2level(timeout_ms);

    return lm3644_set_flash_timeout(dev, timeout_level);
}

int lm3644_trigger2(led_dev_t *dev)
{
    configASSERT(dev->priv != NULL);
    lm3644_led_t* _dev = (lm3644_led_t*)dev->priv;
    uint8_t pin = _dev->power_pin;
    uint8_t chn = _dev->led_index;
    const led_trig_param_t *trig = dev->trig_param;

    if (_dev->use_strobe)
    {
        return 0;
    }

    lm3644_channel_t chn_idx = CHANNEL_0;
    if (chn == CHANNEL_ALL) {
        chn_idx = CHANNEL_0;
    } else {
        chn_idx = chn;
    }

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


#ifdef __cplusplus
}
#endif

static led_ops_t lm3644_led_ops = {
    .init = lm3644_init2,
    .release = lm3644_release2,
    .enable = lm3644_enable2,
    .disable = lm3644_disable2,
    .suspend = lm3644_suspend2,
    .resume = lm3644_resume2,
    .get_current_range = lm3644_get_current_range2,
    .set_current = lm3644_set_current2,
    .set_timeout = lm3644_set_timeout_ms2,
    .trigger = lm3644_trigger2,
};

static lm3644_led_t led0 = {
    .i2c_num = I2C_DEVICE_2,
    .i2c_addr = LM3644_SLAVE_ADDR,
    .i2c_addr_width = 7,
    .i2c_clk = 50 * 1000,
    .power_pin = GPIO_PIN3,
    .led_index = 0,
// TODO: decoupling from sensor???
#if (defined USE_SENSOR_OV02B_STROBE)
    .use_strobe = 1, // use strobe or gpio
#else
    .use_strobe = 0,
#endif

#if USE_RTOS
    .timer = NULL,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
#endif
};

static led_trig_param_t trig_param = {
    .pulse_in_us = 2000,
};

static led_dev_t lm3644 = {
    .name = "lm3644",
    .ops = &lm3644_led_ops,
    .trig_param = &trig_param,
    .is_initialized = 0,
    .is_enable = 0,
    .is_suspend = 0,
    // .timeout_ms = 30,
    .timeout_ms = 40, // this vlaue should be the same with {TIMING_REG, 0x13}
    .priv = &led0,
};

//NOTE: use apis in led_driver.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_LED,  lm3644,  0,  &lm3644,  NULL);
