#ifndef _ISP_AE_ASEM_H_
#define _ISP_AE_ASEM_H_


#include "isp_types.h"
#include "isp_ae_alg.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _cam_5x5_float_matrix_t
{
        float fCoeff[25U];              /**< array of 5x5 float values */
} cam_5x5_float_matrix_t;


int adapt_sem_execute(ae_alg_t *ae_alg, mean_luma_t luma);



#ifdef __cplusplus
}
#endif


#endif /* _ISP_AE_ASEM_H_ */
