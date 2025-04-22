#ifndef __ISP_ALSC_H__
#define __ISP_ALSC_H__

#include "isp_types.h"
#include "isp_awb_alg.h"


#ifdef __cplusplus
extern "C"
{
#endif


int isp_awb_alsc_process_frame(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);


#ifdef __cplusplus
}
#endif

#endif /* __ISP_ALSC_H__ */
