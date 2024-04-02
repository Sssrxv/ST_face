#ifndef LAYER_ELTWISE_H
#define LAYER_ELTWISE_H

#include "layer.h"

namespace xnn {

class Eltwise : public Layer
{
public:
    Eltwise();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

    enum { Operation_PROD = 0, Operation_SUM = 1, Operation_MAX = 2 };

public:
    // param
    int op_type;
    Mat coeffs;
};

} // namespace xnn

#endif // LAYER_ELTWISE_H
