#ifndef LAYER_DROPOUT_H
#define LAYER_DROPOUT_H

#include "layer.h"

namespace xnn {

class Dropout : public Layer
{
public:
    Dropout();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float scale;
};

} // namespace xnn

#endif // LAYER_DROPOUT_H
