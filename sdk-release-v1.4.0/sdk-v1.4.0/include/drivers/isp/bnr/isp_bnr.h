#ifndef _ISP_BNR_H_
#define _ISP_BNR_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_calibdata.h"
#include "isphw.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _isp_bnr_setting_t
{
    bnr_config_t bnr_config;
    bnr_nf_gain_t NfGains;
    bnr_noise_level_loopup_t loopup;
    bnr_inv_strength_t strength;
} isp_bnr_setting_t;


typedef struct _isp_bnr_context_t
{
    const isp_calibdata_bnr_t *calibdata_bnr;
    isp_bnr_setting_t bnr_setting;
    int calib_enabled;
} isp_bnr_context_t;


int isp_bnr_load_calibdata(isp_bnr_context_t *context, const isp_calibdata_t *calibdata);
int isp_bnr_process_frame(isp_bnr_context_t *context, const isp_calibdata_t *calibdata, const isp_sensor_exp_t *sensor_exp);



#ifdef __cplusplus
}
#endif


#endif
