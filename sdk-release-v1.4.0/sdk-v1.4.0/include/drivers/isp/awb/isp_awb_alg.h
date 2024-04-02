#ifndef _ISP_AWB_ALG_H_
#define _ISP_AWB_ALG_H_
#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define MAX_IIR_FILTER_SIZE (50)


typedef enum _awb_measure_mode_t
{
    AWB_MEASURE_MODE_INVALID,
    AWB_MEASURE_MODE_YCBCR,
    AWB_MEASURE_MODE_RGB,
    AWB_MEASURE_MODE_MAX
}awb_measure_mode_t;



typedef enum _awb_num_white_pixel_eval_t
{
    AWB_NUM_WHITE_PIXEL_INVALID         = 0,        /**< initialization value */
    AWB_NUM_WHITE_PIXEL_LTMIN           = 1,        /**< less than configured minimum */
    AWB_NUM_WHITE_PIXEL_GTMAX           = 2,        /**< greater than defined maximum */
    AWB_NUM_WHITE_PIXEL_TARGET_RANGE    = 3,        /**< in min max range */
    AWB_NUM_WHITE_PIXEL_MAX
}awb_num_white_pixel_eval_t;



typedef struct _awb_component_t
{
    float fRed;
    float fGreen;
    float fBlue;
}awb_component_t;


typedef enum _illu_est_region_t
{
    ILLUEST_REGION_INVALID  = 0,    /**< Region undefined (initialization value) */
    ILLUEST_REGION_A        = 1,    /**< Region A */
    ILLUEST_REGION_B        = 2,    /**< Region B */
    ILLUEST_REGION_C        = 3,    /**< Region C */
}illu_est_region_t;



typedef struct _awb_result_t
{
    wb_gain_t       wb_gain;
    cc_matrix_t     cc_matrix;
    cc_offset_t     cc_offset;
    lsc_matrix_t    lsc_matrix;
}awb_result_t;


typedef struct _awb_alg_t
{
    int enable_damp;
    int enable_awbg;
    int enable_acc;
    int enable_alsc;
    int enable_awpr;

    const isp_calibdata_awb_t *calibdata_awb;

    /* Result */
    awb_result_t awb_result;

    /* ACC */
    float scaling_factor;

    /* Damp */
    float iir_damp_coef;

#ifdef CONFIG_ENABLE_DOOR_TYPE
    /* Exposure Prior Probaility Module */
    exp_prior_calc_evaluation_t     door_type;
    float                           iir_damp_items[MAX_IIR_FILTER_SIZE];
    uint32_t                        iir_cur_filter_idx;
    float                           exp_prior_in;             /**< exposure prior indoor probability for current frame */
    float                           exp_prior_out;            /**< exposure prior outdoor probability for current frame */
#endif 
    /* White-Point Revert Module */
    awb_component_t                 reverted_means_rgb;
    
    /* Illumination Estimation */
    awb_component_t                 normlized_means_rgb;
    int32_t                         dominate_illu_profile_idx;
    illu_est_region_t               illu_est_region;
    float                           illu_like_hood[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    float                           illu_weight[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    float                           illu_weight_trans[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];

    /* White-Balance Gain Module */
    float                           rg_proj;
    
    /* NOTE: AWB scale to adjust color temperature */
    float scale_r;
    float scale_gr;
    float scale_gb;
    float scale_b;
} awb_alg_t;

int isp_awb_alg_reset_awb_result(awb_alg_t *awb_alg);
int isp_awb_alg_reset(awb_alg_t *awb_alg);
int isp_awb_alg_set_damp_coeff(awb_alg_t *awb_alg, float damp_coeff);

int isp_awb_alg_exp_prior(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);
int isp_awb_alg_wp_revert(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);
int isp_awb_alg_wb_gain(awb_alg_t *awb_alg, const awbm_info_t *awbm_info, const wb_gain_t *prev_awb_gain);
int isp_awb_alg_illuest(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);
int isp_awb_alg_acc(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);
int isp_awb_alg_alsc(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);

int isp_awb_alg_get_acc_result(awb_alg_t *awb_alg, awb_result_t *awb_result);
int isp_awb_alg_get_alsc_result(awb_alg_t *awb_alg, awb_result_t *awb_result);
int isp_awb_alg_get_wb_gain_result(awb_alg_t *awb_alg, awb_result_t *awb_result);

int isp_awb_alg_set_wb_gain_scale(awb_alg_t *awb_alg, float scale_r, float scale_gr, float scale_gb, float scale_b);



#ifdef __cplusplus
}
#endif





#endif
