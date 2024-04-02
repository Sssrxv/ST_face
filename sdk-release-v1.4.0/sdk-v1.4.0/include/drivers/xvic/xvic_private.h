#ifndef _DRIVER_XVIC_PRIVATE_H
#define _DRIVER_XVIC_PRIVATE_H

#include "AI3100.h"
#include "irq_ctrl.h"

/* For c++ compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void cmsis_irq_callback_handler(IRQn_ID_t irqn);


/* For c++ compatibility */
#ifdef __cplusplus
}
#endif

#endif
