#ifndef LAYER_PACKING_H
#define LAYER_PACKING_H

#include "layer.h"

namespace xnn {

class Packing : public Layer
{
public:
    Packing();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int out_packing;
    int use_padding;
};

} // namespace xnn

#endif // LAYER_PACKING_H
