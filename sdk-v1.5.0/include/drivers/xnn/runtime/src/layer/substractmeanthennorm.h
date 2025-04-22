#ifndef LAYER_SUBSTRACTMEANTHENNORM_H
#define LAYER_SUBSTRACTMEANTHENNORM_H

#include <string>
#include "layer.h"

namespace xnn {

class SubstractMeanThenNorm: public Layer
{
public:
    SubstractMeanThenNorm();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // param
    // 0=fp32 1=int8 2=fp16
    int top_blob_type;

    Mat mean_data;
    Mat norm_data;

    Mat scale_data;
    Mat bias_data;

    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_SUBSTRACTMEANTHENNORM_H
