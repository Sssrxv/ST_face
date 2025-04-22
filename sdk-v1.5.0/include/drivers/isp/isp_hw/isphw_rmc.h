#ifndef _ISPHW_RMC_H_
#define _ISPHW_RMC_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// LSC
typedef enum _active_lsc_id_t
{
    ACTIVE_LSC_ID_ISP = 0,
    ACTIVE_LSC_ID_PRE = 1,
    ACTIVE_LSC_ID_MAX = 2,
}active_lsc_id_t;

typedef enum _lsc_ramset_id_t
{
    LSC_RAM_SET_ID_INVALID   = -1,
    LSC_RAM_SET_ID_0   = 0,
    LSC_RAM_SET_ID_1   = 1,
    LSC_RAM_SET_ID_2   = 2,
    LSC_RAM_SET_ID_3   = 3,
    LSC_RAM_SET_ID_4   = 4,
    LSC_RAM_SET_ID_5   = 5,
    LSC_RAM_SET_ID_MAX = 6,
}lsc_ramset_id_t;

int isphw_rmc_lsc_set_active_lsc(active_lsc_id_t lsc_id); // select which LSC to use
int isphw_rmc_lsc_set_hwram_id(lsc_ramset_id_t ram_id);
int isphw_rmc_lsc_get_hwram_id(lsc_ramset_id_t *ram_id);
int isphw_rmc_lsc_set_matrix(lsc_ramset_id_t ram_id, const lsc_matrix_t *matrix);

// DPC
typedef enum _dpc_ramset_id_t
{
    DPC_RAM_SET_ID_0   = 0,
    DPC_RAM_SET_ID_1   = 1,
    DPC_RAM_SET_ID_2   = 2,
    DPC_RAM_SET_ID_MAX = 3,
}dpc_ramset_id_t;

typedef struct _dpc_bad_pixel_pos_t
{
    uint32_t pos_h;
    uint32_t pos_v;
}dpc_bad_pixel_pos_t;

int isphw_rmc_dpc_set_hwram_id(dpc_ramset_id_t ram_id);
int isphw_rmc_dpc_get_hwram_id(dpc_ramset_id_t *ram_id);
int isphw_rmc_dpc_set_bptbl(dpc_ramset_id_t ram_id, const dpc_bad_pixel_pos_t *bp_tbl, uint32_t bp_num);

#ifdef __cplusplus
}
#endif

#endif

