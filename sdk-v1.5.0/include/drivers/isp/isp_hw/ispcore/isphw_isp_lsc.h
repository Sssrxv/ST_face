#ifndef _ISPHW_ISP_LSC_H_
#define _ISPHW_ISP_LSC_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


int isphw_isp_lsc_enable(void);
int isphw_isp_lsc_disable(void);
int isphw_isp_lsc_set_sector(const lsc_sector_t *sector);
int isphw_isp_lsc_set_matrix(const lsc_matrix_t *matrix);
int isphw_isp_lsc_set_table_sel(uint8_t table_sel);
int isphw_isp_lsc_get_table_sel(uint8_t *table_sel);




#ifdef __cplusplus
}
#endif





#endif
