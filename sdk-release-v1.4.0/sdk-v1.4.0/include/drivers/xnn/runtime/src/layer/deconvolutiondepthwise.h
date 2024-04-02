#ifndef LAYER_DECONVOLUTIONDEPTHWISE_H
#define LAYER_DECONVOLUTIONDEPTHWISE_H

#include "layer.h"

namespace xnn {

class DeconvolutionDepthWise : public Layer
{
public:
    DeconvolutionDepthWise();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // param
    int num_output;
    int kernel_w;
    int kernel_h;
    int dilation_w;
    int dilation_h;
    int stride_w;
    int stride_h;
    int pad_w;
    int pad_h;
    int bias_term;

    int weight_data_size;
    int group;

    // 0=none 1=relu 2=leakyrelu 3=clip 4=sigmoid
    int activation_type;
    Mat activation_params;

    // model
    Mat weight_data;
    Mat bias_data;
};

} // namespace xnn

#endif // LAYER_DECONVOLUTIONDEPTHWISE_H
