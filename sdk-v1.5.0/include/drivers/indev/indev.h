/**
 * @file
 * @brief this layer try to abstract input device driver, now only support touchpad with I2C communication.
 * 
 */
#ifndef _INDEV_H_
#define _INDEV_H_

#include <stdint.h>
#include "i2c.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct indev_dev indev_dev_t;

typedef enum {
    INDEV_TYPE_TOUCHPAD = 0,

    INDEV_TYPE_COUNT,
} indev_type_t;

/**
 * @brief all supported operations of indev device
 */
typedef struct indev_ops {
    int (*init)(indev_dev_t *dev);                                    /** init indev, device must have this api */
    int (*reset)(indev_dev_t *dev);                                   /** reset input device. */
    int (*release)(indev_dev_t *dev);                                 /** release indev, device must have this api */
    int (*enable)(indev_dev_t *dev);                                  /** enable indev, device must have this api */
    int (*disable)(indev_dev_t *dev);                                 /** disable indev, device must have this api */
    int (*get_pressed_pos)(indev_dev_t *dev, int *w, int *h);         /** get pressed pos for touchpad */
    int (*set_pressed_pos)(indev_dev_t *dev, int w, int h);           /** set pressed pos only for sim touchpad */
} indev_ops_t;

/**
 * @brief indev device
 */
typedef struct indev_dev {
    char *name;                     /** name of indev device */
    indev_ops_t *ops;               /** all supported operations of indev device */
    int is_initialized;             /** is this indev device been initialized, must init device before use it */
    int is_enable;                  /** is this indev device enabled, must init device before use it */
    indev_type_t type;              /** now only support touchpad device. */
    i2c_device_number_t i2c_num;    /** input device communicate bus. */
    uint32_t i2c_addr_wr;           /** i2c slave address */
    uint8_t irq_pin_num;            /** touchpad int irq pin num. */
    uint8_t rst_pin_num;            /** touchpad rst pin num. */

#ifdef USE_RTOS
    pthread_mutex_t mutex;        /** mutex of this input device */
#endif

    void *priv;                     /** private params of this device */
} indev_dev_t;

/**
 * @brief Initialize input dev device with device name.
 * @param[in] device_name, input device name from udevice.
 * @return int, none NULL for success and null for fail
 */
indev_dev_t* indev_init(const char *device_name);

/**
 * @brief Release input device
 * @param[in] indev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int indev_release(indev_dev_t* indev);

/**
 * @brief Enable input device
 * @param[in] indev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int indev_enable(indev_dev_t* indev);

/**
 * @brief Disable input device
 * @param[in] indev device instance to operate on
 * @return int, 0 for success and other for fail
 */
int indev_disable(indev_dev_t* indev);

/**
 * @brief if pressed toucpad, get pressed position.
 * @param[in] indev device instance to operate on
 * @param[out] x pressed position X-Axis value.
 * @param[out] y pressed position Y-Axis value.
 * @return int, 0 for touchpad pressed and out position value, less 0 for no any pressed.
 */
int indev_get_pressed_pos(indev_dev_t *indev, int* x, int* y);

/**
 * @brief Set pressed positopn, ONLY for Soft simulation.
 * @param[in] indev device instance to operate on
 * @param[in] x set point of X-axis value.
 * @param[in] y set point of Y-axis value.
 * @return int, 0 for success and other for fail
 */
int indev_set_pressed_pos(indev_dev_t *indev, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
