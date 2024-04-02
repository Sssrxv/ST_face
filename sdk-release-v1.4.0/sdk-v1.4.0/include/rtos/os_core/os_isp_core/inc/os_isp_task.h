#ifndef __OS_ISP_TASK_H__
#define __OS_ISP_TASK_H__

#include <stdint.h>
#include "os_isp_core.h"
#include "videoin_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *  os_isp_core_pipe_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_task_entry 
 * 
 * @param param                 param to config os isp core
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_task_entry(os_isp_core_param_t *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief open one os isp pipe
 * 
 * @param max_out_cnt          max output buffers
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
os_isp_core_pipe_t os_isp_core_open_pipe(int max_out_cnt);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  close os isp core handle
 * 
 * @param handle
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_close_pipe(os_isp_core_pipe_t handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  push one frame into os isp working queue
 * 
 * @param handle            pipe handle
 * @param buf               input frame buffer
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_push_frame(os_isp_core_pipe_t handle, frame_buf_t *buf_in, frame_param_t *pp_param, os_wait_option_t wait_option);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_push_frame_all 
 * 
 * @param handle            pipe handle
 * @param buf               input frame buffer
 * @param pp_param          pp param for isp
 * @param vp_param          vp param for isp
 * @param mp_param          mp param for isp
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_push_frame_all(os_isp_core_pipe_t handle, frame_buf_t *buf, 
        frame_param_t *pp_param,
        frame_param_t *vp_param,
        frame_param_t *mp_param,
        os_wait_option_t wait_option
        );

/* --------------------------------------------------------------------------*/
/** 
 * @brief  get one frame from os isp ready queue
 * 
 * @param handle            pipe handle
 * @param buf               frame buffer
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_get_frame(os_isp_core_pipe_t handle, frame_buf_t **buf_out, os_wait_option_t wait_option);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_get_frame_all 
 * 
 * @param handle            pipe handle
 * @param buf_pp            pp frame buffer
 * @param buf_vp            vp frame buffer
 * @param buf_mp            mp frame buffer
 * @param wait_option       OS_WAIT_NONE/OS_WAIT_FOREVER
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_get_frame_all(os_isp_core_pipe_t handle, 
        frame_buf_t **buf_pp, 
        frame_buf_t **buf_vp, 
        frame_buf_t **buf_mp, 
        os_wait_option_t wait_option
        );

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_task_exit 
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_task_exit(void);


#ifdef __cplusplus
}
#endif 

#endif  // __OS_ISP_TASK_H__
