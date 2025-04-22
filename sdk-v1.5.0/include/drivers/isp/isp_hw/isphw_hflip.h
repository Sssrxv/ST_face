#ifndef _ISPHW_HFLIP_H_
#define _ISPHW_HFLIP_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


int isphw_hflip_set_pic_width(uint32_t pic_width);
int isphw_hflip_enable(void);
int isphw_hflip_disable(void);
int isphw_hflip_cfg_upd(void);


#ifdef __cplusplus
}
#endif





#endif
