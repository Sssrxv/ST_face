#ifndef _ISP_H_
#define _ISP_H_

#include <stdint.h>
#include "isp_context.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _isp_process_param_t
{
    isp_frame_t *bayer_frame;
    isp_frame_t *out_frame0;
    isp_frame_t *out_frame1;
    isp_frame_t *out_frame2;
    //1: enable 0: disable for out_frame0 hflip
    //NOTE: out_frame1 and out_frame2 don't support hflip
    int         hflip0; 
} isp_process_param_t;



int isp_init(isp_context_t *context, const isp_config_param_t *config_param, int hdr_idx);
int isp_release(isp_context_t *context);

int isp_process_frame(isp_context_t *context, const isp_process_param_t *process_param, isp_3a_measure_result_t *isp_3a_measure_result);
int isp_process_3a(isp_context_t *context, const isp_3a_measure_result_t *isp_3a_measure_result, isp_sensor_setting_t *new_sensor_setting);
int isp_process_hdr_3a(isp_context_t *context[3], const isp_3a_measure_result_t isp_3a_measure_result[3], int num_hdr_frames, isp_sensor_setting_t *new_sensor_setting);

int isp_set_af_window(isp_context_t *context, const isp_window_t *window);
int isp_set_ae_window(isp_context_t *context, const isp_window_t *window);
int isp_set_awb_window(isp_context_t *context, const isp_window_t *window);

int isp_set_af_start_position(isp_context_t *context, const isp_sensor_focus_t *start_position);
int isp_set_ae_start_exposure(isp_context_t *context, const isp_sensor_exp_t *start_exposure);

int isp_set_af_mode(isp_context_t *context, af_mode_t mode);
int isp_set_ae_mode(isp_context_t *context, ae_mode_t mode);
int isp_set_awb_mode(isp_context_t *context, awb_mode_t mode);

int isp_set_awb_light(isp_context_t *context, int light_idx);

int isp_set_ae_set_point(isp_context_t *context, float set_point);
int isp_set_ae_clm_tolerance(isp_context_t *context, float clm_tolerance);


int isp_lock_af(isp_context_t *context);
int isp_lock_ae(isp_context_t *context);
int isp_lock_awb(isp_context_t *context);

int isp_unlock_af(isp_context_t *context);
int isp_unlock_ae(isp_context_t *context);
int isp_unlock_awb(isp_context_t *context);

int isp_get_af_lock_state(isp_context_t *context, af_lock_state_t *state);
int isp_get_ae_lock_state(isp_context_t *context, ae_lock_state_t *state);
int isp_get_awb_lock_state(isp_context_t *context, awb_lock_state_t *state);

int isp_get_af_search_state(isp_context_t *context, af_search_state_t *state);
int isp_get_ae_search_state(isp_context_t *context, ae_search_state_t *state);
int isp_get_awb_search_state(isp_context_t *context, awb_search_state_t *state);


int isp_enable_cproc(isp_context_t *context, int enable);
int isp_set_contrast(isp_context_t *context, uint8_t contrast);
int isp_set_brightness(isp_context_t *context, uint8_t brightness);
int isp_set_saturation(isp_context_t *context, uint8_t saturation);
int isp_set_hue(isp_context_t *context, uint8_t hue);
int isp_set_gamma(isp_context_t *context, float gamma);
int isp_set_black_level(isp_context_t *context, int bl_red, int bl_green, int bl_blue);
int isp_set_wdr_curve(isp_context_t *context, const isp_wdr_curve_t *curve);

int isp_set_awb_color_scale(isp_context_t *context, float scale_r, float scale_gr, float scale_gb, float scale_b);

int isp_set_filter_level(isp_context_t *context, int enable, uint8_t denoise_level, uint8_t sharp_level);

#ifdef __cplusplus
}
#endif


#endif
