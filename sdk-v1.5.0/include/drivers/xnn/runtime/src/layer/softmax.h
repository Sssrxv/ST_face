#ifndef LAYER_SOFTMAX_H
#define LAYER_SOFTMAX_H

#include "layer.h"

namespace xnn {

class Softmax : public Layer
{
public:
    Softmax();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int axis;
};

} // namespace xnn

#endif // LAYER_SOFTMAX_H
