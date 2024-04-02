#ifndef LAYER_INPUT_CONVERT_H
#define LAYER_INPUT_CONVERT_H


#include "layer.h"

namespace xnn {

class InputConvert: public Layer
{
public:
    InputConvert();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    void*    workbuffer;
    int      workbuffer_size;

    float scale;
    // 0: CHW, 1: HWC_C16N
    int output_dataformat;
    int output_cn;
    int use_mem_desc;
    xnn_memory_desc_t output_mem_desc;
};

} // namespace xnn



#endif // LAYER_INPUT_CONVERT_H
