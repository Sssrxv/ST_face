#ifndef LAYER_ARGMAX_H
#define LAYER_ARGMAX_H

#include "layer.h"

namespace xnn {

class ArgMax : public Layer
{
public:
    ArgMax();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int out_max_val;
    int topk;
};

} // namespace xnn

#endif // LAYER_ARGMAX_H
