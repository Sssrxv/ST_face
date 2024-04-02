#ifndef LAYER_CAST_H
#define LAYER_CAST_H

#include "layer.h"

namespace xnn {

class Cast : public Layer
{
public:
    Cast();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // element type
    // 0 = auto
    // 1 = float32
    // 2 = float16
    // 3 = int8
    int type_from;
    int type_to;
};

} // namespace xnn

#endif // LAYER_CAST_H
