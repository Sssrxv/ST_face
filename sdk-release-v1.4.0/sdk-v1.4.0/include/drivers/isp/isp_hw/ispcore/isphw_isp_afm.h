#ifndef _ISPHW_ISP_AFM_H_
#define _ISPHW_ISP_AFM_H_

#include "isp_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


int isphw_isp_afm_enable(void);
int isphw_isp_afm_disable(void);
int isphw_isp_afm_set_window(af_window_id_t win_id, const isp_window_t *window);
int isphw_isp_afm_set_thres(uint32_t threshold);
int isphw_isp_afm_set_varshift(uint32_t luma_shift, uint32_t afm_shift);
int isphw_isp_afm_get_result(af_window_id_t win_id, uint32_t *sum, uint32_t *lum);


uint32_t isphw_isp_afm_calc_tenengrad_shift(uint32_t max_pixel_cnt);
uint32_t isphw_isp_afm_calc_luminance_shift(uint32_t max_pixel_cnt);




#ifdef __cplusplus
}
#endif





#endif
