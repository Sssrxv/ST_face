#ifndef _ISPHW_ISP_WDR_H_
#define _ISPHW_ISP_WDR_H_

#include "isp_types.h"


#ifdef __cplusplus
extern "C"
{
#endif




int isphw_isp_wdr_enable(void);
int isphw_isp_wdr_disable(void);
int isphw_isp_wdr_set_rgb_offset(uint16_t rgb_offset);
int isphw_isp_wdr_set_lum_offset(uint8_t lum_offset);
int isphw_isp_wdr_set_delta_min(uint16_t delta_min_thresh, uint16_t delta_min_strength);
int isphw_isp_wdr_set_curve(const isp_wdr_curve_t *curve);






#ifdef __cplusplus
}
#endif





#endif
