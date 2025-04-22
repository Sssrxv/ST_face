#ifndef _ISP_AE_H_
#define _ISP_AE_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_calibdata.h"
#include "isp_ae_alg.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _isp_ae_context_t
{
    ae_mode_t           mode;
    ae_lock_state_t     lock_state;
    ae_search_state_t   search_state;
    ae_alg_t            ae_alg;
    isp_window_t        exp_region;
    ae_result_t         last_result;
    int                 exposure_delay;
    int                 frame_cnt;
} isp_ae_context_t;


typedef struct _isp_ae_process_param_t
{
    exp_result_t    exp_result;
    hist_result_t   hist_result;
} isp_ae_process_param_t;


int isp_ae_init(isp_ae_context_t *context, const isp_config_param_t *config_param, int hdr_idx);
int isp_ae_set_window(isp_ae_context_t *context, const isp_window_t *window);
int isp_ae_set_start_exposure(isp_ae_context_t *context, const isp_sensor_exp_t *start_exposure);
int isp_ae_set_set_point(isp_ae_context_t *context, float set_point);
int isp_ae_set_clm_tolerance(isp_ae_context_t *context, float clm_tolerance);
int isp_ae_set_mode(isp_ae_context_t *context, ae_mode_t mode);
int isp_ae_lock(isp_ae_context_t *context);
int isp_ae_unlock(isp_ae_context_t *context);
int isp_ae_get_lock_state(isp_ae_context_t *context, ae_lock_state_t *state);
int isp_ae_get_search_state(isp_ae_context_t *context, ae_search_state_t *state);
int isp_ae_set_exposure_delay(isp_ae_context_t *context, int num_frames);
int isp_ae_load_calibdata(isp_ae_context_t *context, const isp_calibdata_t *calibdata);
int isp_ae_process_frame(isp_ae_context_t *context, const isp_calibdata_t *calibdata, const isp_ae_process_param_t *process_param, isp_sensor_exp_t *sensor_exp);
int isp_ae_get_last_exposure(isp_ae_context_t *context, isp_sensor_exp_t *sensor_exp);



#ifdef __cplusplus
}
#endif


#endif
