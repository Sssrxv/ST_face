#ifndef __AIVA_MALLOC_H__
#define __AIVA_MALLOC_H__
/*
 * A simple fast malloc implementation for embedded system
 *
 */
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_TLSF_HEAP
#include "tlsf_malloc.h"

typedef struct _mem_pool_t {
    int      has_inited;
    void    *start_addr;
    void    *last_addr;
    size_t   total_size;
    tlsf_t   tlsf_heap;
} __attribute__((packed, aligned(4)))mem_pool_t;
#else
/* memory pool */
typedef struct _mem_pool_t {
    int      has_inited;
    void    *start_addr;
    void    *last_addr;
    void    *cur_avail_addr;
    size_t   total_size;
    /* debug purpose */
    size_t   stat_sum;
    size_t   stat_total;
} __attribute__((packed, aligned(4)))mem_pool_t;

extern mem_pool_t g_norm_mem_pool;
extern mem_pool_t g_dma_mem_pool;
#endif

int check_heap(mem_pool_t *mem_pool);

void* aiva_malloc(size_t size);
void* aiva_malloc_aligned(size_t size, const uint32_t align_size);
void  aiva_free(void *userptr);
void* aiva_calloc(size_t nitems, size_t size);
void* aiva_realloc(void *ptr, size_t size);

void* aiva_dma_alloc(size_t size);
void* aiva_dma_alloc_aligned(size_t size, const uint32_t align_size);
void  aiva_dma_free(void *userptr);

size_t aiva_get_free_heap_size(void);

#ifdef __cplusplus
}
#endif

#endif
