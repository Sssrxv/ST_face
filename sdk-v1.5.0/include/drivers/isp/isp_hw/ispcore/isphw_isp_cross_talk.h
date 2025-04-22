#ifndef _ISPHW_ISP_CROSS_TALK_H_
#define _ISPHW_ISP_CROSS_TALK_H_

#include "isp_types.h"
#include "isphw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif




int isphw_isp_cross_talk_set_coeff(const matrix_3x3_t *matrix);
int isphw_isp_cross_talk_set_offset(const matrix_1x3_t *matrix);





#ifdef __cplusplus
}
#endif





#endif
