#ifndef LAYER_EXP_H
#define LAYER_EXP_H

#include "layer.h"

namespace xnn {

class Exp : public Layer
{
public:
    Exp();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float base;
    float scale;
    float shift;
};

} // namespace xnn

#endif // LAYER_EXP_H
