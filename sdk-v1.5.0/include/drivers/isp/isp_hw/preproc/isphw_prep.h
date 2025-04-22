#ifndef _ISPHW_PREP_H_
#define _ISPHW_PREP_H_

#include "isp_types.h"
#include "isphw_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


int isphw_prep_lsc_cfg_upd(void);

int isphw_prep_gen_cfg_upd(void);


int isphw_prep_set_intf_type(intf_type_t intf_type);
int isphw_prep_enable_input(void);
int isphw_prep_disable_input(void);



#ifdef __cplusplus
}
#endif


#endif
