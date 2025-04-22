#ifndef LAYER_BNLL_H
#define LAYER_BNLL_H

#include "layer.h"

namespace xnn {

class BNLL : public Layer
{
public:
    BNLL();

    virtual int load_param(const ParamDict& pd);
    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
};

} // namespace xnn

#endif // LAYER_BNLL_H
