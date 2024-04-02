#ifndef LAYER_POWER_H
#define LAYER_POWER_H

#include "layer.h"

namespace xnn {

class Power : public Layer
{
public:
    Power();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float power;
    float scale;
    float shift;
};

} // namespace xnn

#endif // LAYER_POWER_H
