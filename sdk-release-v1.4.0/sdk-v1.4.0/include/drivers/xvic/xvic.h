#ifndef _DRIVER_XVIC_H
#define _DRIVER_XVIC_H

#include <stdint.h>


/* For c++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif

typedef int xvic_irq_t;
/* IRQ number settings */
#define XVIC_NUM_SOURCES (66)

/**
 * @brief       Definitions for the interrupt callbacks
 */
typedef int (*xvic_irq_callback_t)(void *ctx);

/**
 * @brief       Initialize XVIC external interrupt
 *
 * @note        This function will set MIP_MEIP. The MSTATUS_MIE must set by user.
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void xvic_init(void);

/** 
 * @brief  xvic_irq_is_enabled 
 * 
 * @param irq_number
 * 
 * @return
 *     - 0      Not enabled
 *     - 1      Enabled
 */
int xvic_irq_is_enabled(xvic_irq_t irq_number);

/**
 * @brief       Enable XVIC external interrupt
 *
 * @param[in]   irq_number      external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */

int xvic_irq_enable(xvic_irq_t irq_number);

/**
 * @brief       Disable XVIC external interrupt
 *
 * @param[in]   irq_number  The external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int xvic_irq_disable(xvic_irq_t irq_number);

/**
 * @brief       Set IRQ priority
 *
 * @param[in]   irq_number      The external interrupt number
 * @param[in]   priority        The priority of external interrupt number
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int xvic_set_priority(xvic_irq_t irq_number, uint32_t priority);

/**
 * @brief       Get IRQ priority
 *
 * @param[in]   irq_number          The external interrupt number
 *
 * @return      The priority of external interrupt number
 */
uint32_t xvic_get_priority(xvic_irq_t irq_number);

/**
 * @brief       Claim an IRQ
 *
 * @return      The current IRQ number
 */
uint32_t xvic_irq_claim(void);

/**
 * @brief       Complete an IRQ
 *
 * @param[in]   source      The source IRQ number to complete
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int xvic_irq_complete(uint32_t source);

/**
 * @brief       Register user callback function by IRQ number
 *
 * @param[in]   irq             The irq
 * @param[in]   callback        The callback
 * @param       ctx             The context
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void xvic_irq_register(xvic_irq_t irq, xvic_irq_callback_t callback, void *ctx);


/**
 * @brief       Deegister user callback function by IRQ number
 *
 * @param[in]   irq     The irq
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
void xvic_irq_unregister(xvic_irq_t irq);

/** 
 * @brief       Save and disable irq
 * 
 * @param irq           irq number
 * @param flag_save     save irq state to flag_save
 */
void xvic_irq_save(xvic_irq_t irq, uint8_t *flag_save);

/** 
 * @brief       Restore flag to irq
 * 
 * @param irq
 * @param flag
 */
void xvic_irq_restore(xvic_irq_t irq, uint8_t flag);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  freeze irq enable operation so that the irq state can be kept diable
 * 
 * @return 0
 */
/* --------------------------------------------------------------------------*/
int xvic_irq_freeze(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  activate irq enable operation 
 * 
 * @return 0
 */
/* --------------------------------------------------------------------------*/
int xvic_irq_activate(void);

/* For c++ compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_XVIC_H */
