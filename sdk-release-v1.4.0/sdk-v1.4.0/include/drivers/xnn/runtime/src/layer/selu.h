#ifndef LAYER_SELU_H
#define LAYER_SELU_H

#include "layer.h"

namespace xnn {

class SELU : public Layer
{
public:
    SELU();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float alpha;
    float lambda;
};

} // namespace xnn

#endif // LAYER_SELU_H
