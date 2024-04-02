#ifndef _ISPHW_PREP_LSC_H_
#define _ISPHW_PREP_LSC_H_

#include "isp_types.h"
#include "../isphw_rmc.h"

#ifdef __cplusplus
extern "C"
{
#endif

int isphw_prep_lsc_enable(void);
int isphw_prep_lsc_disable(void);
int isphw_prep_lsc_set_sector(const prep_lsc_sector_t *sector);
int isphw_prep_lsc_set_matrix(lsc_ramset_id_t ram_id, const lsc_matrix_t *matrix);
int isphw_prep_lsc_set_window(const isp_window_t *window);
int isphw_prep_lsc_set_hwram_sel(lsc_ramset_id_t ram_id);
int isphw_prep_lsc_get_hwram_sel(lsc_ramset_id_t *ram_id);

#ifdef __cplusplus
}
#endif

#endif

