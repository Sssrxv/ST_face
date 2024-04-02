#ifndef LAYER_HARDSIGMOID_H
#define LAYER_HARDSIGMOID_H

#include "layer.h"

namespace xnn {

class HardSigmoid : public Layer
{
public:
    HardSigmoid();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float alpha, beta, lower, upper;
};

} // namespace xnn

#endif // LAYER_HARDSIGMOID_H
