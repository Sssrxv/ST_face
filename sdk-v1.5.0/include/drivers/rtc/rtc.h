#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rtc_callback_t)(void *ctx);

int rtc_init(int clk_select, uint32_t prescalar, uint32_t cmr);
int rtc_set_enable(int enable);
int rtc_clear_status(void);
uint32_t rtc_get_curr_prescalar(void);
uint32_t rtc_get_curr_counter(void);
uint32_t rtc_get_ris(void);
int rtc_irq_register(uint32_t priority, rtc_callback_t cb, void *ctx);
int rtc_irq_unregister();
int rtc_clear_interp_status();

#ifdef __cplusplus
}
#endif

#endif // __RTC_H__
