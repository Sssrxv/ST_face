#ifndef LAYER_EXPANDDIMS_H
#define LAYER_EXPANDDIMS_H

#include "layer.h"

namespace xnn {

class ExpandDims : public Layer
{
public:
    ExpandDims();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int expand_w;
    int expand_h;
    int expand_c;
    Mat axes;
};

} // namespace xnn

#endif // LAYER_EXPANDDIMS_H
