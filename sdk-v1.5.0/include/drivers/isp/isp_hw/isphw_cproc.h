#ifndef _ISPHW_CPROC_H_
#define _ISPHW_CPROC_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

int isphw_cproc_enable(void);
int isphw_cproc_disable(void);
int isphw_cproc_cfg_upd(void);
int isphw_cproc_set_prop(const cproc_prop_t *prop);
int isphw_cproc_set_contrast(uint8_t contrast);
int isphw_cproc_set_brightness(uint8_t brightness);
int isphw_cproc_set_saturation(uint8_t saturation);
int isphw_cproc_set_hue(uint8_t hue);

#ifdef __cplusplus
}
#endif

#endif

