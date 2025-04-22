#ifndef _ISPHW_ISP_CNR_H_
#define _ISPHW_ISP_CNR_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int isphw_isp_cnr_enable(void);
int isphw_isp_cnr_disable(void);
int isphw_isp_cnr_set_line_size(int linesize);
int isphw_isp_cnr_set_thc1(int c1);
int isphw_isp_cnr_set_thc2(int c2);





#ifdef __cplusplus
}
#endif





#endif
