#include "led_driver.h"
#include "syslog.h"
#include "udevice.h"

static int led_get_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_lock(&dev->mutex);
    }
#endif
    return ret;
}

static int led_release_lock(led_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_unlock(&dev->mutex);
    }
#endif
    return ret;
}

int led_init(led_dev_t *dev)
{
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->init != NULL);
    int ret = 0;

    if (!dev->is_initialized)
    {
        ret = dev->ops->init(dev);
        dev->is_initialized = ret == 0 ? 1 : 0;
        if (ret == 0)
        {
#ifdef USE_RTOS
            pthread_mutexattr_t attr;
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&dev->mutex, &attr);
#endif
        }
    }

    return ret;
}

int led_release(led_dev_t *dev)
{
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->release != NULL);

    int ret = 0;
    if (dev->is_initialized)
    {
        led_get_lock(dev);
        ret = dev->ops->release(dev);
        dev->is_initialized = ret == 0 ? 0 : 1;
        led_release_lock(dev);
        if (ret == 0)
        {
#ifdef USE_RTOS
            pthread_mutex_destroy(&dev->mutex);
#endif
        }
    }


    return ret;
}

int led_enable(led_dev_t *dev)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->enable != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    led_get_lock(dev);
    ret = dev->ops->enable(dev);
    led_release_lock(dev);
out:
    return ret;
}

int led_disable(led_dev_t *dev)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->disable != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    led_get_lock(dev);
    ret = dev->ops->disable(dev);
    led_release_lock(dev);

out:
    return ret;
}

int led_get_current_range(led_dev_t *dev, float *current_min, float *current_max, float *current_step)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    if (dev->ops->get_current_range != NULL)
    {
        led_get_lock(dev);
        ret = dev->ops->get_current_range(dev, current_min, current_max, current_step);
        led_release_lock(dev);
        goto out;
    }
    else
    {
        // device not support this api, return large range dummy value
        *current_min = 0;
        *current_max = 10000;
        *current_step = 1;
    }
out:
    return ret;
}

int led_set_current(led_dev_t *dev, float current_ma)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    if (dev->ops->set_current != NULL)
    {
        led_get_lock(dev);
        ret = dev->ops->set_current(dev, current_ma);
        led_release_lock(dev);
    }

out:
    return ret;
}

int led_set_current_level(led_dev_t *dev, led_current_level_t current_level)
{
    configASSERT(dev != NULL && dev->ops != NULL);
    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        return -1;
    }

    if (current_level > CURRENT_LVL_MAX)
    {
        LOGE("", "invalid current level:%d.", (int)current_level);
        return -1;
    }

    // map current_level to current
    float current = current_level * led_current_step_ma;

    return led_set_current(dev, current);
}

int led_set_timeout(led_dev_t *dev, int timeout_ms)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    if (dev->ops->set_timeout != NULL)
    {
        led_get_lock(dev);
        ret = dev->ops->set_timeout(dev, timeout_ms);
        led_release_lock(dev);
    }

out:
    return ret;
}

int led_set_trigger_param(led_dev_t *dev, const led_trig_param_t *trigger_param)
{
    configASSERT(dev != NULL && dev->ops != NULL);
    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        return -1;
    }

    if (dev->trig_param != NULL)
    {
        *dev->trig_param = *trigger_param;
    }

    return 0;
}

int led_trigger(led_dev_t *dev)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->trigger != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    led_get_lock(dev);
    ret = dev->ops->trigger(dev);
    led_release_lock(dev);

out:
    return ret;
}

int led_suspend(led_dev_t *dev)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->suspend != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    led_get_lock(dev);
    ret = dev->ops->suspend(dev);
    led_release_lock(dev);

out:
    return ret;
}

int led_resume(led_dev_t *dev)
{
    int ret = 0;
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->resume != NULL);

    if (!dev->is_initialized)
    {
        LOGE("", "led is not initialized.");
        ret = -1;
        goto out;
    }

    led_get_lock(dev);
    ret = dev->ops->resume(dev);
    led_release_lock(dev);

out:
    return ret;
}

int led_get_initialized_devices(led_dev_t **dev, int cnt)
{
    if (cnt <= 0)
        return -1;

    struct udevice *udev;
    int udev_cnt;
    int initialized_dev_cnt = 0;

    device_get_list_head_and_count(&udev, &udev_cnt);

    for (int i = 0; i < udev_cnt; i++)
    {
        if (udev[i].id == UCLASS_LED)
        {
            led_dev_t* led_dev = (led_dev_t*)udev[i].driver;
            if (led_dev->is_initialized)
            {
                dev[initialized_dev_cnt++] = led_dev;
                if (initialized_dev_cnt >= cnt)
                    break;
            }
        }
    }

    return 0;
}

led_dev_t *led_get_initialized_device()
{
    led_dev_t *dev[1] = {NULL};
    led_get_initialized_devices(dev, sizeof(dev) / sizeof(dev[0]));

    return dev[0];
}

led_dev_t *led_find_device(const char *name)
{
    led_dev_t *dev = NULL;
    struct udevice *udev = NULL;
    int ret = device_find_by_id_name(UCLASS_LED, name, &udev);
    if (ret == 0) {
        dev = (led_dev_t*)udev->driver;
    }

    return dev;
}
