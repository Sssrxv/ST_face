#ifndef _ISPHW_ISP_FILT_H_
#define _ISPHW_ISP_FILT_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "isp_hal.h"    

#define AIVA_FILT_FILT_THRESH_SH0_MASK (ISP_CORE_ISP_FILT_THRESH_SH0__AIVA_FILT_THRESH_SH0_MASK)
#define AIVA_FILT_FILT_THRESH_SH1_MASK (ISP_CORE_ISP_FILT_THRESH_SH1__AIVA_FILT_THRESH_SH1_MASK)
#define AIVA_FILT_FILT_THRESH_BL0_MASK (ISP_CORE_ISP_FILT_THRESH_BL0__AIVA_FILT_THRESH_BL0_MASK)
#define AIVA_FILT_FILT_THRESH_BL1_MASK (ISP_CORE_ISP_FILT_THRESH_BL1__AIVA_FILT_THRESH_BL1_MASK)

#define AIVA_FILT_FILT_FAC_SH0_MASK    (ISP_CORE_ISP_FILT_FAC_SH0__AIVA_FILT_FAC_SH0_MASK)
#define AIVA_FILT_FILT_FAC_SH1_MASK    (ISP_CORE_ISP_FILT_FAC_SH1__AIVA_FILT_FAC_SH1_MASK)
#define AIVA_FILT_FILT_FAC_MID_MASK    (ISP_CORE_ISP_FILT_FAC_MID__AIVA_FILT_FAC_MID_MASK)
#define AIVA_FILT_FILT_FAC_BL0_MASK    (ISP_CORE_ISP_FILT_FAC_BL0__AIVA_FILT_FAC_BL0_MASK)
#define AIVA_FILT_FILT_FAC_BL1_MASK    (ISP_CORE_ISP_FILT_FAC_BL1__AIVA_FILT_FAC_BL1_MASK)

#define AIVA_FILT_CHR_H_MODE_BYPASS    (0)    
#define AIVA_FILT_CHR_H_MODE_STATIC    (1)    
#define AIVA_FILT_CHR_H_MODE_DYN_1     (2)
#define AIVA_FILT_CHR_H_MODE_DYN_2     (3)

#define AIVA_FILT_CHR_V_MODE_BYPASS    (0) 
#define AIVA_FILT_CHR_V_MODE_STATIC8   (1)
#define AIVA_FILT_CHR_V_MODE_STATIC10  (2)
#define AIVA_FILT_CHR_V_MODE_STATIC12  (3)    
 
#define AIVA_FILT_MODE_STATIC          (0)
#define AIVA_FILT_MODE_DYNAMIC         (1)

typedef enum _filt_denoise_level_t
{
    FILT_DENOISE_LEVEL_0,
    FILT_DENOISE_LEVEL_1,
    FILT_DENOISE_LEVEL_2,
    FILT_DENOISE_LEVEL_3,
    FILT_DENOISE_LEVEL_4,
    FILT_DENOISE_LEVEL_5,
    FILT_DENOISE_LEVEL_6,
    FILT_DENOISE_LEVEL_7,
    FILT_DENOISE_LEVEL_8,
    FILT_DENOISE_LEVEL_9,
    FILT_DENOISE_LEVEL_10,
    FILT_DENOISE_LEVEL_TEST,
    FILT_DENOISE_LEVEL_MAX
}filt_denoise_level_t;


typedef enum _filt_sharpening_level_t
{
    FILT_SHARPENING_LEVEL_0,
    FILT_SHARPENING_LEVEL_1,
    FILT_SHARPENING_LEVEL_2,
    FILT_SHARPENING_LEVEL_3,
    FILT_SHARPENING_LEVEL_4,
    FILT_SHARPENING_LEVEL_5,
    FILT_SHARPENING_LEVEL_6,
    FILT_SHARPENING_LEVEL_7,
    FILT_SHARPENING_LEVEL_8,
    FILT_SHARPENING_LEVEL_9,
    FILT_SHARPENING_LEVEL_10,
    FILT_SHARPENING_LEVEL_MAX
}filt_sharpening_level_t;

int isphw_isp_filt_enable(void);
int isphw_isp_filt_disable(void);
int isphw_isp_filt_set_filter_parameter(filt_denoise_level_t denoise_level, filt_sharpening_level_t sharpening_level);
int isphw_isp_filt_set_filter_parameter_ext(uint16_t *denoise_filter, uint8_t *sharp_filter);






#ifdef __cplusplus
}
#endif





#endif
