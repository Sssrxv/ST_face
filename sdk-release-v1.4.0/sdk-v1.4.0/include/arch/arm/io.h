#ifndef _DRIVER_IO_H
#define _DRIVER_IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define readb(addr) (*(volatile uint8_t *)(addr))
#define readw(addr) (*(volatile uint16_t *)(addr))
#define readl(addr) (*(volatile uint32_t *)(addr))
#define readq(addr) (*(volatile uint64_t *)(addr))

#define writeb(v, addr)                                                     \
    {                                                                       \
        (*(volatile uint8_t *)(addr)) = (v);                                \
    }
#define writew(v, addr)                                                     \
    {                                                                       \
        (*(volatile uint16_t *)(addr)) = (v);                               \
    }
#define writel(v, addr)                                                     \
    {                                                                       \
        (*(volatile uint32_t *)(addr)) = (v);                               \
    }
#define writeq(v, addr)                                                     \
    {                                                                       \
        (*(volatile uint64_t *)(addr)) = (v);                               \
    }

typedef uint32_t (*readl_func_t)(volatile void *addr);
typedef void (*writel_func_t)(volatile void *addr, uint32_t data);
typedef int (*checkl_func_t)(volatile void *addr, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_IO_H */
