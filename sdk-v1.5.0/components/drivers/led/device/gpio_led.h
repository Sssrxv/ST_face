#ifndef _GPIO_LED_H_
#define _GPIO_LED_H_

#if USE_RTOS
#include "FreeRTOS.h"
#include "timers.h"
#endif
#include <stdint.h>

/**
 * @brief private data for gpio led device
 * @note see led_driver.h
 */
typedef struct {
    uint8_t pin;          /** control gpio pin number of this led device */
#if USE_RTOS
    TimerHandle_t timer;  /** software timer which will be used to handle led timeout */
#endif
} gpio_led_t;

#endif
