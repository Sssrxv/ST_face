#ifndef LAYER_REORG_H
#define LAYER_REORG_H

#include <string>
#include "layer.h"

namespace xnn {

class Reorg : public Layer
{
public:
    Reorg();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int stride;
    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_REORG_H
