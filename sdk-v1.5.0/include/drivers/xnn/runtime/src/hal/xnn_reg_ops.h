#ifndef XNN_REG_OPS_H_
#define XNN_REG_OPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void REG_WR(uint32_t addr, uint32_t value);
    void REG_RD(uint32_t addr, uint32_t value);
    void MEM_WR(const char * filename, uint32_t addr, int size);
    void MEM_DF(const char * filename, uint32_t addr, int size);
    void* XNN_MEM_ALLOC(int size, int align);
    int XNN_MEM_FREE(void* addr);

#ifdef __cplusplus
}
#endif


#endif
