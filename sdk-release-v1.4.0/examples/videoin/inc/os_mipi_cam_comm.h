#ifndef __OS_MIPI_CAM_COMM_H__
#define __OS_MIPI_CAM_COMM_H__

#include <stdint.h>
#include "videoin.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    videoin_id_t id;
    cis_dev_driver_t *drv;
    frame_fmt_t frame_fmt;
    int skip_power_on;
    int enable_trigger_led;
} os_mipi_mono_cam_param_t;

#define OS_MIPI_MONO_CAM_PARAM_INITIALIZER \
    (((os_mipi_mono_cam_param_t){          \
        .id = 0,                           \
        .drv = NULL,                       \
        .frame_fmt = FRAME_FMT_RAW10,      \
        .skip_power_on = 0,                \
        .enable_trigger_led = 1,           \
    }))

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mipi_mono_camera_init 
 * 
 * @param mipi_mono_cam_param
 * 
 * @return
 */
/* --------------------------------------------------------------------------*/
videoin_context_t *mipi_mono_camera_init(os_mipi_mono_cam_param_t *mipi_mono_cam_param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mipi_mono_camera_uninit 
 * 
 * @param ctx
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int mipi_mono_camera_uninit(videoin_context_t *ctx);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mipi_stereo_camera_init 
 * 
 * @param id_l
 * @param id_r
 * @param drv_l
 * @param drv_r
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
videoin_context_t *mipi_stereo_camera_init(videoin_id_t id_l, videoin_id_t id_r, cis_dev_driver_t *drv_l, cis_dev_driver_t *drv_r);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mipi_stereo_camera_uninit 
 * 
 * @param ctx
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int mipi_stereo_camera_uninit(videoin_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif //__OS_MIPI_CAM_COMM_H__
