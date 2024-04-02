#ifndef _ISPHW_ISP_AWBM_H_
#define _ISPHW_ISP_AWBM_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _awbm_config_t
{
    uint8_t maxY;
    uint8_t refCr_maxR;
    uint8_t minY_maxG;
    uint8_t refCb_maxB;
    uint8_t maxCSum;
    uint8_t minC;
}awbm_config_t;


typedef enum _awbm_mode_t
{
    AWBM_MODE_YCBCR,
    AWBM_MODE_RGB,
    AWBM_MODE_MAX
}awbm_mode_t;


int isphw_isp_awbm_enable(void);
int isphw_isp_awbm_disable(void);
int isphw_isp_awbm_set_window(const isp_window_t *window);
int isphw_isp_awbm_set_frames(uint32_t frames);
int isphw_isp_awbm_set_mode(awbm_mode_t mode, const awbm_config_t *config);
int isphw_isp_awbm_get_result(awbm_result_t *result);





#ifdef __cplusplus
}
#endif





#endif
