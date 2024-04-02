#ifndef _ISPHW_ISP_CAC_H_
#define _ISPHW_ISP_CAC_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AIVA_CAC_H_COUNT_START_MIN (1)
#define AIVA_CAC_H_COUNT_START_MAX (ISP_CORE_ISP_CAC_COUNT_START__AIVA_CAC_H_COUNT_START_MASK >> ISP_CORE_ISP_CAC_COUNT_START__AIVA_CAC_H_COUNT_START__SHIFT)
#define AIVA_CAC_V_COUNT_START_MIN (1)
#define AIVA_CAC_V_COUNT_START_MAX (ISP_CORE_ISP_CAC_COUNT_START__AIVA_CAC_V_COUNT_START_MASK >> ISP_CORE_ISP_CAC_COUNT_START__AIVA_CAC_V_COUNT_START__SHIFT)    

    
int isphw_isp_cac_enable(void);
int isphw_isp_cac_disable(void);
int isphw_isp_cac_config(const cac_config_t *config);




#ifdef __cplusplus
}
#endif





#endif
