#ifndef XNN_HAL_H_
#define XNN_HAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void xnn_hal_reg_write(uint32_t addr, uint32_t value);
    void xnn_hal_reg_check(uint32_t addr, uint32_t value);
    int xnn_hal_execute(const uint8_t *addr, int count, int use_rbc);
    void* xnn_hal_mem_alloc(int size, int align);
    int xnn_hal_mem_free(void* addr);

#ifdef __cplusplus
}
#endif


#endif
