#ifndef LAYER_REDUCTION_H
#define LAYER_REDUCTION_H

#include "layer.h"

namespace xnn {

class Reduction : public Layer
{
public:
    Reduction();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

    enum {
        ReductionOp_SUM     = 0,
        ReductionOp_ASUM    = 1,
        ReductionOp_SUMSQ   = 2,
        ReductionOp_MEAN    = 3,
        ReductionOp_MAX     = 4,
        ReductionOp_MIN     = 5,
        ReductionOp_PROD    = 6
    };

public:
    // param
    int operation;
    int dim;
    float coeff;
};

} // namespace xnn

#endif // LAYER_REDUCTION_H
