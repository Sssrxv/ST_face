#ifndef LAYER_SHUFFLECHANNEL_H
#define LAYER_SHUFFLECHANNEL_H

#include "layer.h"

namespace xnn {

class ShuffleChannel : public Layer
{
public:
    ShuffleChannel();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int group;
};

} // namespace xnn

#endif // LAYER_SHUFFLECHANNEL_H
