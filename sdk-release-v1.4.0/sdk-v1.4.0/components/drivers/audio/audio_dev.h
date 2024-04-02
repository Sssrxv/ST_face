/**
 * @file
 * @brief this driver try to abstract audio device driver and export unified interface to apps
 */
#ifndef _AUDIO_DEV_H_
#define _AUDIO_DEV_H_

#include "i2c.h"
#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select media hal codec mode
 */
typedef enum audio_codec_mode {
    AUDIO_CODEC_MODE_ENCODE = 1,  /** select adc */
    AUDIO_CODEC_MODE_DECODE,      /** select dac */
    AUDIO_CODEC_MODE_BOTH,        /** select both adc and dac */
    AUDIO_CODEC_MODE_LINE_IN,     /** set adc channel */
} audio_codec_mode_t;

/**
 * @brief Select adc channel for input mic signal
 */
typedef enum audio_adc_input {
    AUDIO_ADC_INPUT_LINE1 = 0x00,  /** mic input to adc channel 1 */
    AUDIO_ADC_INPUT_LINE2,         /** mic input to adc channel 2 */
    AUDIO_ADC_INPUT_ALL,           /** mic input to both channels of adc */
    AUDIO_ADC_INPUT_DIFFERENCE,    /** mic input to adc difference channel */
} audio_adc_input_t;

/**
 * @brief Select channel for dac output
 */
typedef enum audio_dac_output {
    AUDIO_DAC_OUTPUT_LINE1 = 0x00,  /** dac output signal to channel 1 */
    AUDIO_DAC_OUTPUT_LINE2,         /** dac output signal to channel 2 */
    AUDIO_DAC_OUTPUT_ALL,           /** dac output signal to both channels */
} audio_dac_output_t;

/**
 * @brief Select I2S interface operating mode i.e. master or slave for audio codec chip
 */
typedef enum audio_iface_mode {
    AUDIO_MODE_SLAVE = 0x00,   /** set slave mode */
    AUDIO_MODE_MASTER = 0x01,  /** set master mode */
} audio_iface_mode_t;

/**
 * @brief Select I2S interface samples per second
 */
typedef enum audio_iface_samples {
    AUDIO_08K_SAMPLES,   /** set to  8k samples per second */
    AUDIO_11K_SAMPLES,   /** set to 11.025k samples per second */
    AUDIO_16K_SAMPLES,   /** set to 16k samples in per second */
    AUDIO_22K_SAMPLES,   /** set to 22.050k samples per second */
    AUDIO_24K_SAMPLES,   /** set to 24k samples in per second */
    AUDIO_32K_SAMPLES,   /** set to 32k samples in per second */
    AUDIO_44K_SAMPLES,   /** set to 44.1k samples per second */
    AUDIO_48K_SAMPLES,   /** set to 48k samples per second */
} audio_iface_samples_t;

/**
 * @brief Select I2S interface number of bits per sample
 */
typedef enum audio_iface_bits {
    AUDIO_BIT_LENGTH_16BITS = 1,   /** set 16 bits per sample */
    AUDIO_BIT_LENGTH_24BITS,       /** set 24 bits per sample */
    AUDIO_BIT_LENGTH_32BITS,       /** set 32 bits per sample */
} audio_iface_bits_t;

/**
 * @brief Select I2S interface format for audio codec chip
 */
typedef enum audio_iface_format {
    AUDIO_I2S_NORMAL = 0,  /** set normal I2S format */
    AUDIO_I2S_LEFT,        /** set all left format */
    AUDIO_I2S_RIGHT,       /** set all right format */
    AUDIO_I2S_DSP,         /** set dsp/pcm format */
} audio_iface_format_t;

/**
 * @brief I2S interface configuration for audio codec chip
 */
typedef struct audio_codec_i2s_iface {
    audio_iface_mode_t mode;        /** audio codec chip mode */
    audio_iface_format_t fmt;       /** I2S interface format */
    audio_iface_samples_t samples;  /** I2S interface samples per second */
    audio_iface_bits_t bits;        /** i2s interface number of bits per sample */
} audio_codec_i2s_iface_t;

/**
 * @brief I2C interface configuration for audio codec chip
 */
typedef struct audio_dev_ctrl_interface {
    i2c_device_number_t i2c_num;
    uint32_t i2c_addr;
    uint32_t i2c_addr_width;
    uint32_t i2c_clk;
} audio_dev_ctrl_interface_t;

/**
 * @brief Configure media hal for initialization of audio codec chip
 */
typedef struct audio_dev_config {
    audio_adc_input_t adc_input;           /** set adc channel */
    audio_dac_output_t dac_output;         /** set dac channel */
    audio_codec_mode_t codec_mode;         /** select codec mode: adc, dac or both */
    audio_codec_i2s_iface_t i2s_iface;     /** set I2S interface configuration */
    // NOTE: we think all audio codec use i2c as ctrl interface
    audio_dev_ctrl_interface_t i2c_iface;  /** set I2C interface configuration */
    int default_volume;                    /** default voice volume */
    int codec_pa_pin;                      /** pin num of pa power */
    int codec_spk_en_pin;                  /** pin num of speaker en*/
} audio_dev_config_t;

#define AUDIO_DEV_CONFIG_INITIALIZER          \
    (((audio_dev_config_t){                   \
        .adc_input = AUDIO_ADC_INPUT_LINE1,   \
        .dac_output = AUDIO_DAC_OUTPUT_LINE1, \
        .codec_mode = AUDIO_CODEC_MODE_BOTH,  \
        .i2s_iface = {                        \
            .mode = AUDIO_MODE_SLAVE,         \
            .fmt = AUDIO_I2S_NORMAL,          \
            .samples = AUDIO_48K_SAMPLES,     \
            .bits = AUDIO_BIT_LENGTH_16BITS,  \
        },                                    \
        .i2c_iface = {                        \
            .i2c_num = I2C_DEVICE_3,          \
            .i2c_addr = 0x18,                 \
            .i2c_addr_width = 7,              \
            .i2c_clk = 20000                  \
        },                                    \
        .default_volume = 65,                 \
        .codec_pa_pin = 27,                   \
        .codec_spk_en_pin = 24,               \
        }))

typedef struct audio_dev audio_dev_t;

typedef struct audio_dev_ops {
    int (*init)(audio_dev_t *dev, audio_dev_config_t *cfg);            /** init audio device, device must provide this api */
    int (*release)(audio_dev_t *dev);                                  /** release audio device */
    int (*suspend)(audio_dev_t *dev);                                  /** suspend device */
    int (*resume)(audio_dev_t *dev);                                   /** resume device */
    int (*speaker_mute)(audio_dev_t *dev, bool enable);                /** mute audio device*/
    int (*speaker_set_volume)(audio_dev_t *dev, int volume);           /** set volume of device, 0~100 */
    int (*mic_mute)(audio_dev_t *dev, bool enable);                    /** mute audio device*/
    int (*mic_set_volume)(audio_dev_t *dev, int volume);               /** mute audio device*/
    int (*enable_speaker_power)(audio_dev_t *dev, bool enable);        /** enable/disable power of speaker*/
} audio_dev_ops_t;

typedef struct audio_dev {
    const char *name;                                   /** name of audio device */
    audio_dev_ops_t *ops;                               /** all supported operations of audio device */
    int is_initialized;                                 /** is this audio device been initialized, must initialize duvice before use it */
    audio_dev_config_t cfg;                             /** configuration of this device */
#ifdef USE_RTOS
    pthread_mutex_t mutex;                              /** mutex of this device */
#endif
    void *priv;                                         /** private data of this device */
} audio_dev_t;

/**
 * @brief Finde audio device by name
 * @param[in] name name of audio device need to find
 * @return audio device found with given name
 */
audio_dev_t *audio_find_device(const char *name);

/**
 * @brief Initialize audio device by name
 * @param[in] dev audio device need to initialize
 * @return int, 0 for success and other for fail
 * @sa audio_dev_release
 */
int audio_dev_init(audio_dev_t *dev, audio_dev_config_t *cfg);

/**
 * @brief Release audio device
 * @param[in] dev audio device need to release
 * @return int, 0 for success and other for fail
 * @sa audio_dev_init
 */
int audio_dev_release(audio_dev_t *dev);

/**
 * @brief Get name of audio device
 * @param[in] dev device to get name
 * @return name of device
 */
const char *audio_dev_get_name(audio_dev_t *dev);

/**
 * @brief Suspend audio devive
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 * @sa audio_dev_resume
 */
int audio_dev_suspend(audio_dev_t *dev);

/**
 * @brief Resume audio devive
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 * @sa audio_dev_suspend
 */
int audio_dev_resume(audio_dev_t *dev);

/**
 * @brief Mute speaker of audio device
 * @param[in] dev device instance to operate on
 * @param[in] enable true - mute, false - unmute
 * @return int, 0 for success and other for fail
 * @sa audio_dev_speaker_set_volume
 */
int audio_dev_speaker_mute(audio_dev_t *dev, bool enable);

/**
 * @brief Set volume of audio devive
 * @param[in] dev device instance to operate on
 * @param[in] volume new volume to set
 * @return int, 0 for success and other for fail
 * @sa audio_dev_speaker_mute
 */
int audio_dev_speaker_set_volume(audio_dev_t *dev, int volume);

/**
 * @brief Mute mic of audio device
 * @param[in] dev device instance to operate on
 * @param[in] enable true - mute, false - unmute
 * @return int, 0 for success and other for fail
 * @sa audio_dev_mic_set_volume
 */
int audio_dev_mic_mute(audio_dev_t *dev, bool enable);

/**
 * @brief Set volume of audio devive
 * @param[in] dev device instance to operate on
 * @param[in] volume new volume to set
 * @return int, 0 for success and other for fail
 * @sa audio_dev_mic_mute
 */
int audio_dev_mic_set_volume(audio_dev_t *dev, int volume);

/**
 * @brief Enable/Disable power of speaker
 * @param[in] dev device instance to operate on
 * @param[in] enable enable or disable power of speaker
 * @return int, 0 for success and other for fail
 */
int audio_dev_enable_speaker_power(audio_dev_t *dev, bool enable);


#ifdef __cplusplus
}
#endif

#endif
