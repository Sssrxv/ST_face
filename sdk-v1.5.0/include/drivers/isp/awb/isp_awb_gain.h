#ifndef __ISP_AWB_GAIN_H__
#define __ISP_AWB_GAIN_H__
 
#include "isp_types.h"
#include "isp_calibdata.h"
#include "isp_awb_alg.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _wb_gain_over_g_t
{
    float gain_r_over_g;                           /**< (Gain-Red / Gain-Green) */
    float gain_b_over_g;                           /**< (Gain-Blue / Gain-Green) */
} wb_gain_over_g_t;

int isp_awb_wb_gain_process_frame(awb_alg_t *awb_alg, const awbm_info_t *awbm_info, const wb_gain_t *prev_awb_gain);



#ifdef __cplusplus
}
#endif


#endif /* __ISP_AWB_GAIN_H__ */
