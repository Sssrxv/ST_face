#ifndef __ISP_AE_CLM_H__
#define __ISP_AE_CLM_H__
 

#include "isp_types.h"
#include "isp_ae_alg.h"


#ifdef __cplusplus
extern "C"
{
#endif


int clm_execute(ae_alg_t *ae_alg, const float set_point, hist_bins_t bins, float *new_exposure);


#ifdef __cplusplus
}
#endif

#endif /* __ISP_AE_CLM_H__ */
