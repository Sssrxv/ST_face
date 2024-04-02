#ifndef _ISP_TYPES_H_
#define _ISP_TYPES_H_

#include <stdint.h>
#include "isp_base_types.h"
#include "isp_calibdata.h"
#include "isp_api.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _image_prop_t
{
    isp_image_fmt_t image_fmt;
    uint32_t width;
    uint32_t height;
    uint32_t luma_pitch;
    uint32_t chroma_pitch;
}image_prop_t;

typedef struct _cc_matrix_t
{
    float coeff[9];
}cc_matrix_t;


typedef struct _cc_offset_t
{
    float coeff[3];
}cc_offset_t;

#define MAX_LSC_SECTORS   (16)
#define LSC_DATA_TBL_SIZE (MAX_LSC_SECTORS + 1) * (MAX_LSC_SECTORS + 1)

typedef struct _lsc_matrix_t
{
    uint16_t dataR[LSC_DATA_TBL_SIZE];
    uint16_t dataGR[LSC_DATA_TBL_SIZE];
    uint16_t dataGB[LSC_DATA_TBL_SIZE];
    uint16_t dataB[LSC_DATA_TBL_SIZE];
} lsc_matrix_t;


#define LSC_GRAD_TBL_SIZE (8)
#define LSC_SIZE_TBL_SIZE (8)
typedef struct _lsc_sector_t
{
    uint16_t lsc_xgrad_tbl[LSC_GRAD_TBL_SIZE];
    uint16_t lsc_ygrad_tbl[LSC_GRAD_TBL_SIZE];
    uint16_t lsc_xsize_tbl[LSC_SIZE_TBL_SIZE];
    uint16_t lsc_ysize_tbl[LSC_SIZE_TBL_SIZE];
}lsc_sector_t;


#define PREP_LSC_GRAD_TBL_SIZE (8)
#define PREP_LSC_SIZE_TBL_SIZE (16)
typedef struct _prep_lsc_sector_t
{
    uint16_t lsc_xgrad_tbl[PREP_LSC_GRAD_TBL_SIZE];
    uint16_t lsc_ygrad_tbl[PREP_LSC_GRAD_TBL_SIZE];
    uint16_t lsc_xsize_tbl[PREP_LSC_SIZE_TBL_SIZE];
    uint16_t lsc_ysize_tbl[PREP_LSC_SIZE_TBL_SIZE];
}prep_lsc_sector_t;



typedef struct _wb_gain_t
{
    float red;
    float green_r;
    float green_b;
    float blue;
}wb_gain_t;


typedef struct _lens_range_t
{
    uint32_t min_lens;
    uint32_t max_lens;
    uint32_t step_lens;
}lens_range_t;


typedef enum _af_window_id_t
{
    AF_WINDOW_ID_A,
    AF_WINDOW_ID_B,
    AF_WINDOW_ID_C,
    AF_WINDOW_ID_MAX
}af_window_id_t;




// CPROC
//--------

typedef enum _cproc_chrom_out_range_t
{
    CPROC_CHROM_OUT_RANGE_BT601,
    CPROC_CHROM_OUT_RANGE_FULL,
    CPROC_CHROM_OUT_RANGE_MAX
}cproc_chrom_out_range_t;

typedef enum _cproc_lum_in_range_t
{
    CPROC_LUM_IN_RANGE_BT601,
    CPROC_LUM_IN_RANGE_FULL,
    CPROC_LUM_IN_RANGE_MAX
}cproc_lum_in_range_t;



typedef enum _cproc_lum_out_range_t
{
    CPROC_LUM_OUT_RANGE_BT601,
    CPROC_LUM_OUT_RANGE_FULL,
    CPROC_LUM_OUT_RANGE_MAX
}cproc_lum_out_range_t;

typedef struct _cproc_prop_t
{
    cproc_chrom_out_range_t chrom_out_range;
    cproc_lum_in_range_t    lum_in_range;
    cproc_lum_out_range_t   lum_out_range;
}cproc_prop_t;


// GAMMA
//-------

typedef enum _isp_gamma_seg_mode_t
{
    ISP_GAMMA_SEG_MODE_LOG  = 0,
    ISP_GAMMA_SEG_MODE_EQU  = 1,
    ISP_GAMMA_SEG_MODE_MAX
} isp_gamma_seg_mode_t;

#define ISP_GAMMA_CURVE_SIZE (17)
typedef struct _isp_gamma_curve_t
{
    uint16_t gamma_y[ISP_GAMMA_CURVE_SIZE];
} isp_gamma_curve_t;


// DEGAMMA
//---------
#define ISP_DEGAMMA_CURVE_SIZE      (17)
typedef struct _isp_degamma_curve_t
{
    uint8_t  segment[ISP_DEGAMMA_CURVE_SIZE-1]; /* x_i segment size*/
    uint16_t red[ISP_DEGAMMA_CURVE_SIZE];       /* red point */
    uint16_t green[ISP_DEGAMMA_CURVE_SIZE];     /* green point */
    uint16_t blue[ISP_DEGAMMA_CURVE_SIZE];      /* blue point */
} isp_degamma_curve_t;




typedef struct _awbm_info_t
{
    // The frame white points measuring result.
    awbm_result_t awbm_result;
    hist_result_t hist_result;

    // The coefficient applied on this frame.
    cc_matrix_t cc_matrix;
    cc_offset_t cc_offset;
    wb_gain_t wb_gain;
    lsc_matrix_t lsc_matrix;
    float analog_gain;
    float itime;
} awbm_info_t;


typedef struct _aem_info_t
{
    //Normal AE measuring result
    int             exp_enabled;
    exp_result_t    exp_result;
    hist_result_t   hist_result;

    float           analog_gain;
    float           digital_gain;
    float           itime;
    float           frame_rate;
} aem_info_t;



typedef struct _afm_info_t
{
    afm_result_t  afm_result;
    uint32_t      lens_pos;
    isp_window_t  region_window[AF_WINDOW_ID_MAX];
} afm_info_t;




typedef enum _bnr_gain_usage_t
{
    BNR_GAIN_USAGE_INVALID        = 0,
    BNR_GAIN_USAGE_DISABLED       = 1,
    BNR_GAIN_USAGE_NF_GAINS       = 2,
    BNR_GAIN_USAGE_LSC_GAINS      = 3,
    BNR_GAIN_USAGE_NF_LSC_GAINS   = 4,
    BNR_GAIN_USAGE_AWB_GAINS      = 5,
    BNR_GAIN_USAGE_AWB_LSC_GAINS  = 6,
    BNR_GAIN_USAGE_MAX
} bnr_gain_usage_t;


typedef enum _bnr_rb_filter_size_t
{
    BNR_RB_FILTER_SIZE_INVLAID = 0,
    BNR_RB_FILTER_SIZE_9x9     = 1,
    BNR_RB_FILTER_SIZE_13x9    = 2,
    BNR_RB_FILTER_SIZE_MAX
} bnr_rb_filter_size_t;


#define BNR_MAX_SPATIAL_COEFFS (6)
typedef struct _bnr_spatial_t
{
    uint8_t weightCoeff[BNR_MAX_SPATIAL_COEFFS];
} bnr_spatial_t;


typedef struct _bnr_config_t
{
    bnr_gain_usage_t        gain_usage;
    bnr_rb_filter_size_t    rb_filter_size;

    int process_red_pixel;
    int process_green_r_pixel;
    int process_green_b_pixel;
    int process_blue_pixel;

    bnr_spatial_t   spatial_g;
    bnr_spatial_t   spatial_rb;
} bnr_config_t;


typedef struct _dpc_static_config_t
{
    uint32_t isp_dpc_mode;
    uint32_t isp_dpc_output_mode;
    uint32_t isp_dpc_set_use;
             
    uint32_t isp_dpc_methods_set_1;
    uint32_t isp_dpc_methods_set_2;
    uint32_t isp_dpc_methods_set_3;
             
    uint32_t isp_dpc_line_thresh_1;
    uint32_t isp_dpc_line_mad_fac_1;
    uint32_t isp_dpc_pg_fac_1;
    uint32_t isp_dpc_rnd_thresh_1;
    uint32_t isp_dpc_rg_fac_1;
             
    uint32_t isp_dpc_line_thresh_2;
    uint32_t isp_dpc_line_mad_fac_2;
    uint32_t isp_dpc_pg_fac_2;
    uint32_t isp_dpc_rnd_thresh_2;
    uint32_t isp_dpc_rg_fac_2;
             
    uint32_t isp_dpc_line_thresh_3;
    uint32_t isp_dpc_line_mad_fac_3;
    uint32_t isp_dpc_pg_fac_3;
    uint32_t isp_dpc_rnd_thresh_3;
    uint32_t isp_dpc_rg_fac_3;
             
    uint32_t isp_dpc_ro_limits;
    uint32_t isp_dpc_rnd_offs;
} dpc_static_config_t;



typedef enum _cac_h_clip_mode_t
{
    CAC_H_CLIP_MODE_FIX4,
    CAC_H_CLIP_MODE_DYN5,
    CAC_H_CLIP_MODE_MAX
} cac_h_clip_mode_t;


typedef enum _cac_v_clip_mode_t
{
    CAC_V_CLIP_MODE_FIX2,
    CAC_V_CLIP_MODE_FIX3,
    CAC_V_CLIP_MODE_DYN4,
    CAC_V_CLIP_MODE_MAX
} cac_v_clip_mode_t;


typedef struct _cac_config_t
{
    uint16_t            width;
    uint16_t            height;
    int16_t             h_center_offset;
    int16_t             v_center_offset;
    cac_h_clip_mode_t   h_clip_mode;
    cac_v_clip_mode_t   v_clip_mode;
    uint16_t            a_blue;
    uint16_t            a_red;
    uint16_t            b_blue;
    uint16_t            b_red;
    uint16_t            c_blue;
    uint16_t            c_red;
    uint8_t             x_ns;
    uint8_t             x_nf;
    uint8_t             y_ns;
    uint8_t             y_nf;
} cac_config_t;



#ifdef __cplusplus
}
#endif


#endif
