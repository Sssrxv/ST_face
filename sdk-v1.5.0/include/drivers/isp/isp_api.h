#ifndef _ISP_API_H_
#define _ISP_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _isp_window_t
{
    int h_offset;
    int v_offset;
    int h_size;
    int v_size;
} isp_window_t;

typedef enum _isp_bayer_pat_t {
    ISP_BAYER_PAT_RGGB = 0x0,
    ISP_BAYER_PAT_GRBG = 0x1,
    ISP_BAYER_PAT_GBRG = 0x2,
    ISP_BAYER_PAT_BGGR = 0x3,
    ISP_BAYER_PAT_MAX,
} isp_bayer_pat_t;

typedef struct _isp_sensor_exp_param_t
{
    float min_again[3]; //for max 3 hdr exposure setting
    float max_again[3];
    float min_again_step[3];

    float min_dgain[3];
    float max_dgain[3];
    float min_dgain_step[3];

    float min_itime[3];
    float max_itime[3];
    float min_itime_step[3];
} isp_sensor_exp_param_t;

typedef struct _isp_sensor_focus_param_t
{
    float min_focus_pos;
    float max_focus_pos;
    float min_focus_step;
} isp_sensor_focus_param_t;


typedef struct _isp_sensor_param_t
{
    isp_sensor_exp_param_t      exp_param; 
    isp_sensor_focus_param_t    focus_param;
    isp_bayer_pat_t bayer_pat;
    int has_auto_focus;
    float frame_rate;
} isp_sensor_param_t;


typedef struct _isp_sensor_exp_t
{
    float again;
    float dgain;
    float itime;
} isp_sensor_exp_t;

typedef struct _isp_sensor_hdr_exp_t
{
    int   num_hdr_frames;
    float again[3];
    float dgain[3];
    float itime[3];
} isp_sensor_hdr_exp_t;


typedef struct _isp_sensor_focus_t
{
    float focus_pos;
} isp_sensor_focus_t;

typedef struct _isp_sensor_setting_t
{
    isp_sensor_exp_t        sensor_exp;
    isp_sensor_hdr_exp_t    sensor_hdr_exp;
    isp_sensor_focus_t      sensor_focus;
} isp_sensor_setting_t;

typedef struct _isp_user_setting_interface_t
{
    int     lsc_enabled;
    float   scale_factor_again_itime;
} isp_user_setting_interface_t;

typedef struct _awbm_result_t
{
    uint32_t  white_cnt;
    uint8_t   mean_y_g;
    uint8_t   mean_cb_b;
    uint8_t   mean_cr_r;
} awbm_result_t;


#define HIST_NUM_BINS (16)
typedef struct _hist_result_t
{
    uint32_t bins[HIST_NUM_BINS];
} hist_result_t;


typedef struct _exp_result_t
{
    uint8_t luma[25];
} exp_result_t;


typedef struct _afm_result_t
{
    uint32_t sharpness_a;
    uint32_t sharpness_b;
    uint32_t sharpness_c;

    uint32_t luminance_a;
    uint32_t luminance_b;
    uint32_t luminance_c;

    uint32_t pixel_cnt_a;
    uint32_t pixel_cnt_b;
    uint32_t pixel_cnt_c;

    uint32_t max_pixel_cnt;

    uint32_t luma_shift;
    uint32_t afm_shift;
} afm_result_t;



typedef struct _isp_3a_measure_result_t
{
    isp_sensor_exp_t    sensor_exp;
    isp_sensor_focus_t  sensor_focus;
    awbm_result_t       awbm_result;
    hist_result_t       hist_result;
    exp_result_t        exp_result;
    afm_result_t        afm_result;
} isp_3a_measure_result_t;


typedef enum _isp_mode_t
{
    ISP_MODE_NORMAL,
    ISP_MODE_YUV_HDR,
    ISP_MODE_BAYER_HDR
} isp_mode_t;

typedef enum _isp_color_temperature_t_ {
    ISP_COLOR_TEMP_NORMAL = 0,
    ISP_COLOR_TEMP_COLD,
    ISP_COLOR_TEMP_WARM,
    ISP_COLOR_TEMP_LIGHT_COLD,
    ISP_COLOR_TEMP_LIGHT_WARM,
} isp_color_temperature_t;


typedef struct _isp_config_param_t
{
    isp_window_t        acq_window;
    isp_window_t        out_window;
    isp_sensor_param_t  sensor_param;
    isp_user_setting_interface_t user_setting;
    void*               calibdata;
    int                 calibdata_size;
} isp_config_param_t;


typedef struct _isp_hdr_lut_t
{
    uint8_t sigm_lut0[256];
    uint8_t sigm_lut1[256];
    uint8_t wgt_lut0[256];
    uint8_t wgt_lut1[256];
} isp_hdr_lut_t;


typedef struct _isp_hdr_config_param_t
{
    isp_hdr_lut_t hdr_lut;
    int sharpen_strength;
} isp_hdr_config_param_t;


typedef enum _isp_image_fmt_t
{
    ISP_IMAGE_FMT_NV12,
    ISP_IMAGE_FMT_NV21,
    ISP_IMAGE_FMT_I420,
    ISP_IMAGE_FMT_YV12,
    ISP_IMAGE_FMT_YUV422INTERLEAVE,
    ISP_IMAGE_FMT_YUV422SEMIPLANAR,
    ISP_IMAGE_FMT_YUV422PLANAR,
    ISP_IMAGE_FMT_RGBBAYER8,
    ISP_IMAGE_FMT_RGBBAYER10,
    ISP_IMAGE_FMT_RGBBAYER12
} isp_image_fmt_t;


typedef struct _isp_image_buf_t
{
    int      chnl0_width;
    int      chnl0_height;
    int      chnl0_stride;
    uint32_t chnl0_addr;

    int      chnl1_width;
    int      chnl1_height;
    int      chnl1_stride;
    uint32_t chnl1_addr;

    int      chnl2_width;
    int      chnl2_height;
    int      chnl2_stride;
    uint32_t chnl2_addr;
} isp_image_buf_t;


typedef struct _isp_frame_t
{
    isp_image_fmt_t image_fmt;
    isp_image_buf_t image_buf;
    isp_window_t    zoom_window;
} isp_frame_t;


#define ISP_WDR_DX_SIZE (33)
#define ISP_WDR_DY_SIZE (33)
typedef struct _isp_wdr_curve_t
{
    uint16_t dx[ISP_WDR_DX_SIZE];
    uint16_t dy[ISP_WDR_DY_SIZE];
} isp_wdr_curve_t;



typedef enum _af_mode_t
{
    AF_MODE_INVALID,
    AF_MODE_MANUAL,
    AF_MODE_ONE_SHOT,
    AF_MODE_CONTINUOUS_PICTURE,
    AF_MODE_CONTINUOUS_VIDEO,
    AF_MODE_MAX
} af_mode_t;

typedef enum _ae_flicker_type_t
{
    AE_FLICKER_TYPE_INVALID,
    AE_FLICKER_TYPE_OFF,
    AE_FLICKER_TYPE_50HZ,
    AE_FLICKER_TYPE_60HZ,
    AE_FLICKER_TYPE_MAX
} ae_flicker_type_t;


typedef enum _ae_mode_t
{
    AE_MODE_INVALID,
    AE_MODE_MANUAL,
    AE_MODE_AUTO,
    AE_MODE_MAX
} ae_mode_t;


typedef enum _awb_mode_t
{
    AWB_MODE_INVALID,
    AWB_MODE_MANUAL,
    AWB_MODE_AUTO,
    AWB_MODE_MAX
} awb_mode_t;


typedef enum _ae_lock_state_t
{
    AE_LOCK_STATE_INVALID,
    AE_LOCK_STATE_LOCKED,
    AE_LOCK_STATE_UNLOCKED,
    AE_LOCK_STATE_MAX
} ae_lock_state_t;

typedef enum _af_lock_state_t
{
    AF_LOCK_STATE_INVALID,
    AF_LOCK_STATE_LOCKED,
    AF_LOCK_STATE_UNLOCKED,
    AF_LOCK_STATE_MAX
} af_lock_state_t;


typedef enum _awb_lock_state_t
{
    AWB_LOCK_STATE_INVALID,
    AWB_LOCK_STATE_LOCKED,
    AWB_LOCK_STATE_UNLOCKED,
    AWB_LOCK_STATE_MAX
} awb_lock_state_t;


typedef enum _awb_search_state_t
{
    AWB_SEARCH_STATE_INVALID,
    AWB_SEARCH_STATE_SEARCHING,
    AWB_SEARCH_STATE_CONVERGED,
    AWB_SEARCH_STATE_MAX
} awb_search_state_t;


typedef enum _ae_search_state_t
{
    AE_SEARCH_STATE_INVALID,
    AE_SEARCH_STATE_SEARCHING,
    AE_SEARCH_STATE_CONVERGED,
    AE_SEARCH_STATE_MAX
} ae_search_state_t;



typedef enum _af_search_state_t
{
    AF_SEARCH_STATE_INVALID,
    AF_SEARCH_STATE_INIT,
    AF_SEARCH_STATE_SEARCHING,
    AF_SEARCH_STATE_CONVERGED,
    AF_SEARCH_STATE_FAILED,
    AF_SEARCH_STATE_MAX
} af_search_state_t;


typedef struct _isp_api_process_param_t
{
    isp_frame_t *in_frames[3];
    isp_frame_t *out_frames[3];
    //1: enable 0: disable for out_frame0 hflip
    //NOTE: out_frame[1] and out_frame[2] don't support hflip
    int         hflip0; 
} isp_api_process_param_t;

typedef struct isp_api_context_t *isp_api_handler_t;


isp_api_handler_t isp_api_create_context(isp_mode_t mode, int num_hdr_frames);
int isp_api_destroy_context(isp_api_handler_t handler);
int isp_api_init(isp_api_handler_t handler, isp_mode_t mode, const isp_config_param_t *config_param, const isp_hdr_config_param_t *hdr_config_param);
int isp_api_process_frame(isp_api_handler_t handler, const isp_api_process_param_t *process_param, isp_sensor_setting_t *new_sensor_setting);

int isp_api_set_out_window(isp_api_handler_t handler, const isp_window_t *window);


int isp_api_set_af_window(isp_api_handler_t handler, const isp_window_t *window);
int isp_api_set_ae_window(isp_api_handler_t handler, const isp_window_t *window);
int isp_api_set_awb_window(isp_api_handler_t handler, const isp_window_t *window);

int isp_api_set_af_start_position(isp_api_handler_t handler, const isp_sensor_focus_t *start_position);
int isp_api_set_ae_start_exposure(isp_api_handler_t handler, const isp_sensor_exp_t *start_exposure);

int isp_api_set_af_mode(isp_api_handler_t handler, af_mode_t mode);
int isp_api_set_ae_mode(isp_api_handler_t handler, ae_mode_t mode);
int isp_api_set_awb_mode(isp_api_handler_t handler, awb_mode_t mode);

int isp_api_set_awb_light(isp_api_handler_t handler, int light_idx);

int isp_api_set_ae_set_point(isp_api_handler_t handler, float set_point);
int isp_api_set_ae_clm_tolerance(isp_api_handler_t handler, float clm_tolerance);

int isp_api_lock_af(isp_api_handler_t handler);
int isp_api_lock_ae(isp_api_handler_t handler);
int isp_api_lock_awb(isp_api_handler_t handler);

int isp_api_unlock_af(isp_api_handler_t handler);
int isp_api_unlock_ae(isp_api_handler_t handler);
int isp_api_unlock_awb(isp_api_handler_t handler);

int isp_api_get_af_lock_state(isp_api_handler_t handler, af_lock_state_t *state);
int isp_api_get_ae_lock_state(isp_api_handler_t handler, ae_lock_state_t *state);
int isp_api_get_awb_lock_state(isp_api_handler_t handler, awb_lock_state_t *state);

int isp_api_get_af_search_state(isp_api_handler_t handler, af_search_state_t *state);
int isp_api_get_ae_search_state(isp_api_handler_t handler, ae_search_state_t *state);
int isp_api_get_awb_search_state(isp_api_handler_t handler, awb_search_state_t *state);

int isp_api_enable_cproc(isp_api_handler_t handler, int enable);
int isp_api_set_contrast(isp_api_handler_t handler, uint8_t contrast);
int isp_api_set_brightness(isp_api_handler_t handler, uint8_t brightness);
int isp_api_set_saturation(isp_api_handler_t handler, uint8_t saturation);
int isp_api_set_hue(isp_api_handler_t handler, uint8_t hue);
int isp_api_set_gamma(isp_api_handler_t handler, float gamma);
int isp_api_set_black_level(isp_api_handler_t handler, int bl_red, int bl_green, int bl_blue);
int isp_api_set_wdr_curve(isp_api_handler_t handler, const isp_wdr_curve_t *curve);

int isp_api_set_color_temperature(isp_api_handler_t, isp_color_temperature_t t);

int isp_api_set_filter(isp_api_handler_t, int enable, uint8_t denoise_level, uint8_t sharp_level);

#ifdef __cplusplus
}
#endif


#endif
