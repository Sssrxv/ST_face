#ifndef _ISP_AF_H_
#define _ISP_AF_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_calibdata.h"
#include "isp_af_alg.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _isp_af_context_t
{
    af_mode_t           mode;
    af_lock_state_t     lock_state;
    af_alg_t            af_alg;
    uint32_t            luma_shift;
    isp_window_t        region_window[AF_WINDOW_ID_MAX];
    isp_sensor_focus_t  sensor_focus;
} isp_af_context_t;


typedef struct _isp_af_process_param_t
{
    afm_result_t  afm_result;
} isp_af_process_param_t;


int isp_af_init(isp_af_context_t *context, const isp_config_param_t *config_param, int hdr_idx);
int isp_af_set_window(isp_af_context_t *context, const isp_window_t *window);
int isp_af_set_start_position(isp_af_context_t *context, const isp_sensor_focus_t *start_position);
int isp_af_set_mode(isp_af_context_t *context, af_mode_t mode);
int isp_af_lock(isp_af_context_t *context);
int isp_af_unlock(isp_af_context_t *context);
int isp_af_get_lock_state(isp_af_context_t *context, af_lock_state_t *state);
int isp_af_get_search_state(isp_af_context_t *context, af_search_state_t *state);
int isp_af_load_calibdata(isp_af_context_t *context, const isp_calibdata_t *calibdata);
int isp_af_process_frame(isp_af_context_t *context, const isp_calibdata_t *calibdata, const isp_af_process_param_t *process_param, isp_sensor_focus_t *sensor_focus);

#ifdef __cplusplus
}
#endif


#endif
