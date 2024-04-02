#ifndef __ISP_EXP_PRIOR_H__
#define __ISP_EXP_PRIOR_H__

#include "isp_types.h"
#include "isp_calibdata.h"
#include "isp_awb_alg.h"
#ifdef __cplusplus
extern "C"
{
#endif


#define AWB_EXPPRIOR_FILTER_SIZE_MAX   50       /**< AWB prior exp filter count max. It should be higher than the min count */
#define AWB_EXPPRIOR_FILTER_SIZE_MIN   1        /**< AWB prior exp filter count min */

#define AWB_EXPPRIOR_MIDDLE             0.5f    /**< value to initialize the gexp prior filter */



/*****************************************************************************/
/**
 *  @brief  Evaluation of the calculated probability
 */
/*****************************************************************************/
typedef enum _exp_prior_calc_evaluation_t
{
    EXPPRIOR_INVALID            = 0,
    EXPPRIOR_INDOOR             = 1,    /**< evaluated to indoor */
    EXPPRIOR_OUTDOOR            = 2,    /**< evaluated to outdoor */    
    EXPPRIOR_TRANSITION_RANGE   = 3,    /**< evaluated to transition range between indoor and outdoor */
    EXPPRIOR_MAX                = 4,
} exp_prior_calc_evaluation_t;


int isp_awb_exp_resize_iir_filter(awb_alg_t *awb_alg, isp_calibdata_awb_iir_t *exp_prior_ctx, const uint16_t size, const float initValue);
int isp_awb_exp_prior_process_frame(awb_alg_t *awb_alg, const awbm_info_t *awbm_info);

#ifdef __cplusplus
}
#endif

#endif /* __ISP_EXP_PRIOR_H__ */
