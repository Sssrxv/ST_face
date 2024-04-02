#ifndef LAYER_SPLIT_H
#define LAYER_SPLIT_H

#include "layer.h"

namespace xnn {

class Split : public Layer
{
public:
    Split();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
};

} // namespace xnn

#endif // LAYER_SPLIT_H
