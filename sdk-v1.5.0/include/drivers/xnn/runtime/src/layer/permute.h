#ifndef LAYER_PERMUTE_H
#define LAYER_PERMUTE_H

#include "layer.h"

namespace xnn {

class Permute : public Layer
{
public:
    Permute();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int order_type;
};

} // namespace xnn

#endif // LAYER_PERMUTE_H
