#ifndef LAYER_TANH_H
#define LAYER_TANH_H

#include "layer.h"

namespace xnn {

class TanH : public Layer
{
public:
    TanH();

    virtual int load_param(const ParamDict& pd);
    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;
};

} // namespace xnn

#endif // LAYER_TANH_H
