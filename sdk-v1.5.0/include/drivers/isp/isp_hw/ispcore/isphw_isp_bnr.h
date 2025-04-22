#ifndef _ISPHW_ISP_BNR_H_
#define _ISPHW_ISP_BNR_H_

#include "isp_types.h"


#ifdef __cplusplus
extern "C"
{
#endif




typedef struct _bnr_nf_gain_t
{
    uint16_t red;
    uint16_t green_r;
    uint16_t green_b;
    uint16_t blue;
}bnr_nf_gain_t;

typedef struct _bnr_inv_strength_t
{
    uint8_t weight_r;
    uint8_t weight_g;
    uint8_t weight_b;
}bnr_inv_strength_t;


typedef enum _bnr_noise_level_loopup_scale_t
{
    BNR_NOISE_LEVEL_LOOK_UP_SCALE_INVALID     = -1,
    BNR_NOISE_LEVEL_LOOK_UP_SCALE_LINEAR      = 0,
    BNR_NOISE_LEVEL_LOOK_UP_SCALE_LOGARITHMIC = 1,
    BNR_NOISE_LEVEL_LOOK_UP_SCALE_MAX
}bnr_noise_level_loopup_scale_t;

#define BNR_MAX_NLF_COEFFS  (17)
typedef struct _bnr_noise_level_loopup_t
{
    uint16_t nll_coeff[BNR_MAX_NLF_COEFFS];
    bnr_noise_level_loopup_scale_t x_scale;
}bnr_noise_level_loopup_t;


int isphw_isp_bnr_enable(void);
int isphw_isp_bnr_disable(void);

int isphw_isp_bnr_config(const bnr_config_t *config);
int isphw_isp_bnr_set_noise_function_gain(const bnr_nf_gain_t *nf_gain);
int isphw_isp_bnr_set_strength(const bnr_inv_strength_t *strength);
int isphw_isp_bnr_set_noise_level_loopup(const bnr_noise_level_loopup_t *noise_level_loopup);


int isphw_isp_bnr_set_static_demo_config_gain12(void);
int isphw_isp_bnr_set_static_demo_config_gain24(void);
int isphw_isp_bnr_set_static_demo_config_gain48(void);



#ifdef __cplusplus
}
#endif





#endif
