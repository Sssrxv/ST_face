#ifndef LAYER_CLIP_H
#define LAYER_CLIP_H

#include "layer.h"

namespace xnn {

class Clip : public Layer
{
public:
    Clip();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float min;
    float max;
};

} // namespace xnn

#endif // LAYER_CLIP_H
