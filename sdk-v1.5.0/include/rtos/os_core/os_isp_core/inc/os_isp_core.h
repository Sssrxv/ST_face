#ifndef __OS_ISP_CORE_H__
#define __OS_ISP_CORE_H__

#include <stdint.h>
#include "os_core.h"
#include "isp_api.h"
#include "videoin.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CALIBDATA_SIZE (36472)

typedef struct _os_isp_core_param_t {
    cis_dev_driver_t *drv;              /** cis sensor driver */
    bool  enable_hdr;                   /** whether to enable HDR */
    bool  calib_file_exist;             /** whether to get calib data from file */
    union {
        const void *filename;           /** calibration filename */
        const void *data_ptr;           /** calibration data pointer if calib_file_exist is false */
    } calib_data;
    int   calib_size;                   /** calibration data size, default as CALIBDATA_SIZE */
    int   is_vertical;                  /** whether the sensor is vertically placed */
    int   acq_h_offset;                 /** horizontal offset of acq_window */
    int   acq_v_offset;                 /** vertical offset of acq_window */
    int   acq_h_size;                   /** horizontal size of acq_window */
    int   acq_v_size;                   /** vertical size of acq_window */
    bool  force_bayer_pat;              /** whether to force set bayer pattern */
    int   cis_bayer_pat;                /** the bayer pattern value to be set */
    int   awb_light_idx;
    int   enable_lsc;
    int   enable_filter;
    int   filter_dns_level;
    int   filter_sharp_level;
    float scale_factor_again_itime;
    int   use_user_setting_exposure;
    float max_itime_setting;
    int   enable_cproc;
    int   cproc_contrast;
    int   cproc_brightness;
    int   cproc_saturation;
    int   cproc_hue;
    int   tone_style;
} os_isp_core_param_t;

#define OS_ISP_PARAM_INITIALIZER            \
    ( ( (os_isp_core_param_t)               \
    {                                       \
        .drv = NULL,                        \
        .enable_hdr = 0,                    \
        .calib_file_exist = 0,              \
        .calib_data = {NULL},               \
        .calib_size = CALIBDATA_SIZE,       \
        .is_vertical  = 0,                  \
        .acq_h_offset = 0,                  \
        .acq_v_offset = 0,                  \
        .acq_h_size = 0,                    \
        .acq_v_size = 0,                    \
        .force_bayer_pat = 0,               \
        .cis_bayer_pat = CIS_BAYER_PAT_MAX, \
        .awb_light_idx = -1,                \
        .enable_lsc = 1,                    \
        .enable_filter = 1,                 \
        .filter_dns_level = 10,             \
        .filter_sharp_level = 9,            \
        .scale_factor_again_itime = 1.0,    \
        .use_user_setting_exposure = 0,     \
        .max_itime_setting = 0.12,          \
        .enable_cproc = 0,                  \
        .cproc_contrast = 128,              \
        .cproc_brightness = 0,              \
        .cproc_saturation = 128,            \
        .cproc_hue = 0,                     \
        .tone_style = 0,                    \
    }                                       \
    )                                       \
    )

typedef struct _os_isp_core_handler_t {
    isp_api_handler_t   isp_api;
    cis_dev_driver_t   *cis_drv;
} *os_isp_core_handler_t;

typedef struct _os_isp_yuv_hdr_param_t {
    int sharpen_strength;
} os_isp_yuv_hdr_param_t;

#define OS_ISP_YUV_HDR_PARAM_INITIALIZER    \
    ( ( (os_isp_yuv_hdr_param_t)            \
    {                                       \
        .sharpen_strength = 2,              \
    }                                       \
    )                                       \
    )

typedef struct _os_isp_yuv_hdr_handler_t {
    isp_api_handler_t isp_api;
} *os_isp_yuv_hdr_handler_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief  keep the scale factor and calc the cropping window size and offset
 * 
 * @param wi                    input width
 * @param hi                    input height
 * @param wo                    output width
 * @param ho                    output height
 * @param h_size                crop window width
 * @param v_size                crop window height
 * @param h_off                 crop window horizontal offset
 * @param v_off                 crop window vertical offset
 */
/* --------------------------------------------------------------------------*/
void calc_zoom_offset(float wi, float hi, float wo, float ho,
                             int *h_size, int *v_size, int *h_off, int *v_off);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_read_calib_data 
 * 
 * @param filename              input filename
 * @param calib_data            pointer to the calib data buffer
 * @param calib_size            calib data size
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int os_isp_read_calib_data(const char *filename, void *calib_data, int calib_size);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_init 
 * 
 * @param param                 param to config os isp core
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
os_isp_core_handler_t os_isp_core_init(os_isp_core_param_t *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  isp process frame buffer
 * 
 * @param context_handler       isp handler
 * @param frame_buf             input frame buffer
 * @param pp_param              frame_param_t of pp_frame
 * @param pp_frame              pp_frame
 * @param vp_param              frame_param_t of vp_frame
 * @param vp_frame              vp_frame
 * @param mp_param              frame_param_t of mp_frame
 * @param mp_frame              mp_frame
 * @param enable_hdr            whether to enable hdr
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_proc_all(os_isp_core_handler_t context_handler,
        frame_buf_t      *frame_buf,
        frame_param_t    *pp_param,
        frame_buf_t     **pp_frame,
        frame_param_t    *vp_param,
        frame_buf_t     **vp_frame,
        frame_param_t    *mp_param,
        frame_buf_t     **mp_frame,
        int               enable_hdr
        );

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_proc_frame 
 * 
 * @param context_handler       isp handler
 * @param frame_buf             input frame buffer
 * @param pp_param              frame_param_t of pp_frame
 * @param pp_frame              pp_frame
 * @param enable_hdr            whether to enable hdr
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_proc_frame(os_isp_core_handler_t context_handler,
        frame_buf_t      *frame_buf,
        frame_param_t    *pp_param,
        frame_buf_t     **pp_frame,
        int               enable_hdr
        );

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_core_uninit 
 * 
 * @param context_handler
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_core_uninit(os_isp_core_handler_t context_handler);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  init YUV HDR core engine
 * 
 * @param param             init parameter
 * 
 * @return handler of YUV HDR engine 
 */
/* --------------------------------------------------------------------------*/
os_isp_yuv_hdr_handler_t os_isp_yuv_hdr_init(os_isp_yuv_hdr_param_t *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_yuv_hdr_proc 
 * 
 * @param context_handler       isp handler
 * @param buf0                  the first frame to merge
 * @param buf1                  the second frame to merge
 * @param frmae_hdr             the output HDR merged frame
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_yuv_hdr_proc(os_isp_yuv_hdr_handler_t context_handler, 
        frame_buf_t *buf0, 
        frame_buf_t *buf1, 
        frame_buf_t **frame_hdr
        );

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_isp_yuv_hdr_uninit 
 * 
 * @param context_handler
 * 
 * @return  0 - OK, -1 - Fail 
 */
/* --------------------------------------------------------------------------*/
int os_isp_yuv_hdr_uninit(os_isp_yuv_hdr_handler_t context_handler);


#ifdef __cplusplus
}
#endif 

#endif  // __OS_ISP_CORE_H__
