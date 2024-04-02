#ifndef _AIVA_SLEEP_H
#define _AIVA_SLEEP_H

#include <stdint.h>
#include "syslog.h"

#ifdef __cplusplus
extern "C" {
#endif

int aiva_usleep(uint64_t usec);
int aiva_msleep(uint32_t msec);
int aiva_sleep(uint32_t seconds);

int aiva_busy_delay_us(uint64_t usec);
int aiva_busy_delay_ms(uint32_t msec);

uint32_t aiva_get_curr_ms(void);

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */

#define typecheck(type, x)          \
({  type __dummy;                   \
    typeof(x) __dummy2;             \
    (void)(&__dummy == &__dummy2);  \
    1;                              \
 })

#define time_after(a, b)            \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((long)((b) - (a)) < 0))

#define time_before(a, b) time_after(b, a)

#define PRINT_CURR_LINE_MS() LOGW(__func__, "line(%d): %d ms", __LINE__, aiva_get_curr_ms())

#ifdef __cplusplus
}
#endif

#endif /* _BSP_SLEEP_H */

