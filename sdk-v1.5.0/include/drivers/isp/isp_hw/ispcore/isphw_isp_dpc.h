#ifndef _ISPHW_ISP_DPC_H_
#define _ISPHW_ISP_DPC_H_

#include "isp_types.h"
#include "../isphw_rmc.h"

#ifdef __cplusplus
extern "C"
{
#endif

int isphw_isp_dpc_enable(void);
int isphw_isp_dpc_disable(void);

int isphw_isp_dpc_set_static_config(const dpc_static_config_t *config);
int isphw_isp_dpc_set_hwram_sel(dpc_ramset_id_t ram_id);
int isphw_isp_dpc_get_hwram_sel(dpc_ramset_id_t *ram_id);
int isphw_isp_dpc_set_bptbl(dpc_ramset_id_t ram_id, const dpc_bad_pixel_pos_t *bp_tbl, uint32_t bp_num);

int isphw_isp_dpc_set_static_demo_config_gain12(void);
int isphw_isp_dpc_set_static_demo_config_gain24(void);
int isphw_isp_dpc_set_static_demo_config_gain48(void);

#ifdef __cplusplus
}
#endif





#endif
