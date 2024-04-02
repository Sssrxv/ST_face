#ifndef LAYER_DEQUANTIZE_INT8_H
#define LAYER_DEQUANTIZE_INT8_H


#include "layer.h"

namespace xnn {

class DequantizeInt8 : public Layer
{
public:
    DequantizeInt8();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    void*    workbuffer;
    int      workbuffer_size;

    int   hwc2chw; //0: chw to chw dequantize, 1: hwc to chw dequantize
    float scale;
    int use_mem_desc;
    xnn_memory_desc_t input_mem_desc;
};


} // namespace xnn

#endif //LAYER_DEQUANTIZE_INT8_H
