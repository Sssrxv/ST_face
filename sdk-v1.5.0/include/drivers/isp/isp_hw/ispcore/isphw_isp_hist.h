#ifndef _ISPHW_ISP_HIST_H_
#define _ISPHW_ISP_HIST_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _hist_mode_t
{
    HIST_MODE_RGB   = 0x1,
    HIST_MODE_R     = 0x2,
    HIST_MODE_G     = 0x3,
    HIST_MODE_B     = 0x4,
    HIST_MODE_Y     = 0x5,
    HIST_MODE_MAX
}hist_mode_t;


#define HIST_GRID_ITEMS    (25)
typedef struct _hist_weight_t
{
    uint8_t weight[HIST_GRID_ITEMS];
}hist_weight_t;


int isphw_isp_hist_enable(hist_mode_t mode);
int isphw_isp_hist_disable(void);
int isphw_isp_hist_set_window(const isp_window_t *window);
int isphw_isp_hist_set_weight(const hist_weight_t *hist_weight);
int isphw_isp_hist_get_result(hist_result_t *result);





#ifdef __cplusplus
}
#endif





#endif
