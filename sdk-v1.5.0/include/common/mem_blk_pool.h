#ifndef __MEM_BLK_POOL_H__
#define __MEM_BLK_POOL_H__
#include <stdint.h>
#include <stdbool.h>
#include "xlist.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif // USE_RTOS
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_create 
 * 
 * @param name
 * @param blk_size          blk size
 * @param blk_cnt           blk cnt
 * @param addr_align        addr alignment
 * 
 * @return mem blk pool handle
 */
/* --------------------------------------------------------------------------*/
void *mem_blk_pool_create(const char *name, uint32_t blk_size, uint32_t blk_cnt, uint32_t addr_align);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_destroy 
 * 
 * @param pool_handle
 */
/* --------------------------------------------------------------------------*/
void  mem_blk_pool_destroy(void *pool_handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_get, get free memory block
 * 
 * @param pool_handle       pool hanle
 * @param bytes             the bytes should be smaller than memory block size
 * 
 * @return data pointer of free memory block
 */
/* --------------------------------------------------------------------------*/
void *mem_blk_pool_get(void *pool_handle, uint32_t bytes);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_put 
 * 
 * @param pool_handle       pool handle
 * @param ptr               data pointer of memory block
 */
/* --------------------------------------------------------------------------*/
void  mem_blk_pool_put(void *pool_handle, void *ptr);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  is_valid_mem_blk_pool 
 * 
 * @param pool_handle
 * 
 * @return  true/false
 */
/* --------------------------------------------------------------------------*/
bool is_valid_mem_blk_pool(void *pool_handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_find 
 * 
 * @param  name
 * 
 * @return mem blk pool handle if found
 */
/* --------------------------------------------------------------------------*/
void *mem_blk_pool_find(const char *name);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_free_blk_cnt, get free mem blk count
 * 
 * @param pool_handle
 * 
 * @return free block count  
 */
/* --------------------------------------------------------------------------*/
int mem_blk_pool_free_blk_cnt(void *pool_handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_used_blk_cnt, get used mem blk count 
 * 
 * @param pool_handle
 * 
 * @return used block count  
 */
/* --------------------------------------------------------------------------*/
int mem_blk_pool_used_blk_cnt(void *pool_handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mem_blk_pool_max_blk_cnt, get max mem blk count
 * 
 * @param pool_handle
 * 
 * @return max memory block  
 */
/* --------------------------------------------------------------------------*/
int mem_blk_pool_max_blk_cnt(void *pool_handle);

#ifdef __cplusplus
}
#endif

#endif

