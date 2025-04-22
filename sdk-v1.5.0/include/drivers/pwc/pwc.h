#ifndef __PWC_H__
#define __PWC_H__

#include <stdint.h>
#include "xvic/xvic.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define PWC_INTR_PRI              (19 << 3)

typedef enum _pwc_device_number
{
    PWC_DEVICE_0,
    PWC_DEVICE_1,
    PWC_DEVICE_2,
    PWC_DEVICE_3,
    PWC_DEVICE_4,
    PWC_DEVICE_5,
    PWC_DEVICE_MAX,
} pwc_device_number_t;

/**
 * @brief       Init pwc device
 *
 * @param[in]   num         PWC index
 *
 * @return      void
 */
void pwc_init(pwc_device_number_t num);

/**
 * @brief       SET PWC device clock
 *
 * @param[in]   num         PWC index
 * @param[in]   pulse       The extern pulse cycle
 *
 * @return      void
 */
void pwc_set_system_clock(pwc_device_number_t num, uint32_t extern_pulse);

/**
 * @brief       SET PWC interrupt threshold
 *
 * @param[in]   num         PWC index
 * @param[in]   threshold   The threshold value
 *
 * @return      void
 */
void pwc_set_threshold(pwc_device_number_t num, uint32_t threshold);

/**
 * @brief       Get PWC count
 *
 * @param[in]   num         PWC index
 *
 * @return      void
 */
uint32_t pwc_get_count(pwc_device_number_t num);

/**
 * @brief       Enable PWC device
 *
 * @param[in]   num         PWC index
 *
 * @return      void
 */
void pwc_enable(pwc_device_number_t num);

/**
 * @brief       Disable PWC device
 *
 * @param[in]   num         PWC index
 *
 * @return      void
 */
void pwc_disable(pwc_device_number_t num);

/**
 * @brief       Register PWC device callback
 *
 * @param[in]   num         PWC index
 * @param[in]   callback    The pwc callback
 * @param[in]   ctx         The param of the callback
 * @param[in]   priority    The priority of the irq
 *
 * @return      Transfer length
 */
void pwc_irq_register(pwc_device_number_t num, xvic_irq_callback_t pwc_callback, void *ctx, uint32_t priority);

#ifdef __cplusplus
}
#endif 

#endif //__PWC_H__
