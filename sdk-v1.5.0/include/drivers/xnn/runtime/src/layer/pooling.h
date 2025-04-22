#ifndef LAYER_POOLING_H
#define LAYER_POOLING_H

#include "layer.h"

namespace xnn {

class Pooling : public Layer
{
public:
    Pooling();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

    enum { PoolMethod_MAX = 0, PoolMethod_AVE = 1 };

public:
    // param
    int pooling_type;
    int kernel_w;
    int kernel_h;
    int stride_w;
    int stride_h;
    int pad_left;
    int pad_right;
    int pad_top;
    int pad_bottom;
    int global_pooling;
    int pad_mode;// 0=full 1=valid 2=SAME

    // 0=fp32 1=int8 2=fp16
    int top_blob_type;

    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_POOLING_H
