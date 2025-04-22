#ifndef _INDEV_TOUCHPAD_SIM_H_
#define _INDEV_TOUCHPAD_SIM_H_

#if USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#include "indev.h"

typedef struct 
{
    int is_pressed;
    int x;
    int y;
    int pad_max_w;
    int pad_max_h;
} touchpad_priv_t; 


#endif
