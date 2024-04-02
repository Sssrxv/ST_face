#ifndef LAYER_FLATTEN_H
#define LAYER_FLATTEN_H

#include "layer.h"

namespace xnn {

class Flatten : public Layer
{
public:
    Flatten();

    virtual int load_param(const ParamDict& pd);
    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;
};

} // namespace xnn

#endif // LAYER_FLATTEN_H
