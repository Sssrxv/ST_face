#ifndef _LIBJPEG_HARDWARE_H_
#define _LIBJPEG_HARDWARE_H_

#include <stdint.h>

#define MJPEG_QTABLE_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _mjpeg_comp_level_t
{
    MJPEG_COMP_LEVEL_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    MJPEG_COMP_LEVEL_HIGH          = 1,    /**< high compressing rate, %75 quality */
    MJPEG_COMP_LEVEL_LOW           = 2,    /**< low compressing rate */
    MJPEG_COMP_LEVEL_01_PERCENT    = 3,    /**< %01 quality */
    MJPEG_COMP_LEVEL_10_PERCENT    = 4,    /**< %10 quality */
    MJPEG_COMP_LEVEL_20_PERCENT    = 5,    /**< %20 quality */
    MJPEG_COMP_LEVEL_30_PERCENT    = 6,    /**< %30 quality */
    MJPEG_COMP_LEVEL_40_PERCENT    = 7,    /**< %40 quality */
    MJPEG_COMP_LEVEL_50_PERCENT    = 8,    /**< %50 quality */
    MJPEG_COMP_LEVEL_60_PERCENT    = 9,    /**< %60 quality */
    MJPEG_COMP_LEVEL_70_PERCENT    = 10,   /**< %70 quality */
    MJPEG_COMP_LEVEL_80_PERCENT    = 11,   /**< %80 quality */
    MJPEG_COMP_LEVEL_99_PERCENT    = 12,   /**< %99 quality */
    MJPEG_COMP_LEVEL_MAX                   /**< upper border (only for an internal evaluation) */
} mjpeg_comp_level_t;


extern const uint8_t YQTable75PerCent[];
extern const uint8_t UVQTable75PerCent[];
extern const uint8_t YQTableLowComp1[];
extern const uint8_t UVQTableLowComp1[];
extern const uint8_t YQTable99PerCent[];
extern const uint8_t UVQTable99PerCent[];
extern const uint8_t YQTable80PerCent[];
extern const uint8_t UVQTable80PerCent[];
extern const uint8_t YQTable70PerCent[];
extern const uint8_t UVQTable70PerCent[];
extern const uint8_t YQTable60PerCent[];
extern const uint8_t UVQTable60PerCent[];
extern const uint8_t YQTable50PerCent[];
extern const uint8_t UVQTable50PerCent[];
extern const uint8_t YQTable40PerCent[];
extern const uint8_t UVQTable40PerCent[];
extern const uint8_t YQTable30PerCent[];
extern const uint8_t UVQTable30PerCent[];
extern const uint8_t YQTable20PerCent[];
extern const uint8_t UVQTable20PerCent[];
extern const uint8_t YQTable10PerCent[];
extern const uint8_t UVQTable10PerCent[];
extern const uint8_t YQTable01PerCent[];
extern const uint8_t UVQTable01PerCent[];


#ifdef __cplusplus
}
#endif


#endif // __LIBJPEG_HARDWARE_H__