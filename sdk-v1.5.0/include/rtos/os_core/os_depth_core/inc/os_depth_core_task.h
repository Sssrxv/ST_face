#ifndef __OS_DEPTH_CORE_TASK_H__
#define __OS_DEPTH_CORE_TASK_H__

#include <stdint.h>
#include "os_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *  os_depth_core_pipe_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_depth_core_task_entry, create and start os depth core task
 * 
 * @param cam_l             left  camera param
 * @param cam_r             right camera param
 * @param Q                 Q param
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_task_entry(void *cam_l, void *cam_r, void *Q);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  open depth core pipe
 * 
 * @param max_out_cnt       max output buffers
 * 
 * @return pipe handle if OK, NULL if failed   
 */
/* --------------------------------------------------------------------------*/
os_depth_core_pipe_t os_depth_core_open_pipe(int max_out_cnt);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  close depth core pipe
 * 
 * @param handle            depth pipe handle
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_close_pipe(os_depth_core_pipe_t handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_depth_core_push_frame 
 * 
 * @param handle            depth pipe handle
 * @param buf               frame buffer
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_push_frame(os_depth_core_pipe_t handle, frame_buf_t *buf, os_wait_option_t wait_option);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_depth_core_get_frames 
 * 
 * @param handle            depth pipe handle
 * @param buf_depth         pointer which points to depth frame buffer
 * @param buf_rec           pointer which points to recitfy frame buffer
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_get_frames(os_depth_core_pipe_t handle, frame_buf_t **buf_depth, frame_buf_t **buf_rec, os_wait_option_t wait_option);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_depth_core_task_exit 
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_task_exit(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_depth_core_get_frame_count 
 * 
 * @return 0 - OK, -1 - FAIL 
 */
/* --------------------------------------------------------------------------*/
int os_depth_core_get_frame_count(void);

#ifdef __cplusplus
}
#endif 

#endif  // __OS_DEPTH_CORE_TASK_H__
