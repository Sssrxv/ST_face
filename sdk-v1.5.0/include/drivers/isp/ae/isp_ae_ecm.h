#ifndef __ISP_AE_ECM_H__
#define __ISP_AE_ECM_H__
 

#ifdef __cplusplus
extern "C"
{
#endif


#include "isp_types.h"
#include "isp_ae_alg.h"


/*****************************************************************************/
/**
 * predefined flicker period values for ECM module
 */
/*****************************************************************************/
#define ECM_TFLICKER_OFF   ((ECM_TFLICKER_100HZ+ECM_TFLICKER_120HZ)/2)  //!< predefined flicker period value for ECM module
#define ECM_TFLICKER_100HZ (1.0/100.0)                                  //!< predefined flicker period value for ECM module
#define ECM_TFLICKER_120HZ (1.0/120.0)                                  //!< predefined flicker period value for ECM module


int ecm_execute(ae_alg_t *ae_alg, float exposure, float *split_analog_gain, float *split_digital_gain, float *split_integration_time);



#ifdef __cplusplus
}
#endif


#endif /* __ISP_AE_ECM_H__ */
