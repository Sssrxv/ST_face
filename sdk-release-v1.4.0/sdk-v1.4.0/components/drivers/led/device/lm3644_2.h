#ifndef _LM3644_2_H_
#define _LM3644_2_H_

#include "i2c.h"

#if USE_RTOS
#include "FreeRTOS.h"
#include "timers.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#include <stdint.h>

typedef enum {
    CHANNEL_0   = 0,
    CHANNEL_1   = 1,
    CHANNEL_ALL = 2
} lm3644_channel_t;

/**
 * @brief private data for lm3644 led device
 * @note see led_driver.h
 */
typedef struct {
    i2c_device_number_t i2c_num;          /** i2c number */
    uint8_t i2c_addr;                     /** i2c address */
    uint8_t i2c_addr_width;               /** i2c address width */
    uint32_t i2c_clk;                     /** i2c clk */
    uint8_t power_pin;                    /** power pin number */
    uint8_t led_index;                    /** which led is bind to this lm3644 device */
    int use_strobe;                       /** use sensor strobe or gpio to trigger lm3644 */
#if USE_RTOS
    TimerHandle_t timer;                  /** software timer which will be used to handle led timeout */
    pthread_mutex_t mutex;                /** internal mutex */
#endif
} lm3644_led_t;

#endif
