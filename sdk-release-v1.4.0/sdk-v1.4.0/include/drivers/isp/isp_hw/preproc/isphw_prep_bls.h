#ifndef _ISPHW_PREP_BLS_H_
#define _ISPHW_PREP_BLS_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int isphw_prep_bls_enable(void);
int isphw_prep_bls_disable(void);
int isphw_prep_bls_set_black_level(uint16_t bl_a, uint16_t bl_b, uint16_t bl_c, uint16_t bl_d);





#ifdef __cplusplus
}
#endif





#endif
