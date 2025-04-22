#ifndef _ISPHW_RESIZE_H_
#define _ISPHW_RESIZE_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _resize_id_t
{
    RESIZE_ID_1   = 0,    //=>Preview Resize
    RESIZE_ID_2   = 1,    //=>Video Resize
    RESIZE_ID_3   = 2,    //=>Still Image(ZSL) Resize
    RESIZE_ID_MAX
}resize_id_t;



typedef struct _resize_factor_t
{
    uint32_t scaleHy;
    uint32_t scaleHcb;
    uint32_t scaleHcr;
    uint32_t scaleVy;
    uint32_t scaleVc;
    uint32_t coeffSumHy;
    uint32_t coeffSumHc;
    uint32_t coeffSumVy;
    uint32_t coeffSumVc;
}resize_factor_t;



typedef struct _resize_filter_t
{
    uint32_t coeff[65];
}resize_filter_t;

typedef struct _resize_ctrl_t
{
    int hyEnable;
    int hcEnable;
    int vyEnable;
    int vcEnable;
    int hyUp;
    int hcUp;
    int vyUp;
    int vcUp;
}resize_ctrl_t;



typedef struct _resize_phase_t
{
    uint32_t phaseHy;
    uint32_t phaseHc;
    uint32_t phaseVy;
    uint32_t phaseVc;
}resize_phase_t;



int isphw_resize_set_factor(resize_id_t resize_id, const resize_factor_t *factor);
int isphw_resize_set_phase(resize_id_t resize_id, const resize_phase_t *phase);
int isphw_resize_set_filter(resize_id_t resize_id, const resize_filter_t *filter);
int isphw_resize_set_ctrl(resize_id_t resize_id, const resize_ctrl_t *ctrl);
int isphw_resize_set_input_height(resize_id_t resize_id, uint32_t height);

int isphw_resize_cfg_upd(resize_id_t resize_id);




#ifdef __cplusplus
}
#endif





#endif
