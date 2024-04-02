#ifndef _ISP_CALIBDATA_H_
#define _ISP_CALIBDATA_H_

#include <stdint.h>
#include "isp_base_types.h"

//These macros define the maximum numbers for storing
//a kind of tuning data. They can be reconfigured if the
//number is not enough to get a good image quality after
//getting consensus between firmawre/tuning/driver team.
#define ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES    (7)
#define ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS     (16)
#define ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS     (6)
#define ISP_CALIBDATA_CONFIG_AWB_MAX_SATS             (4)
#define ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS             (2)


//These macros define the size of a kind array. They
//are mapped to hardware and only should be changed when
//the hardware changed.
#define ISP_CALIBDATA_DEGAMMA_CURVE_SIZE          (17)
#define ISP_CALIBDATA_GAMMA_CURVE_SIZE            (17)
#define ISP_CALIBDATA_WDR_CURVE_SIZE              (33)
#define ISP_CALIBDATA_ISP_FILTER_DENOISE_SIZE     (7)
#define ISP_CALIBDATA_ISP_FILTER_SHARP_SIZE       (5)
#define ISP_CALIBDATA_SNR_SIGMA_COEFFS_SIZE       (17)
#define ISP_CALIBDATA_SNR_CALC_COEFFS_SIZE        (17)


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_1x1_float_matrix_t
{
    float fCoeff[1];
}isp_calibdata_1x1_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 | 2 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_1x3_float_matrix_t
{
    float fCoeff[3];
}isp_calibdata_1x3_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 | ... | 4 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_1x4_float_matrix_t
{
    float fCoeff[4];
}isp_calibdata_1x4_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_2x1_float_matrix_t
{
    float fCoeff[2];
}isp_calibdata_2x1_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 |
 *          | 2 | 3 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_2x2_float_matrix_t
{
    float fCoeff[4];
}isp_calibdata_2x2_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 |  2 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_3x1_float_matrix_t
{
    float fCoeff[3];
}isp_calibdata_3x1_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 |  2 |
 *          | 3 | 4 |  5 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_3x2_float_matrix_t
{
    float fCoeff[6];
}isp_calibdata_3x2_float_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 |  2 |
 *          | 3 | 4 |  5 |
 *          | 6 | 7 |  8 |
 *
 * @note    Coefficients are represented as float numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_3x3_float_matrix_t
{
    float fCoeff[9];
}isp_calibdata_3x3_float_matrix_t;



/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 | 2 | ... | 4 |
 *
 * @note    Coefficients are represented as short numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_1x4_ushort_matrix_t
{
    uint16_t uCoeff[4];
}isp_calibdata_1x4_ushort_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          | 0 | 1 | 2 | ... | 16 |
 *
 * @note    Coefficients are represented as short numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_1x17_ushort_matrix_t
{
    uint16_t uCoeff[17];
}isp_calibdata_1x17_ushort_matrix_t;


/*****************************************************************************/
/**
 * @brief   Matrix coefficients
 *
 *          |   0 |   1 |   2 |   3 |   4 |   5 |   6 |   7 | ....
 *          |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  24 | ....
 *          |  34 |  35 |  36 |  37 |  38 |  39 |  40 |  41 | ....
 *          ...
 *          ...
 *          ...
 *          | 271 | 272 | 273 | 274 | 275 | 276 | 277 | 278 | .... | 288 |
 *
 * @note    Coefficients are represented as short numbers
 */
/*****************************************************************************/
typedef struct _isp_calibdata_17x17_ushort_matrix_t
{
    uint16_t uCoeff[17 * 17];
}isp_calibdata_17x17_ushort_matrix_t;




typedef struct _isp_calibdata_awb_clip_param_t
{
    float   rg1[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   max_dist1[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   rg2[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   max_dist2[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
}isp_calibdata_awb_clip_param_t;


typedef struct _isp_calibdata_awb_global_fade_param_t
{
    float   global_fade1[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   global_gain_distance1[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   global_fade2[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
    float   global_gain_distance2[ISP_CALIBDATA_CONFIG_AWB_MAX_CURVE_POINTS];
}isp_calibdata_awb_global_fade_param_t;


typedef struct _isp_calibdata_awb_fade2param_t
{
    float    fadef[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    cb_min_region_max[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    cr_min_region_max[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    max_csum_region_max[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    cb_min_region_min[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    cr_min_region_min[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
    float    max_csum_region_min[ISP_CALIBDATA_CONFIG_AWB_MAX_FADE2_POINTS];
}isp_calibdata_awb_fade2param_t;


typedef struct _isp_calibdata_awb_iir_t
{
    float    iir_damp_coef_add;                        /**< incrementer of damping coefficient */
    float    iir_damp_coef_sub;                        /**< decrementer of damping coefficient */
    float    iir_damp_filter_threshold;                /**< threshold for incrementing or decrementing of damping coefficient */

    float    iir_damping_coef_min;                     /**< minmuim value of damping coefficient */
    float    iir_damping_coef_max;                     /**< maximum value of damping coefficient */
    float    iir_damping_coef_init;                    /**< initial value of damping coefficient */

    uint16_t iir_filter_size;                         /**< number of filter items */
    float    iir_filter_init_value;                    /**< initial value of the filter items */
}isp_calibdata_awb_iir_t;


typedef struct _isp_calibdata_center_line_t
{
    float f_n0_rg;                                  /**< Rg component of normal vector */
    float f_n0_bg;                                  /**< Bg component of normal vector */
    float f_d;                                      /**< Distance of normal vector     */
} isp_calibdata_center_line_t;


typedef struct _isp_calibdata_awb_global_t
{
    isp_calibdata_3x1_float_matrix_t        svd_mean_value;
    isp_calibdata_3x2_float_matrix_t        pca_matrix;
    isp_calibdata_center_line_t             center_line;

    isp_calibdata_awb_clip_param_t          awb_clip_param;               /**< clipping parameter in Rg/Bg space */
    isp_calibdata_awb_global_fade_param_t   awb_global_fade_param;
    isp_calibdata_awb_fade2param_t          awb_fade2param;

    float                                   rg_proj_indoor_min;
    float                                   rg_proj_outdoor_min;
    float                                   rg_proj_max;
    float                                   rg_proj_max_sky;

    uint8_t                                 outdoor_clip_illu;  //The index of the outdoor illumination.

    float                                   region_size;
    float                                   region_size_inc;
    float                                   region_size_dec;

    isp_calibdata_awb_iir_t                 iir;
}isp_calibdata_awb_global_t;

enum  isp_calibdata_awb_door_enum_type_t
{
    ISP_CALIBDATA_AWB_DOOR_TYPE_OUTDOOR = 0,
    ISP_CALIBDATA_AWB_DOOR_TYPE_INDOOR  = 1
};
typedef int32_t isp_calibdata_awb_door_type_t;




/* gaussian mixture modell */
typedef struct _isp_calibdata_gmm_t
{
    isp_calibdata_2x1_float_matrix_t    gauss_mean_value;
    isp_calibdata_2x2_float_matrix_t    covariance_matrix;
    isp_calibdata_1x1_float_matrix_t    gauss_factor;
    isp_calibdata_2x1_float_matrix_t    threshold;
}isp_calibdata_gmm_t;



typedef struct _isp_calibdata_awb_gain_t
{
    isp_calibdata_1x4_float_matrix_t    component_gain;     /**< White Balance Gains*/
}isp_calibdata_awb_gain_t;




typedef struct _isp_calibdata_awb_cc_t
{
    isp_calibdata_3x3_float_matrix_t    cross_talk_coeff;    /**< CrossTalk matrix coefficients */
    isp_calibdata_1x3_float_matrix_t    cross_talk_offset;   /**< CrossTalk offsets */
}isp_calibdata_awb_cc_t;


enum isp_calibdata_4ch_color_component_enum_t
{
    ISP_CALIBDATA_4CH_COLOR_COMPONENT_RED     = 0,
    ISP_CALIBDATA_4CH_COLOR_COMPONENT_GREENR  = 1,
    ISP_CALIBDATA_4CH_COLOR_COMPONENT_GREENB  = 2,
    ISP_CALIBDATA_4CH_COLOR_COMPONENT_BLUE    = 3,
    ISP_CALIBDATA_4CH_COLOR_COMPONENT_MAX
};
typedef int32_t isp_calibdata_4ch_color_component_t;

typedef struct _isp_calibdata_awb_lsc_t
{
    isp_calibdata_17x17_ushort_matrix_t  lsc_matrix[ISP_CALIBDATA_4CH_COLOR_COMPONENT_MAX];
} isp_calibdata_awb_lsc_t;


typedef struct _isp_calibdata_awb_lsc_global_t
{
    uint16_t    lsc_xgrad_tbl[8];
    uint16_t    lsc_ygrad_tbl[8];
    uint16_t    lsc_xsize_tbl[16];
    uint16_t    lsc_ysize_tbl[16];
}isp_calibdata_awb_lsc_global_t;


typedef struct _isp_calibdata_awb_sensorgain2sat_t
{
    float   sensor_gain[ISP_CALIBDATA_CONFIG_AWB_MAX_SATS];
    float   saturation[ISP_CALIBDATA_CONFIG_AWB_MAX_SATS];
}isp_calibdata_awb_sensorgain2sat_t;

typedef struct _isp_calibdata_awb_sensorgain2vig_t
{
    float   sensor_gain[ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS];
    float   vignetting[ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS];
}isp_calibdata_awb_sensorgain2vig_t;

typedef struct _isp_calibdata_awb_sat2cc_t
{
    uint8_t                     sat_num; //0~ISP_CALIBDATA_CONFIG_AWB_MAX_SATS
    float                       saturation[ISP_CALIBDATA_CONFIG_AWB_MAX_SATS]; //in decending order.
    isp_calibdata_awb_cc_t      sat2cc[ISP_CALIBDATA_CONFIG_AWB_MAX_SATS];     //in decending order.
}isp_calibdata_awb_sat2cc_t;

typedef struct _isp_calibdata_awb_vig2lsc_t
{
    uint8_t                     vig_num; //0 ~ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS
    float                       vignetting[ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS]; //in decending order.
    isp_calibdata_awb_lsc_t     vig2lsc[ISP_CALIBDATA_CONFIG_AWB_MAX_VIGS];    //in decending order.
}isp_calibdata_awb_vig2lsc_t;



typedef struct _isp_calibdata_ae_t
{
    float   set_point;        /**< set point to hit by the ae control system */
    float   clm_tolerance;
    float   damp_over_still;   /**< damping coefficient for still image mode */
    float   damp_under_still;  /**< damping coefficient for still image mode */
    float   damp_over_video;   /**< damping coefficient for video mode */
    float   damp_under_video;  /**< damping coefficient for video mode */
}isp_calibdata_ae_t;


typedef struct _isp_calibdata_bls_t
{
    isp_calibdata_1x4_ushort_matrix_t level; /**< black level for all 4 color components */
}isp_calibdata_bls_t;



typedef struct _isp_calibdata_degamma_t
{
    uint8_t     segment[ISP_CALIBDATA_DEGAMMA_CURVE_SIZE-1];  /**< x_i segment size */
    uint16_t    red[ISP_CALIBDATA_DEGAMMA_CURVE_SIZE];        /**< red point */
    uint16_t    green[ISP_CALIBDATA_DEGAMMA_CURVE_SIZE];      /**< green point */
    uint16_t    blue[ISP_CALIBDATA_DEGAMMA_CURVE_SIZE];       /**< blue point */
}isp_calibdata_degamma_t;


enum isp_calibdata_gamma_curve_enum_type_t
{
    ISP_CALIBDATA_GAMMA_CURVE_TYPE_LOG  = 0,
    ISP_CALIBDATA_GAMMA_CURVE_TYPE_EQU  = 1
};
typedef int32_t isp_calibdata_gamma_curve_type_t;

typedef struct _isp_calibdata_gamma_t
{
    isp_calibdata_gamma_curve_type_t    curve_type;
    uint16_t                            gamma_y[ISP_CALIBDATA_GAMMA_CURVE_SIZE];
}isp_calibdata_gamma_t;

typedef struct _isp_calibdata_wdr_t
{
    uint16_t x_val[ISP_CALIBDATA_WDR_CURVE_SIZE];
    uint16_t y_val[ISP_CALIBDATA_WDR_CURVE_SIZE];
}isp_calibdata_wdr_t;

typedef struct _isp_calibdata_ispfilter_t
{
    uint16_t denoise_filter[ISP_CALIBDATA_ISP_FILTER_DENOISE_SIZE];
    //denoise_filter[0] => STAGE1_SELECT
    //denoise_filter[1] => FILT_H_MODE
    //denoise_filter[2] => FILT_V_MODE
    //denoise_filter[3] => ISP_FILTER_THRESH_SH0
    //denoise_filter[4] => ISP_FILTER_THRESH_SH1
    //denoise_filter[5] => ISP_FILTER_THRESH_BL0
    //denoise_filter[6] => ISP_FILTER_THRESH_BL1

    uint8_t  sharp_filter[ISP_CALIBDATA_ISP_FILTER_SHARP_SIZE];
    //sharp_filter[0] => ISP_FILTER_FAC_SH0
    //sharp_filter[1] => ISP_FILTER_FAC_SH1
    //sharp_filter[2] => ISP_FILTER_FAC_MID
    //sharp_filter[3] => ISP_FILTER_FAC_BL0
    //sharp_filter[4] => ISP_FILTER_FAC_BL1
}isp_calibdata_ispfilter_t;

typedef struct _isp_calibdata_cac_t
{
    uint8_t                             x_norm_shift;
    uint8_t                             x_norm_factor;
    uint8_t                             y_norm_shift;
    uint8_t                             y_norm_factor;

    isp_calibdata_1x3_float_matrix_t    red;                /**< coeffciencts A, B and C for red */
    isp_calibdata_1x3_float_matrix_t    blue;               /**< coeffciencts A, B and C for blue */

    int16_t                             x_offset;
    int16_t                             y_offset;
}isp_calibdata_cac_t;


typedef struct _isp_calibdata_resolution_t
{
    uint32_t width; 
    uint32_t height;
}isp_calibdata_resolution_t;



enum isp_calibdata_bnr_nll_segmentation_enum_t
{
    ISP_CALIBDATA_BNR_NLL_SEGMENTATION_EQU = 0,
    ISP_CALIBDATA_BNR_NLL_SEGMENTATION_LOG = 1
};
typedef int32_t isp_calibdata_bnr_nll_segmentation_t;

typedef struct _isp_calibdata_bnr_t
{
    isp_calibdata_bnr_nll_segmentation_t    nll_segmentation;
    isp_calibdata_1x17_ushort_matrix_t      nll_coeff;

    uint16_t                                sigma_green;
    uint16_t                                sigma_red_blue;
    float                                   gradient;
    float                                   offset;
    isp_calibdata_1x4_float_matrix_t        nf_gains;
}isp_calibdata_bnr_t;



typedef struct _isp_calibdata_dpc_t
{
    uint32_t   isp_dpc_mode;
    uint32_t   isp_dpc_output_mode;
    uint32_t   isp_dpc_set_use;
    uint32_t   isp_dpc_methods_set_1;
    uint32_t   isp_dpc_methods_set_2;
    uint32_t   isp_dpc_methods_set_3;
    uint32_t   isp_dpc_line_thresh_1;
    uint32_t   isp_dpc_line_mad_fac_1;
    uint32_t   isp_dpc_pg_fac_1;
    uint32_t   isp_dpc_rnd_thresh_1;
    uint32_t   isp_dpc_rg_fac_1;
    uint32_t   isp_dpc_line_thresh_2;
    uint32_t   isp_dpc_line_mad_fac_2;
    uint32_t   isp_dpc_pg_fac_2;
    uint32_t   isp_dpc_rnd_thresh_2;
    uint32_t   isp_dpc_rg_fac_2;
    uint32_t   isp_dpc_line_thresh_3;
    uint32_t   isp_dpc_line_mad_fac_3;
    uint32_t   isp_dpc_pg_fac_3;
    uint32_t   isp_dpc_rnd_thresh_3;
    uint32_t   isp_dpc_rg_fac_3;
    uint32_t   isp_dpc_ro_limits;
    uint32_t   isp_dpc_rnd_offs;
}isp_calibdata_dpc_t;


enum isp_calibdata_snr_lut_seg_mode_enum_t
{
    ISP_CALIBDATA_SNR_LUT_SEG_MODE_LOG = 0,
    ISP_CALIBDATA_SNR_LUT_SEG_MODE_EQU = 1
};
typedef int32_t isp_calibdata_snr_lut_seg_mode_t;

typedef struct _isp_calibdata_stnr_t
{
    uint8_t             gamma;
    //k, b are used to calculate the strength using the fomulat:
    //strength = sqrt((k * Gain) + b);
    float               k;
    float               b;
    uint8_t             sigmaCoeff[ISP_CALIBDATA_SNR_SIGMA_COEFFS_SIZE];
    uint8_t             fupThrh;
    uint8_t             fupFacTh;
    uint8_t             fupFacTl;
    uint8_t             fudThrh;
    uint8_t             fudFacTh;
    uint8_t             fudFacTl;
}isp_calibdata_stnr_t;


typedef struct _isp_calibdata_awb_t
{
    uint32_t                            illu_num;//The illuminations contained for AWB. Should be less than 
                                        //or equal to ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES

    isp_calibdata_awb_global_t          awb_global;
    isp_calibdata_gmm_t                 awb_gmm[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    isp_calibdata_awb_gain_t            awb_gain[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    isp_calibdata_awb_door_type_t       awb_door_type[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];

    isp_calibdata_awb_cc_t              awb_cc[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    isp_calibdata_awb_sensorgain2sat_t  awb_sensor_gain2sat[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    isp_calibdata_awb_sat2cc_t          awb_sat2cc[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];

    isp_calibdata_awb_lsc_global_t      awb_lsc_global;
    isp_calibdata_awb_sensorgain2vig_t  awb_sensor_gain2vig[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
    isp_calibdata_awb_vig2lsc_t         awb_vig2lsc[ISP_CALIBDATA_CONFIG_AWB_MAX_ILLU_PROFILES];
} isp_calibdata_awb_t;



typedef struct _isp_calibdata_t
{
    isp_calibdata_resolution_t  resolution;
    isp_calibdata_awb_t         awb;
    isp_calibdata_ae_t          ae;
    isp_calibdata_bls_t         bls;
    isp_calibdata_degamma_t     degamma;
    isp_calibdata_gamma_t       gamma;
    isp_calibdata_ispfilter_t   ispFilter;
    isp_calibdata_cac_t         cac;
    isp_calibdata_bnr_t         bnr;
    isp_calibdata_dpc_t         dpc;
    isp_calibdata_stnr_t        stnr;
    isp_calibdata_wdr_t         wdr;
} isp_calibdata_t;


#ifdef __cplusplus
}
#endif


#endif
