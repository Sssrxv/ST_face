#ifndef _ISPHW_ISP_DEGAMMA_H_
#define _ISPHW_ISP_DEGAMMA_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int isphw_isp_degamma_enable(void);
int isphw_isp_degamma_disable(void);
int isphw_isp_degamma_set_curve(const isp_degamma_curve_t *curve);




#ifdef __cplusplus
}
#endif





#endif
