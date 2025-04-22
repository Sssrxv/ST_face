#include "audio_dev.h"
#include "syslog.h"
#include "udevice.h"
#include "uac_comm.h" // for audio_frame_buf_t
#include "i2s.h"
#include <stdlib.h>

#define DEBUG_AUDIO_DEV_MIC (0)
#define DEBUG_AUDIO_DEV_SPK (0)

#if DEBUG_AUDIO_DEV_MIC || DEBUG_AUDIO_DEV_SPK
#include "fs_file.h"
uint8_t *mic_pcm_buf = NULL;
uint8_t *spk_pcm_buf = NULL;
#endif

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

int audio_dev_convert_sample_rate(audio_iface_samples_t samples)
{
    int sample_rate = 16000;
    switch (samples)
    {
    case AUDIO_08K_SAMPLES:
        sample_rate = 8000;
        break;
    case AUDIO_11K_SAMPLES:
        sample_rate = 11000;
        break;
    case AUDIO_16K_SAMPLES:
        sample_rate = 16000;
        break;
    case AUDIO_22K_SAMPLES:
        sample_rate = 22000;
        break;
    case AUDIO_24K_SAMPLES:
        sample_rate = 24000;
        break;
    case AUDIO_32K_SAMPLES:
        sample_rate = 32000;
        break;
    case AUDIO_44K_SAMPLES:
        sample_rate = 44000;
        break;
    case AUDIO_48K_SAMPLES:
        sample_rate = 48000;
        break;
        break;
    
    default:
        break;
    }

    return sample_rate;
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

        if (cfg->initialize_i2s)
        {
            i2s_init_config_t i2s_init_config = {
                .i2s_config = {
                    .dev_num = I2S_DEVICE_0,
                    .ch_num = I2S_CHANNEL_0,
                    .work_mode = I2S_MASTER,
                    .rxtx_mode = I2S_RXTX,
                    .sample_rate = audio_dev_convert_sample_rate(cfg->i2s_iface.samples),
                    .xfs = 256,
                    .word_select_cycles = SCLK_CYCLES_32,
                    .rx_word_length = RESOLUTION_16_BIT,
                    .rx_fifo_threshold = TRIGGER_LEVEL_10,
                    .tx_word_length = RESOLUTION_16_BIT,
                    .tx_fifo_threshold = TRIGGER_LEVEL_4
                },
                .i2s_dma_config = {
                    .use_dma = 1,
                    .auto_clear = 1,
                    .rxtx_mode = I2S_RXTX,
                    .rx_dma_queue_size = 4*1024,
                    .tx_dma_queue_size = 4*1024,
                    .rx_dma_queue_num = 2,
                    .tx_dma_queue_num = 2,
                }
            };

            i2s_channel_init(&i2s_init_config);
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

static const int BYTES_PER_SAMPLE = 2;
static inline uint32_t get_i2s_raw_data_len_bytes(int num_samples)
{
    return num_samples * 8; // 2channel*4Bytes_per_channel=8bytes contain one sample
}

int audio_dev_mic_get_frame(audio_dev_t *dev, audio_frame_buf_t* frame, uint32_t timeout_ms)
{
    (void)dev;

    int ret = i2s_dma_get_lock(timeout_ms);
    if (ret == 0)
    {
        int num_samples_need_read = frame->num_samples;
        uint32_t i2s_raw_data_len_bytes = get_i2s_raw_data_len_bytes(num_samples_need_read);
        uint8_t *pcm_buf_raw = (uint8_t *)calloc(i2s_raw_data_len_bytes, 1);
        // read i2s raw data
        size_t raw_data_bytes = 0;
        i2s_dma_read(pcm_buf_raw, i2s_raw_data_len_bytes, &raw_data_bytes, timeout_ms);
        i2s_dma_release_lock();
        //
        size_t num_samples_read = raw_data_bytes / 8;
        if ((int)num_samples_read != num_samples_need_read) {
            LOGW(__func__, "no enough data received!!!");
        }
        size_t pcm_data_bytes = num_samples_read * BYTES_PER_SAMPLE;
        uint32_t pcm_data_len_bytes = 0;
        i2s_convert_raw_data_to_pcm((uint32_t *)pcm_buf_raw, raw_data_bytes/4, frame->addr0, &pcm_data_len_bytes, RESOLUTION_16_BIT, CHANNEL_MONO);
        configASSERT(pcm_data_bytes == pcm_data_len_bytes);
        free(pcm_buf_raw);
    }
#if (DEBUG_AUDIO_DEV_MIC)
    static fs_file_t *mic_file = NULL;
    static int mic_buf_offset = 0;
    static const int mic_pcm_buf_size = 10*1024;
    static const char *mic_file_name = "mic.pcm";
    if (mic_pcm_buf == NULL) {
        mic_pcm_buf = malloc(mic_pcm_buf_size);
        mic_file = fs_file_open(mic_file_name, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC);
        mic_buf_offset = 0;
    } else {
        int data_size = frame->num_samples * BYTES_PER_SAMPLE;
        if (mic_buf_offset + data_size <= mic_pcm_buf_size) {
            memcpy(mic_pcm_buf + mic_buf_offset, frame->addr0, data_size);
            LOGI("mic", "offset:%d size:%d", mic_buf_offset, data_size);
            mic_buf_offset += data_size;
        } else if (mic_file) {
            LOGI("mic", "write %d data to file:%s", mic_pcm_buf_size, mic_file_name);
            fs_file_write(mic_file, mic_pcm_buf, mic_pcm_buf_size);
            fs_file_close(mic_file);
            mic_file = NULL;
            free(mic_pcm_buf);
            // NOTE: we not set mic_pcm_buf to NULL, so only save data once
        }
    }
#endif

    return ret;
}

int audio_dev_speaker_play_frame(audio_dev_t *dev, audio_frame_buf_t *frame, uint32_t timeout_ms)
{
    (void)dev;
    size_t bytes_written = 0;

    int ret = i2s_dma_get_lock(timeout_ms);
    if (ret == 0)
    {
        int num_samples = frame->num_samples;
        uint32_t i2s_raw_data_len_bytes = get_i2s_raw_data_len_bytes(num_samples);
        uint8_t *pcm_buf_raw = (uint8_t *)malloc(i2s_raw_data_len_bytes);
        uint32_t raw_data_len_words = 0;
        i2s_convert_pcm_to_raw_data(frame->addr0, num_samples * BYTES_PER_SAMPLE, (uint32_t *)pcm_buf_raw, &raw_data_len_words, RESOLUTION_16_BIT, CHANNEL_MONO);

        ret = i2s_dma_write(pcm_buf_raw, i2s_raw_data_len_bytes, &bytes_written, timeout_ms);
        free(pcm_buf_raw);

        i2s_dma_release_lock();
    }

#if (DEBUG_AUDIO_DEV_SPK)
    static fs_file_t *spk_file = NULL;
    static int spk_buf_offset = 0;
    static const int spk_pcm_buf_size = 100*1024;
    static const char *spk_file_name = "spk.pcm";
    if (spk_pcm_buf == NULL) {
        spk_pcm_buf = malloc(spk_pcm_buf_size);
        spk_file = fs_file_open(spk_file_name, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC);
        spk_buf_offset = 0;
    } else {
        int data_size = frame->num_samples * BYTES_PER_SAMPLE;
        if (spk_buf_offset + data_size <= spk_pcm_buf_size) {
            memcpy(spk_pcm_buf + spk_buf_offset, frame->addr0, data_size);
            LOGI("mic", "offset:%d size:%d", spk_buf_offset, data_size);
            spk_buf_offset += data_size;
        } else if (spk_file) {
            LOGI("spk", "write %d data to file:%s", spk_pcm_buf_size, spk_file_name);
            fs_file_write(spk_file, spk_pcm_buf, spk_pcm_buf_size);
            fs_file_close(spk_file);
            spk_file = NULL;
            free(spk_pcm_buf);
            // NOTE: we not set spk_pcm_buf to NULL, so only save data once
        }
    }
#endif

    return ret;
}
