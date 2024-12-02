#include "phx3d8014.h"
#include "led_driver.h"
#include "gpio.h"
#include "sysctl.h"
#include "syslog.h"
#include "udevice.h"
#include "pwm.h"

#if USE_RTOS
#include <stdio.h>
#else
#include "aiva_sleep.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

static const char* TAG = "mpq7235.c";

static pwm_device_number_t m_pwm_dev = PWM_DEVICE_0;
static int mpq7235_pwm_init(void);
static void mpq7235_power_off(void);
static void mpq7235_power_on(void);

#if USE_RTOS
static void timeout_cb(TimerHandle_t timer)
{
    mpq7235_cfg_t* _dev = (mpq7235_cfg_t*)pvTimerGetTimerID(timer);
    // gpio_set_pin(23, GPIO_PV_LOW);
}
#endif

static void mpq7235_power_on(void)
{
    gpio_set_pin(23, GPIO_PV_HIGH);
}

static void mpq7235_power_off(void)
{
    gpio_set_pin(23, GPIO_PV_LOW);
}


int mpq7235_led_init(led_dev_t *dev)
{
    uint8_t ret;
    mpq7235_cfg_t* _dev = (mpq7235_cfg_t*)dev->priv;
    uint8_t pin = _dev->pin;
    int timeout_ms = dev->timeout_ms;
    // ret = gpio_init();
    // if(ret != 0) {
    //     LOGE(TAG, "gpio init fail\n");
    //     return -1;
    // }
    
    /* Set GPIO1 as an input */
    gpio_set_drive_mode(pin, GPIO_DM_INPUT);

    /* Set GPIO23 as a ouput power pin */
    gpio_set_drive_mode(23, GPIO_DM_OUTPUT);

    /* Set GPIO0 out pwm */
    mpq7235_pwm_init();

#if USE_RTOS
    char timer_name[20] = {0};
    snprintf(timer_name, sizeof(timer_name) / sizeof(timer_name[0]), "GPIO%d_LED_TIMER", (int)pin);
    // create software timer
    // NOTE: we not release timer
    _dev->timer = xTimerCreate
                 (timer_name,
                   pdMS_TO_TICKS(timeout_ms),
                   0,
                   (void *)_dev,
                   timeout_cb);
#endif
    LOGE(TAG, "gpio init SUCCESS\n");
    return 0;
}

int mpq7235_led_release(led_dev_t *dev)
{
#if USE_RTOS
    mpq7235_cfg_t* _dev = (mpq7235_cfg_t*)dev->priv;
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

int mpq7235_led_enable(led_dev_t *dev)
{
    LOGE(__func__,"mpq7235 enable");
    dev->is_enable = 1;
    mpq7235_power_on();
    return 0;
}

int mpq7235_led_disable(led_dev_t *dev)
{
    LOGE(__func__,"mpq7235 disable");
    dev->is_enable = 0;
    mpq7235_power_off();
    return 0;
}

int mpq7235_led_suspend(led_dev_t *dev)
{
    LOGE(__func__,"mpq7235 suspend");
    dev->is_suspend = 1;
    mpq7235_power_off();
    pwm_set_enable(m_pwm_dev, 0);
    return 0;
}

int mpq7235_led_resume(led_dev_t *dev)
{
    LOGE(__func__,"mpq7235 resume");
    dev->is_suspend = 0;
    mpq7235_power_on();
    return 0;
}

int mpq7235_led_set_timeout(led_dev_t *dev, int timeout_ms)
{
    dev->timeout_ms = timeout_ms;

    return 0;
}

void gpio_led_set_brightness(const double duty)
{ 
    double duty_X1 = duty;
    pwm_set_frequency(m_pwm_dev, 1, duty_X1);
}


int gpio_led_get_nfault(void)
{
    gpio_pin_value_t pin_value;

    /* If pin_value is 0, it indicates that the LED has nfault */
    pin_value = gpio_get_pin(1);

    return pin_value;
}

int mpq7235_cfg_trigger(led_dev_t *dev)
{
    configASSERT(dev->priv != NULL);
    LOGE(__func__,"mpq7235 doing");
    mpq7235_cfg_t* _dev = (mpq7235_cfg_t*)dev->priv;
    uint8_t pin = _dev->pin;
    int timeout_ms = dev->timeout_ms;
    if (!dev->is_suspend)
    {
        if (dev->is_enable)
        {
            pwm_set_enable(m_pwm_dev, 1);
        }

#if USE_RTOS
        TimerHandle_t timer = _dev->timer;
        TickType_t period = xTimerGetPeriod(timer);
        TickType_t expect_period = timeout_ms / portTICK_PERIOD_MS;
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
        aiva_busy_delay_ms(timeout_ms);
        pwm_set_enable(m_pwm_dev, 0);
#endif
    }

    return 0;
}

static int mpq7235_pwm_init(void)
{
    pwm_init(m_pwm_dev);
    pwm_set_enable(m_pwm_dev, 0);
    /*  The init value is 1K, and the duty cycle is 50% */
    pwm_set_frequency(m_pwm_dev, 1*1000, 0.5);

    return 0;
}

#ifdef __cplusplus
}
#endif

static led_ops_t gpio_led_ops = {
    .init = mpq7235_led_init,
    .release = mpq7235_led_release,
    .enable = mpq7235_led_enable,
    .disable = mpq7235_led_disable,
    .suspend = mpq7235_led_suspend,
    .resume = mpq7235_led_resume,
    .get_current_range = NULL,
    .set_current = NULL,
    .set_timeout = mpq7235_led_set_timeout,
    .trigger = mpq7235_cfg_trigger,
};

/* nfault pin */
static mpq7235_cfg_t gpio1 = {
    .pin = GPIO_PIN1,
#if USE_RTOS
    .timer = NULL,
#endif
};

static led_dev_t phx3d8014 = {
    .name = "phx3d8014",
    .ops = &gpio_led_ops,
    .trig_param = NULL,
    .is_initialized = 0,
    .is_enable = 0,
    .is_suspend = 0,
    .timeout_ms = 30,
    .priv = &gpio1,
};


//NOTE: use apis in led_driver.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_LED,     phx3d8014,    0,      &phx3d8014,    NULL);



