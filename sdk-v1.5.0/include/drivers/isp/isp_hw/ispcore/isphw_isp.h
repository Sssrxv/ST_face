#ifndef _ISPHW_ISP_H_
#define _ISPHW_ISP_H_

#include "isp_types.h"
#include "isphw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif



int isphw_isp_enable_input(void);
int isphw_isp_disable_input(void);
int isphw_isp_enable_output(void);
int isphw_isp_disable_output(void);
int isphw_isp_set_mode(intf_type_t intf_type);
int isphw_isp_gen_cfg_upd(void);
int isphw_isp_cfg_upd(void);
int isphw_isp_set_yc_range(yc_range_type_t range);




#ifdef __cplusplus
}
#endif





#endif
