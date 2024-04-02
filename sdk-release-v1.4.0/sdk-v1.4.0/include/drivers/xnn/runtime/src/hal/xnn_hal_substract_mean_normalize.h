#ifndef XNN_HAL_SUBSTRACT_MEAN_NORMALIZE_H_
#define XNN_HAL_SUBSTRACT_MEAN_NORMALIZE_H

#include <stdint.h>
#include "mat.h"

namespace xnn {

    // xnn_hal_substract_mean_normalize do the following operation,
    // 1. each uint8 image pixel will first substract the mean value
    // 2. the result of 1 will be multiplied by scale_norm to get value between 0~1
    // 3. the result of 2 will be multiplied by scale_int8 and cast to int8 data type
    //    (int8)(scale_int8 * ((x - mean) * scale_norm))
    // the data is supposed to be stored in rgba 4 channnel format, and the return Mat
    // norm_int8 will be a 16 channel HWC mat.
    int xnn_hal_substract_mean_normalize(const uint8_t* data, int stride, float mean, float scale_norm, float scale_int8, int w, int h, xnn::Mat &norm_int8);
    
    int xnn_hal_substract_mean_normalize(const uint8_t* data, int stride, float mean[3], float scale_norm[3], float scale_int8, int w, int h, xnn::Mat &norm_int8);

} //namespace xnn

#endif
