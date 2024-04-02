#ifndef LAYER_SPP_H
#define LAYER_SPP_H

#include "layer.h"

namespace xnn {

class SPP : public Layer
{
public:
    SPP();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

    enum { PoolMethod_MAX = 0, PoolMethod_AVE = 1 };

public:
    // param
    int pooling_type;
    int pyramid_height;
};

} // namespace xnn

#endif // LAYER_SPP_H
