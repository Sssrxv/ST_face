#ifndef LAYER_CONCAT_H
#define LAYER_CONCAT_H

#include "layer.h"

namespace xnn {

class Concat : public Layer
{
public:
    Concat();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    int axis;

    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_CONCAT_H
