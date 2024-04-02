#ifndef _ISP_AE_ALG_H_
#define _ISP_AE_ALG_H_

#include "isp_types.h"
#include "isphw_isp_exp.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef uint8_t mean_luma_t[EXP_GRID_ITEMS];
typedef uint32_t hist_bins_t[HIST_NUM_BINS];


/*****************************************************************************/
/**
 *          ae_sem_mode_t
 *
 * @brief   mode type of AE Scene Evaluation
 *
 */
/*****************************************************************************/
typedef enum _ae_sem_mode_t
{
    AE_SCENE_EVALUATION_INVALID    = 0,        /* invalid (only used for initialization) */
    AE_SCENE_EVALUATION_DISABLED   = 1,        /* Scene Evaluation disabled (fix setpoint) */
    AE_SCENE_EVALUATION_FIX        = 2,        /* Scene Evaluation fix (static ROI) */
    AE_SCENE_EVALUATION_ADAPTIVE   = 3,        /* Scene Evaluation adaptive (ROI caluclated by Scene Evaluation */
    AE_SCENE_EVALUATION_MAX
} ae_sem_mode_t;


/*****************************************************************************/
/**
 *          ae_damping_mode_t
 *
 * @brief   mode type of AE Damping
 *
 */
/*****************************************************************************/
typedef enum _ae_damping_mode_t
{
    AE_DAMPING_MODE_INVALID        = 0,        /* invalid (only used for initialization) */
    AE_DAMPING_MODE_STILL_IMAGE    = 1,        /* damping mode still image */
    AE_DAMPING_MODE_VIDEO          = 2,        /* damping mode video */
    AE_DAMPING_MODE_MAX
} ae_damping_mode_t;


/*****************************************************************************/
/**
 *          ae_exp_mode_t
 *
 * @brief   mode type of AE Exposure, normal
 *
 */
/*****************************************************************************/
typedef enum _ae_exp_mode_t
{
    AE_EXP_MODE_INVALID           = 0,        /* invalid (only used for initialization) */
    AE_EXP_MODE_NORMAL            = 1,        /* Exposure controled in normal mode*/
    AE_EXP_MODE_MAX
}ae_exp_mode_t;


/*****************************************************************************/
/**
 *          ae_ecm_mode_t
 *
 * @brief   mode type of AE Exposure Conversion
 *
 */
/*****************************************************************************/
typedef enum _ae_ecm_mode_t
{
    AE_EXPOSURE_CONVERSION_INVALID = 0,        /* invalid (only used for initialization) */
    AE_EXPOSURE_CONVERSION_LINEAR  = 1,        /* Exposure Conversion uses a linear function (eq. 38) */
    AE_EXPOSURE_CONVERSION_MAX
} ae_ecm_mode_t;


typedef struct _ae_result_t
{
    float analog_gain;
    float digital_gain;
    float itime;
}ae_result_t;


typedef struct _ae_alg_t
{
    float min_analog_gain;
    float max_analog_gain;
    float min_digital_gain;
    float max_digital_gain;
    float min_itime;
    float max_itime;
    float start_exposure;     /**< start exposure value of the ae control system */
    float exposure;           /**< current exposure value of the ae control system */
    float min_exposure;       /**< minmal exposure value (sensor specific) */
    float max_exposure;       /**< maximal exposure value (sensor specific) */
    float analog_gain;
    float digital_gain;
    float itime;
    float scale_factor_again_itime;
    int   enable_damp;

    float mean_luma;
    float mean_luma_object;
    ae_exp_mode_t exp_mode;
    ae_sem_mode_t sem_mode;

    float set_point;
    float sem_set_point;
    float orig_set_point;
    float orig_sem_set_point;
    ae_damping_mode_t damping_mode;
    float damp_over_still;
    float damp_under_still;
    float damp_over_video;
    float damp_under_video;

    float d;
    float z;
    float m0;

    float ecm_old_alpha;        /**< for ECM lock range >*/
    float ecm_old_gain;         /**< for ECM lock range >*/
    float ecm_old_tint;         /**< for ECM lock range >*/

    union _sem_ctx
    {
        struct _sem
        {
            float mean_luma_region0;
            float mean_luma_region1;
            float mean_luma_region2;
            float mean_luma_region3;
            float mean_luma_region4;
        } sem;

        struct _asem
        {
            float               otsu_threshold;  /**< threshold calculated with otsu */

            mean_luma_t         binary0;        /**< thresholded luma matrix */
            mean_luma_t         cc0;            /**< matrix of labeled 0-conponents */
            mean_luma_t         ll0;            /**< matrix of labeled 1-conponents */
            uint32_t            num_cc0;         /**< number of 0-conponents */
            uint32_t            lbl_biggest_cc0;  /**< label of biggest 0-connected-component */
            float               max_center_x0;
            float               max_center_y0;

            mean_luma_t         binary1;        /**< thresholded luma matrix */
            mean_luma_t         cc1;            /**< matrix of labeled 1-conponents */
            mean_luma_t         ll1;            /**< matrix of labeled 1-conponents */
            uint32_t            num_cc1;         /**< number of 1-conponents */
            uint32_t            lbl_biggest_cc1;  /**< label of biggest 1-connected-component */
            float               max_center_x1;
            float               max_center_y1;

            mean_luma_t         object_region;   /**< resulting matrix  (0 = Object, 1 = Background ) */
        } asem;
    } sem_ctx;

    /* control loop module */
    #define CLM_HIST_NUM_BINS   ( 3 * HIST_NUM_BINS )

    float               clm_tolerance;
    float               luma_deviation;
    uint32_t            clm_histogram[CLM_HIST_NUM_BINS];
    uint32_t            clm_histogramSize;
    float               clm_histogramX[CLM_HIST_NUM_BINS];
    uint32_t            sum_histogram;
    float               mean_histogram;

    /* debug output */
    hist_bins_t         histogram;
    mean_luma_t         luma;

    ae_flicker_type_t   ecm_flicker_select;
    float               ecm_t_flicker;        /**< flicker period */
    float               ecm_t0_fac;
    float               ecm_t0;              /**< start of flicker avoidance */
    float               ecm_a0;              /**< linear: slope of gain */

    ae_result_t         ae_result;
    uint32_t            cnt;
}ae_alg_t;



int ae_alg_reset(ae_alg_t *ae_alg);
int ae_alg_config(ae_alg_t *ae_alg, float frame_rate, const isp_sensor_exp_param_t *exp_param, int hdr_idx);
int ae_alg_set_start_exposure(ae_alg_t *ae_alg, float again, float dgain, float itime);
int ae_alg_run(ae_alg_t *ae_alg, aem_info_t *aem_info);
int ae_alg_get_result(ae_alg_t *ae_alg, ae_result_t *ae_result);
int ae_alg_get_search_state(ae_alg_t *ae_alg, ae_mode_t mode, ae_search_state_t *search_state);


#ifdef __cplusplus
}
#endif



#endif
