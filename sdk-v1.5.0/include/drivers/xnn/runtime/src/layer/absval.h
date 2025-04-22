#ifndef LAYER_ABSVAL_H
#define LAYER_ABSVAL_H

#include "layer.h"

namespace xnn {

class AbsVal : public Layer
{
public:
    AbsVal();

    virtual int load_param(const ParamDict& pd);
    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;
};

} // namespace xnn

#endif // LAYER_ABSVAL_H
