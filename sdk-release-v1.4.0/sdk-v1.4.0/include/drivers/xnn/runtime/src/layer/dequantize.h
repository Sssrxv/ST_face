#ifndef LAYER_DEQUANTIZE_H
#define LAYER_DEQUANTIZE_H

#include "layer.h"

namespace xnn {

class Dequantize : public Layer
{
public:
    Dequantize();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float scale;
    int bias_term;
    int bias_data_size;
    //0:calculate in fp32 1:calculate in fp16
    int calc_in_fp16;

    Mat bias_data;
};

} // namespace xnn

#endif // LAYER_DEQUANTIZE_H
