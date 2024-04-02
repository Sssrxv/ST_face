#ifndef LAYER_INPUT_CONVERT2UINT8_H
#define LAYER_INPUT_CONVERT2UINT8_H


#include "layer.h"

namespace xnn {

class InputConvert2UINT8: public Layer
{
public:
    InputConvert2UINT8();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    void*    workbuffer;
    int      workbuffer_size;

    float scale;
    // 0: CHW, 1: RGBA_UINT8
    int output_dataformat;
    int output_cn;
    int use_mem_desc;
    xnn_memory_desc_t output_mem_desc;
};

} // namespace xnn



#endif // LAYER_INPUT_CONVERT2UINT8_H
