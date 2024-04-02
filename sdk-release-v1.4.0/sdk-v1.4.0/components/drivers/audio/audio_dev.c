#include "audio_dev.h"
#include "syslog.h"
#include "udevice.h"

static int audio_dev_get_lock(audio_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_lock(&dev->mutex);
    }
#endif
    return ret;
}

static int audio_dev_release_lock(audio_dev_t *dev)
{
    int ret = 0;
#ifdef USE_RTOS
    if (!xPortIsInISR()) {
        ret = pthread_mutex_unlock(&dev->mutex);
    }
#endif
    return ret;
}

audio_dev_t *audio_find_device(const char *name)
{
    audio_dev_t *dev = NULL;
    struct udevice *udev = NULL;
    int ret = device_find_by_id_name(UCLASS_AUDIO_CODEC, name, &udev);
    if (ret == 0) {
        dev = (audio_dev_t*)udev->driver;
    }

    return dev;
}

int audio_dev_init(audio_dev_t *dev, audio_dev_config_t *cfg)
{
    configASSERT(dev != NULL && dev->ops != NULL && dev->ops->init);

    int ret = 0;
    if (!dev->is_initialized)
    {
        ret = dev->ops->init(dev, cfg);
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

int audio_dev_release(audio_dev_t *dev)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->release)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->release(dev);
        dev->is_initialized = ret == 0 ? 0 : 1;
        audio_dev_release_lock(dev);
        if (ret == 0)
        {
#ifdef USE_RTOS
            pthread_mutex_destroy(&dev->mutex);
#endif
        }
    }

    return ret;
}

int audio_dev_suspend(audio_dev_t *dev)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->suspend)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->suspend(dev);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_resume(audio_dev_t *dev)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->resume)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->resume(dev);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_mic_mute(audio_dev_t *dev, bool enable)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->mic_mute)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->mic_mute(dev, enable);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_mic_set_volume(audio_dev_t *dev, int volume)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->mic_set_volume)
    {
        audio_dev_get_lock(dev);
        volume = volume > 100 ? 100 : volume;
        volume = volume < 0 ? 0 : volume;
        ret = dev->ops->mic_set_volume(dev, volume);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_speaker_mute(audio_dev_t *dev, bool enable)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->speaker_mute)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->speaker_mute(dev, enable);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_speaker_set_volume(audio_dev_t *dev, int volume)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->speaker_set_volume)
    {
        audio_dev_get_lock(dev);
        volume = volume > 100 ? 100 : volume;
        volume = volume < 0 ? 0 : volume;
        ret = dev->ops->speaker_set_volume(dev, volume);
        audio_dev_release_lock(dev);
    }

    return ret;
}

int audio_dev_enable_speaker_power(audio_dev_t *dev, bool enable)
{
    configASSERT(dev != NULL && dev->ops != NULL);

    int ret = 0;
    if (dev->is_initialized && dev->ops->enable_speaker_power)
    {
        audio_dev_get_lock(dev);
        ret = dev->ops->enable_speaker_power(dev, enable);
        audio_dev_release_lock(dev);
    }

    return ret;
}

const char *audio_dev_get_name(audio_dev_t *dev)
{
    configASSERT(dev != NULL);

    return dev->name;
}
