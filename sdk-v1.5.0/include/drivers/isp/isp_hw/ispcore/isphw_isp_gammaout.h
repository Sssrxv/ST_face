#ifndef _ISPHW_ISP_GAMMA_OUT_H_
#define _ISPHW_ISP_GAMMA_OUT_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int isphw_isp_gamma_out_enable(void);
int isphw_isp_gamma_out_disable(void);
int isphw_isp_gamma_out_set_curve(isp_gamma_seg_mode_t seg_mode, const isp_gamma_curve_t *gamma_curve);





#ifdef __cplusplus
}
#endif





#endif
