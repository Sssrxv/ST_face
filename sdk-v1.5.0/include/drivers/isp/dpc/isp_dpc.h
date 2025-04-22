#ifndef _ISP_DPC_H_
#define _ISP_DPC_H_

#include <stdint.h>
#include "isp_api.h"
#include "isp_calibdata.h"
#include "isphw.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct _isp_dpc_setting_t
{
    dpc_static_config_t dpc_config;
} isp_dpc_setting_t;


typedef struct _isp_dpc_context_t
{
    isp_dpc_setting_t dpc_setting;
} isp_dpc_context_t;


int isp_dpc_load_calibdata(isp_dpc_context_t *context, const isp_calibdata_t *calibdata);
int isp_dpc_process_frame(isp_dpc_context_t *context, const isp_calibdata_t *calibdata, const isp_sensor_exp_t *sensor_exp);




#ifdef __cplusplus
}
#endif


#endif
