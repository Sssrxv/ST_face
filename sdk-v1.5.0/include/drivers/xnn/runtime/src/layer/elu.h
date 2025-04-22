#ifndef LAYER_ELU_H
#define LAYER_ELU_H

#include "layer.h"

namespace xnn {

class ELU : public Layer
{
public:
    ELU();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float alpha;
};

} // namespace xnn

#endif // LAYER_ELU_H
