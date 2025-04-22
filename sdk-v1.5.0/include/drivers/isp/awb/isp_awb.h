#ifndef _ISP_AWB_H_
#define _ISP_AWB_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_calibdata.h"
#include "isp_awb_alg.h"



#ifdef __cplusplus
extern "C" {
#endif


typedef struct _isp_awb_context_t
{
    awb_mode_t          mode;
    awb_lock_state_t    lock_state;
    awb_search_state_t  search_state;
    awb_alg_t           awb_alg;
    uint32_t            pre_white_cnt;
    float               stable_deviation;
    float               stable_threshold;
    awb_result_t        last_result;
    isp_window_t        region_window;

    prep_lsc_sector_t   prep_lsc_sector;
    lsc_sector_t        lsc_sector;
    int                 calib_enabled;
} isp_awb_context_t;


typedef struct _isp_awb_process_param_t
{
    awbm_result_t       awbm_result;
    hist_result_t       hist_result;
    isp_sensor_exp_t    sensor_exp;
} isp_awb_process_param_t;


int isp_awb_init(isp_awb_context_t *context, const isp_config_param_t *config_param, int hdr_idx);
int isp_awb_set_window(isp_awb_context_t *context, const isp_window_t *window);
int isp_awb_set_mode(isp_awb_context_t *context, awb_mode_t mode);
int isp_awb_set_light(isp_awb_context_t *context, int light_idx);
int isp_awb_lock(isp_awb_context_t *context);
int isp_awb_unlock(isp_awb_context_t *context);
int isp_awb_get_lock_state(isp_awb_context_t *context, awb_lock_state_t *state);
int isp_awb_get_search_state(isp_awb_context_t *context, awb_search_state_t *state);
int isp_awb_load_calibdata(isp_awb_context_t *context, const isp_calibdata_t *calibdata);
int isp_awb_process_frame(isp_awb_context_t *context, const isp_calibdata_t *calibdata, const isp_awb_process_param_t *process_param);
int isp_awb_set_color_scale(isp_awb_context_t *context, float scale_r, float scale_gr, float scale_gb, float scale_b);

#ifdef __cplusplus
}
#endif


#endif
