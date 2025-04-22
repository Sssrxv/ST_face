#ifndef LAYER_RELU_H
#define LAYER_RELU_H

#include "layer.h"

namespace xnn {

class ReLU : public Layer
{
public:
    ReLU();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;
    virtual int forward_inplace_int8(Mat& bottom_top_blob, const Option& opt) const;

public:
    float slope;
};

} // namespace xnn

#endif // LAYER_RELU_H
