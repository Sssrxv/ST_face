#ifndef __ISP_AWB_ACC_H__
#define __ISP_AWB_ACC_H__
 
#include "isp_types.h"
#include "isp_calibdata.h"
#include "isp_awb_alg.h"


#define AWB_SCALING_THRESHOLD0 0.3f
#define AWB_SCALING_THRESHOLD1 0.7f
#define AWB_DAMPING_SCALING_FACTOR 0.25f

#ifdef __cplusplus
extern "C"
{
#endif


int isp_awb_acc_process_frame(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);

#ifdef __cplusplus
}
#endif

#endif /* __ISP_AWB_ACC_H__ */
