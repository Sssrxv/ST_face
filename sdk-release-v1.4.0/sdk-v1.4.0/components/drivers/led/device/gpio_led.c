#include "gpio_led.h"
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


#ifdef __cplusplus
extern "C" {
#endif

#if USE_RTOS
static void timeout_cb(TimerHandle_t timer)
{
    gpio_led_t* _dev = (gpio_led_t*)pvTimerGetTimerID(timer);
    gpio_set_pin(_dev->pin, GPIO_PV_LOW);
}
#endif

int gpio_led_init(led_dev_t *dev)
{
    gpio_led_t* _dev = (gpio_led_t*)dev->priv;
    uint8_t pin = _dev->pin;
    int timeout_ms = dev->timeout_ms;
    //
    sysctl_io_switch_t io_switch = IO_SWITCH_GPIO0 + pin;
    sysctl_set_io_switch(io_switch, 1);
    gpio_set_drive_mode(pin, GPIO_DM_OUTPUT);

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

    return 0;
}

int gpio_led_release(led_dev_t *dev)
{
#if USE_RTOS
    gpio_led_t* _dev = (gpio_led_t*)dev->priv;
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

int gpio_led_enable(led_dev_t *dev)
{
    dev->is_enable = 1;

    return 0;
}

int gpio_led_disable(led_dev_t *dev)
{
    dev->is_enable = 0;

    return 0;
}

int gpio_led_suspend(led_dev_t *dev)
{
    dev->is_suspend = 1;

    return 0;
}

int gpio_led_resume(led_dev_t *dev)
{
    dev->is_suspend = 0;

    return 0;
}

int gpio_led_set_timeout(led_dev_t *dev, int timeout_ms)
{
    dev->timeout_ms = timeout_ms;

    return 0;
}

int gpio_led_trigger(led_dev_t *dev)
{
    configASSERT(dev->priv != NULL);
    gpio_led_t* _dev = (gpio_led_t*)dev->priv;
    uint8_t pin = _dev->pin;
    int timeout_ms = dev->timeout_ms;
    if (!dev->is_suspend)
    {
        if (dev->is_enable)
        {
            gpio_set_pin(pin, GPIO_PV_HIGH);
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
        gpio_set_pin(pin, GPIO_PV_LOW);
#endif
    }

    return 0;
}


#ifdef __cplusplus
}
#endif

static led_ops_t gpio_led_ops = {
    .init = gpio_led_init,
    .release = gpio_led_release,
    .enable = gpio_led_enable,
    .disable = gpio_led_disable,
    .suspend = gpio_led_suspend,
    .resume = gpio_led_resume,
    .get_current_range = NULL,
    .set_current = NULL,
    .set_timeout = gpio_led_set_timeout,
    .trigger = gpio_led_trigger,
};

static gpio_led_t gpio3 = {
    .pin = GPIO_PIN3,
#if USE_RTOS
    .timer = NULL,
#endif
};

static led_dev_t gpio_led = {
    .name = "gpio_led",
    .ops = &gpio_led_ops,
    .trig_param = NULL,
    .is_initialized = 0,
    .is_enable = 0,
    .is_suspend = 0,
    .timeout_ms = 30,
    .priv = &gpio3,
};


//NOTE: use apis in led_driver.h
/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_LED,     gpio_led,    0,      &gpio_led,    NULL);
