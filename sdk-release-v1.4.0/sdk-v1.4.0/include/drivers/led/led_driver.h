/**
 * @file
 * @brief this layer try to abstract led driver and export unified interface to apps
 *        this abstract layer provid two api groups:
 *        1. group 1: call led_driver_set_current_level to set current level;
 *        2. grou 2 provide more precise current control by set current directly
 *          (1)call led_driver_get_current_info to get min/max/step of current
 *          (2)call led_driver_set_current to set current directly
 */
#ifndef _LED_DRIVER_H_
#define _LED_DRIVER_H_

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


static const int led_current_step_ma = 50; /** current step between current levels */
/**
 * @brief supported current level
 * @note  current level is just a suggestion for actual led driver,
 *        CURRENT_LVL_1 is about 50ma, actual led driver will set
 *        current to the clost value around 50ma which is supported
 *        by actual led driver, so the final current may be 40ma.
 */
typedef enum {
    CURRENT_LVL_0  = 0,  /** ≈ 0 ma */
    CURRENT_LVL_1  = 1,  /** ≈ 50 ma */
    CURRENT_LVL_2  = 2,  /** ≈ 100 ma */
    CURRENT_LVL_3  = 3,  /** ≈ 150 ma */
    CURRENT_LVL_4  = 4,  /** ≈ 200 ma */
    CURRENT_LVL_5  = 5,  /** ≈ 250 ma */
    CURRENT_LVL_6  = 6,  /** ≈ 300 ma */
    CURRENT_LVL_7  = 7,  /** ≈ 350 ma */
    CURRENT_LVL_8  = 8,  /** ≈ 400 ma */
    CURRENT_LVL_9  = 9,  /** ≈ 450 ma */
    CURRENT_LVL_10 = 10, /** ≈ 500 ma */
    CURRENT_LVL_11 = 11, /** ≈ 550 ma */
    CURRENT_LVL_12 = 12, /** ≈ 600 ma */
    CURRENT_LVL_13 = 13, /** ≈ 650 ma */
    CURRENT_LVL_14 = 14, /** ≈ 700 ma */
    CURRENT_LVL_15 = 15, /** ≈ 750 ma */
    CURRENT_LVL_16 = 16, /** ≈ 800 ma */
    CURRENT_LVL_17 = 17, /** ≈ 850 ma */
    CURRENT_LVL_18 = 18, /** ≈ 900 ma */
    CURRENT_LVL_19 = 19, /** ≈ 950 ma */
    CURRENT_LVL_20 = 20, /** ≈ 1000 ma */
    CURRENT_LVL_21 = 21, /** ≈ 1050 ma */
    CURRENT_LVL_22 = 22, /** ≈ 1100 ma */
    CURRENT_LVL_23 = 23, /** ≈ 1150 ma */
    CURRENT_LVL_24 = 24, /** ≈ 1200 ma */
    CURRENT_LVL_25 = 25, /** ≈ 1250 ma */
    CURRENT_LVL_26 = 26, /** ≈ 1300 ma */
    CURRENT_LVL_27 = 27, /** ≈ 1350 ma */
    CURRENT_LVL_28 = 28, /** ≈ 1400 ma */
    CURRENT_LVL_29 = 29, /** ≈ 1450 ma */
    CURRENT_LVL_MAX = 30, /** ≈ 1500 ma */
} led_current_level_t;


// /**
//  * @brief led control channel
//  * @note  some led driver may only have one channel.
//  */
// typedef enum {
//     CHANNEL_0   = 0,
//     CHANNEL_1   = 1,
//     CHANNEL_ALL = 2
// } led_channel_t;

/**
 * @brief param for led trigger
 *        trigger pulse will pull low and then pull up after wait for pulse_in_us
 */
typedef struct {
    int pulse_in_us; /** how long will the trigger pulse will stay at low level */
} led_trig_param_t;

typedef struct led_dev led_dev_t;

/**
 * @brief all supported operations of led device
 */
typedef struct led_ops {
    int (*init)(led_dev_t *dev);                                                      /** init led, device must have this api */
    int (*release)(led_dev_t *dev);                                                   /** release led, device must have this api */
    int (*enable)(led_dev_t *dev);                                 /** enable led, device must have this api */
    int (*disable)(led_dev_t *dev);                                /** disable led, device must have this api */
    int (*get_current_range)(led_dev_t *dev, float *min, float *max, float *step);    /** get min/max/step, set to NULL if not support */
    int (*set_current)(led_dev_t *dev, float current_ma);          /** set led current, set to NULL if not support */
    int (*set_timeout)(led_dev_t *dev, int timeout_ms);            /** set led timtout, led will auto stop work after timeout untill next trigger, set to NULL if not support */
    int (*trigger)(led_dev_t *dev);                                /** trigger led to make it work again, device must have this api */
    int (*suspend)(led_dev_t *dev);                                /** suspend led, led will not work until resume, device must have this api */
    int (*resume)(led_dev_t *dev);                                 /** resume led, device must have this api */
} led_ops_t;

/**
 * @brief led device
 */
typedef struct led_dev {
    char *name;                   /** name of led device */
    led_ops_t *ops;               /** all supported operations of led device */
    int is_initialized;           /** is this led device been initialized, must init device before use it */
    int is_enable;   /** is this led device enabled */
    int is_suspend;               /** is this led suspended */
    int timeout_ms;               /** timeout in milisecond of this led device */
#ifdef USE_RTOS
    pthread_mutex_t mutex;        /** mutex of this led device */      
#endif
    led_trig_param_t *trig_param; /** trigger params of this device */
    void *priv;                   /** private params of this device */
} led_dev_t;

/**
 * @brief Initialize led device
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int led_init(led_dev_t *dev);

/**
 * @brief Release led device
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int led_release(led_dev_t *dev);

/**
 * @brief Enable given led device
 * @note  Led is disabled by default
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int led_enable(led_dev_t *dev);

/**
 * @brief Disable led deivce
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int led_disable(led_dev_t *dev);

/**
 * @brief Get min/max/step current of led
 * @param[in] dev device instance to operate on
 * @param[in,out] current_min min current value in mA which is supported by this led device
 * @param[in,out] current_max max current value in mA which is supported by this led device
 * @param[in,out] current_step current change step in mA which is supported by this led device
 * @return int, 0 for success and other for fail
 */
int led_get_current_range(led_dev_t *chn, float *current_min, float *current_max, float *current_step);

/**
 * @brief Set led current
 * @param[in] dev device instance to operate on
 * @param[in] current_ma current value in mA will set to led
 * @return int, 0 for success and other for fail
 * @sa #led_get_current_range
 */
int led_set_current(led_dev_t *chn, float current_ma);

/**
 * @brief Set current level
 * @param[in] dev device instance to operate on
 * @param[in] current_level current level will set to led
 * @return int, 0 for success and other for fail
 * @sa #led_set_current #led_current_level_t
 */
int led_set_current_level(led_dev_t *chn, led_current_level_t current_level);

/**
 * @brief Set led timtout, led will auto stop work after timeout untill next trigger
 * @param[in] dev device instance to operate on
 * @param[in] timeout_ms timeout in milisecond
 * @return int, 0 for success and other for fail
 * @sa #led_trigger
 */
int led_set_timeout(led_dev_t *chn, int timeout_ms);

/**
 * @brief Set led trigger param
 * @param[in] dev device instance to operate on
 * @param[in] trigger_param new trigger param
 * @return int, 0 for success and other for fail
 */
int led_set_trigger_param(led_dev_t *dev, const led_trig_param_t *trigger_param);

/**
 * @brief Trigger led to make it work again
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 * @sa #led_set_timeout
 */
int led_trigger(led_dev_t *dev);

/**
 * @brief Suspend led
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 * @sa #led_resume
 */
int led_suspend(led_dev_t *dev);

/**
 * @brief Resume led
 * @param[in] dev device instance to operate on
 * @return int, 0 for success and other for fail
 * @sa led_suspend
 */
int led_resume(led_dev_t *dev);

/**
 * @brief Get initialized led devices
 * @param[in, out] dev device instance array to fill
 * @param[in] cnt max instance to fill the instance array
 * @return int, 0 for success and other for fail
 */
int led_get_initialized_devices(led_dev_t **dev, int cnt);

/**
 * @brief Get one initialized led device, return the first one found
 * @return initialized device, NULL if no device initialized
 */
led_dev_t *led_get_initialized_device();

/**
 * @brief Find led device with giving name
 * @return led device, NULL if no device found
 */
led_dev_t *led_find_device(const char *name);

#ifdef __cplusplus
}
#endif

#endif
