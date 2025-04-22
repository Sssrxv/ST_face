#ifndef LAYER_QUANTIZE_H
#define LAYER_QUANTIZE_H

#include "layer.h"

namespace xnn {

class Quantize : public Layer
{
public:
    Quantize();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    void*    workbuffer;
    int      workbuffer_size;

    int   chw2hwc; //0: chw to chw quantize, 1: chw to hwc quantize 
    int   caller_alloc_top; //0: allocate top blob by Quantize, 1: allocate top blob by the caller
    float scale;
    int   output_cn;
    int use_mem_desc;
    xnn_memory_desc_t output_mem_desc;
};

} // namespace xnn

#endif // LAYER_QUANTIZE_H
