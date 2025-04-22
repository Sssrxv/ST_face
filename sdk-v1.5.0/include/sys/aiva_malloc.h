#ifndef __AIVA_MALLOC_H__
#define __AIVA_MALLOC_H__
/*
 * A simple fast malloc implementation for embedded system
 *
 */
#include <stdint.h>
#include <stddef.h>

// #define SMALL_SIZE                  (1060) // make sure not smaller than db item(1060)
// #define SMALL_SIZE_POOL             (307200+100*1060 + 496) // *100 for 100 user +496 for 32align
#define SMALL_SIZE                  (CONFIG_SMALL_MEM_SIZE_BYTE)
#define SMALL_SIZE_POOL             (CONFIG_SMALL_MEM_POOL_SIZE_BYTE)

#ifdef __cplusplus
extern "C" {
#endif

/* memory pool */
typedef struct _mem_pool_t {
    const char *name;
    int      has_inited;
    void    *start_addr;
    void    *last_addr;
    void    *cur_avail_addr;
    size_t   total_size;
    void *   pimpl;
    /* debug purpose */
    size_t   num_fragments;
    size_t   stat_total;
    size_t   max_aval_size;
} __attribute__((packed, aligned(4)))mem_pool_t;

int check_heap(mem_pool_t *mem_pool);

void* aiva_malloc(size_t size);
void* aiva_malloc_aligned(size_t size, const uint32_t align_size);
void  aiva_free(void *userptr);
void* aiva_calloc(size_t nitems, size_t size);
void* aiva_realloc(void *ptr, size_t size);

void* aiva_dma_alloc(size_t size);
void* aiva_dma_alloc_aligned(size_t size, const uint32_t align_size);
void  aiva_dma_free(void *userptr);

extern mem_pool_t g_norm_mem_pool;
extern void aiva_default_print_memory_info(mem_pool_t *mem_pool);

#ifdef CONFIG_ENABLE_DYNAMIC_MEMPOOL
// try to create memory poll with given size, will alloc from global memory pool
mem_pool_t *aiva_malloc_create_mempool(const char *name, size_t size);
void aiva_malloc_release_mempool(mem_pool_t *pool);
void *aiva_malloc_in_pool(mem_pool_t *pool, size_t size, uint32_t align_size);
int aiva_malloc_is_addr_in_mempool(void *addr);
int aiva_malloc_is_dynamic_mempool(mem_pool_t *pool);
#endif

int aiva_get_heap_usage(void);

#ifdef __cplusplus
}
#endif

#endif
