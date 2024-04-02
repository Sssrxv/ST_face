#ifndef _DRIVER_PWM_H
#define _DRIVER_PWM_H

#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum _pwm_device_number
{
    PWM_DEVICE_0 = 0,
    PWM_DEVICE_1,
    PWM_DEVICE_2,
    PWM_DEVICE_3,
    PWM_DEVICE_4,
    PWM_DEVICE_5,
    PWM_DEVICE_6,
    PWM_DEVICE_7,
    PWM_DEVICE_8,
    PWM_DEVICE_9,
    PWM_DEVICE_10,
    PWM_DEVICE_11,
    PWM_DEVICE_MAX,
} pwm_device_number_t;


/**
 * @brief       Init pwm timer
 *
 * @param[in]   timer       timer
 */
void pwm_init(pwm_device_number_t pwm_number);

/**
 * @brief       Enable timer
 *
 * @param[in]   enable      Enable or disable
 *
 */
void pwm_set_enable(pwm_device_number_t pwm_number,  int enable);

/**
 * @brief       Set pwm duty
 *
 * @param[in]   frequency       pwm frequency
 * @param[in]   duty            duty
 *
 */
double pwm_set_frequency(pwm_device_number_t pwm_number,  double frequency, double duty);

/** 
 * @brief  pwm_is_enabled 
 * 
 * @param pwm_number
 * 
 * @return 0/1 
 */
int pwm_is_enabled(pwm_device_number_t pwm_number);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PWM_H */
