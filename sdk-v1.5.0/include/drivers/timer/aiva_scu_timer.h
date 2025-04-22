#ifndef __AIVA_SCU_TIMER_H__
#define __AIVA_SCU_TIMER_H__

#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif


typedef int (*scu_timer_callback_t)(void *ctx);

void scu_gl_timer_init();
void scu_pv_timer_init();

void scu_gl_timer_set_single_shot(int is_single_shot);
void scu_pv_timer_set_single_shot(int is_single_shot);

int  scu_gl_timer_irq_register(int is_single_shot, uint32_t priority, 
        scu_timer_callback_t cb, void *ctx);
int  scu_pv_timer_irq_register(int is_single_shot, uint32_t priority, 
        scu_timer_callback_t cb, void *ctx);

int  scu_gl_timer_irq_unregister();
int  scu_pv_timer_irq_unregister();

void scu_gl_timer_set_enable(int enbale);
void scu_pv_timer_set_enable(int enbale);

int scu_gl_timer_clear_interp_status();
int scu_pv_timer_clear_interp_status();

void scu_gl_timer_set_interval(uint64_t ns);
void scu_pv_timer_set_interval(uint64_t ns);

uint64_t scu_gl_timer_get_count();


#ifdef __cplusplus
}
#endif

#endif // __AIVA_SCU_TIMER_H__
