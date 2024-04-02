#ifndef LAYER_SQUEEZE_H
#define LAYER_SQUEEZE_H

#include "layer.h"

namespace xnn {

class Squeeze : public Layer
{
public:
    Squeeze();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int squeeze_w;
    int squeeze_h;
    int squeeze_c;
};

} // namespace xnn

#endif // LAYER_SQUEEZE_H
