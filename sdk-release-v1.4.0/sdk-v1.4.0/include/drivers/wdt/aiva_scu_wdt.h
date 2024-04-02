#ifndef __AIVA_SCU_WDT_H__
#define __AIVA_SCU_WDT_H__

#include <stdint.h>
#include <stddef.h>
#include "xvic/xvic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WDT_INTR_PRI              (19 << 3)
#define WDT_DEFAULT_TIMEOUT       (10 * 1000 * 1000UL)

typedef enum _wdt_work_mode
{
    WDT_TIMER_MODE = 0,
    WDT_WATCHDOG_MODE = 1,
} wdt_work_mode_t;


uint32_t scu_wdt_init(uint64_t time_out_us);

void scu_wdt_start();

void scu_wdt_stop();

void scu_wdt_feed();

void scu_wdt_work_mode(wdt_work_mode_t mode);

int scu_is_wdt_reset();

uint32_t scu_wdt_get_count();

uint32_t scu_wdt_get_status();

void scu_wdt_irq_enable(void);

void scu_wdt_irq_disable(void);

void scu_wdt_irq_register(xvic_irq_callback_t wdt_external_callback, void *ctx, uint32_t priority);

#ifdef __cplusplus
}
#endif

#endif // __AIVA_SCU_WDT_H__
