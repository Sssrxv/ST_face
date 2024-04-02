#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif


typedef enum {
    WATCH_SIZE_S8  = 0, 
    WATCH_SIZE_S16 = 1,
    WATCH_SIZE_S32 = 2,
    WATCH_SIZE_ANY,
} watch_size_t;

typedef enum {
    WATCH_TYPE_RD = 0,
    WATCH_TYPE_WR = 1,
    WATCH_TYPE_ANY,
} watch_type_t;


int arm_install_hw_watchpoint(int i, uint32_t addr, watch_size_t size, watch_type_t type);

#if defined(__cplusplus)
}
#endif

#endif // __WATCHPOINT_H__
