#ifndef __DEPTH_H__
#define __DEPTH_H__

#include "frame_mgr.h"
#include "rectify.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _depth_mode_t {
    DEPTH_MODE_STEREO = 0,
    DEPTH_MODE_MONO_L,
    DEPTH_MODE_MONO_R,
    DEPTH_MODE_MAX,
} depth_mode_t;

typedef enum _depth_fmt_t {
    DEPTH_OUT_DISP_16U = 0,
    DEPTH_OUT_DEPTH_16U,
    DEPTH_OUT_DISP_RGB565,
} depth_fmt_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief init depth engine
 * 
 * @param cam_l         left  camera param
 * @param cam_r         right camera param
 * @param Q             Q matrix param
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_init(vde_cam_param_t *cam_l, vde_cam_param_t *cam_r, vde_q_param_t *Q);

/** 
 * @brief  depth_update_calib_param 
 * 
 * @param cam_l
 * @param cam_r
 * @param Q
 * 
 * @return   
 */
int depth_update_calib_param(vde_cam_param_t *cam_l, vde_cam_param_t *cam_r, vde_q_param_t *Q);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set depth working mode, mono-camera/stereo camera
 * 
 * @param mode          DEPTH_MODE_STEREO - stereo camera depth
 *                      DEPTH_MODE_MONO_L - mono camera depth with left camera reference
 *                      DEPTH_MODE_MONO_R - mono camera depth with right camera reference
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_mode(depth_mode_t mode);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set depth output format
 * 
 * @param fmt           DEPTH_OUT_DISP_16U      - 16bit disparity
 *                      DEPTH_OUT_DEPTH_16U     - 16bit depth
 *                      DEPTH_OUT_DISP_RGB565   - 16bit RGB565 disparity
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_output_fmt(depth_fmt_t fmt);

/* --------------------------------------------------------------------------*/
/** 
 * @brief for mono-camera depth, set the reference frame
 * 
 * @param frame_ref     pointer to the reference frame
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_ref_frame(frame_buf_t *frame_ref);

/* --------------------------------------------------------------------------*/
/** 
 * @brief fill disparty to depth LUT
 * 
 * @param buffer        buffer pointer
 * @param offset        offset of the buffer to write
 * @param count         byte count
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_fill_lut(uint16_t *buffer, int offset, int count);

/* --------------------------------------------------------------------------*/
/** 
 * @brief given captured frame, output disparty/depth image of the ROI
 * 
 * @param frame_gray    captured gray8 frame
 * @param roi           region of interest
 * @param disp16u       output disparity/depth image
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_process_roi(frame_buf_t *frame_gray, frame_roi_t *roi, frame_buf_t *disp16u);

/* --------------------------------------------------------------------------*/
/** 
 * @brief given captured frame, output disparty/depth image
 * 
 * @param frame_gray    captured gray8 frame
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
frame_buf_t *depth_process(frame_buf_t *frame_gray);

/* --------------------------------------------------------------------------*/
/** 
 * @brief given captured frame, output the disparty/depth image and the rectify image
 * 
 * @param frame_gray    captured gray8 frame
 * @param frame_depth   pointer of the pointer to the output disparty/depth image, if NULL, no output
 * @param frame_rec     pointer of the pointer to the out rectified image, if NULL, no ouput
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_calc_and_rectify(frame_buf_t *frame_gray, frame_buf_t **frame_depth, frame_buf_t **frame_rec);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_release 
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_release(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief get default camera matrix
 * 
 * @param param        pointer to vde camera parameter
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_get_default_cam_param(vde_cam_param_t *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief get default Q matrix
 * 
 * @param Q           pointer to Q matrix
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_get_default_Q(vde_q_param_t *Q);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set depth algo threshold
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 *
 */
/* --------------------------------------------------------------------------*/
int depth_set_crl_th(uint32_t value);

/** 
 * @brief  depth_get_crl_th 
 * 
 * @return   
 */
uint32_t depth_get_crl_th(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set algo threshold
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_atw_ctrl(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_atw_ctrl 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_atw_ctrl(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set alog F1 param
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_algo_ctrl_f1(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_algo_ctrl_f1 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_algo_ctrl_f1(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set alog F2 param
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_algo_ctrl_f2(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_algo_ctrl_f2 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_algo_ctrl_f2(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set DPP min param
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_dpp_th_min(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_dpp_th_min 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_dpp_th_min(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief set DPP max param
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_dpp_th_max(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_dpp_th_max 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_dpp_th_max(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_cvb_desc_f 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_cvb_desc_f(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_cvb_desc_f 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_cvb_desc_f(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_cvb_gray_f 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_cvb_gray_f(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_cvb_gray_f 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_cvb_gray_f(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_cvb_h_offs 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_cvb_h_offs(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_cvb_h_offs 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_cvb_h_offs(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_saf_k_mask 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_saf_k_mask(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_saf_k_mask 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_saf_k_mask(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_saf_k_size 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_saf_k_size(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_saf_k_size 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_saf_k_size(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_saf_lamda 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_saf_lamda(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_saf_lamda 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_saf_lamda(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief depth_set_saf_sp_ref 
 * 
 * @param value
 * 
 * @return 0 - OK, -1 - FAIL  
 */
/* --------------------------------------------------------------------------*/
int depth_set_saf_sp_ref(uint32_t value);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_get_saf_sp_ref 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
uint32_t depth_get_saf_sp_ref(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  depth_set_ofe_param 
 * 
 * @param data
 * @param cnt
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int depth_set_ofe_param(uint32_t *data, int cnt);

#ifdef __cplusplus
}
#endif

#endif //__DEPTH_H__
