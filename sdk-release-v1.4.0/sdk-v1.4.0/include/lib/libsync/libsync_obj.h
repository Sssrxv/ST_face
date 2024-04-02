#ifndef _LIB_SYNC_OBJ_H
#define _LIB_SYNC_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief libsync is designed to release cpu when hardware is running task,
 *        you should use this lib with rtos enable; the implement will make
 *        sure you can call sync_obj_wake_pendings in ISR.
 *        the typical scenario to use this lib is trigger some hardware task
 *        (resize for example) in thread, and wait on the sync obj, wait will
 *        release cpu, when the hardware finish resize task, call
 *        sync_obj_wake_pendings in the ISR to wake the pending thread.
 */

typedef struct sync_obj sync_obj_t;

/**
 * @brief initialize sync object
 * @param[in,out]   sync_obj   sync object to initialize
 * @return 0 for success, -1 for fail
 * @note call #sync_obj_release to release resource
 */
int sync_obj_init(sync_obj_t **sync_obj);

/**
 * @brief release sync object
 * @param[in]   sync_obj   sync object to release, this sync_obj must get from #sync_obj_init
 * @return 0 for success, -1 for fail
 * @sa #sync_obj_init
 */
int sync_obj_release(sync_obj_t *sync_obj);

/**
 * @brief wait on sync object
 * @param[in]   sync_obj   sync object to wait, caller will
 *                         release cpu until sync object is fullfiled
 * @return 0 for success, -1 for fail
 * @sa #sync_obj_init #sync_obj_timed_wait
 */
int sync_obj_wait(sync_obj_t *sync_obj);

/**
 * @brief wait on sync object with timeout
 * @param[in]   sync_obj   sync object to wait, caller will
 *                         release cpu until sync object is fullfiled or timeout
 * @return 0 for success, -1 for fail, timeout for example
 * @sa #sync_obj_init #sync_obj_wait
 */
int sync_obj_timed_wait(sync_obj_t *sync_obj, long timeout_ms);

/**
 * @brief wake up task/thread which pending on the sync_obj
 * @param[in]   sync_obj   sync object to signal, caller which wait on 
 *                         this sync object will wake up
 * @return 0 for success, -1 for fail
 * @sa #sync_obj_init #sync_obj_wait #sync_obj_timed_wait
 * @note this api is typically called in hardware task finish ISR
 */
int sync_obj_wake_pendings(sync_obj_t *sync_obj);

#ifdef __cplusplus
}
#endif

#endif
