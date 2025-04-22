#ifndef LAYER_RESHAPE_H
#define LAYER_RESHAPE_H

#include "layer.h"

namespace xnn {

class Reshape : public Layer
{
public:
    Reshape();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // reshape flag
    // 0 = copy from bottom
    // -1 = remaining
    // -233 = drop this dim (default)
    int w;
    int h;
    int c;
    int permute;
    int ndim;
};

} // namespace xnn

#endif // LAYER_RESHAPE_H
