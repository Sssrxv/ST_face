#ifndef __MPQ7235_H__
#define __MPQ7235_H__

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
} mpq7235_cfg_t;

void gpio_led_set_brightness(const double duty);

int gpio_led_get_nfault(void);

#endif
