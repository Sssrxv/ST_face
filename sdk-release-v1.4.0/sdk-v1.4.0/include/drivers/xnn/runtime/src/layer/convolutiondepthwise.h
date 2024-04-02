#ifndef LAYER_CONVOLUTIONDEPTHWISE_H
#define LAYER_CONVOLUTIONDEPTHWISE_H

#include "layer.h"

namespace xnn {

class ConvolutionDepthWise : public Layer
{
public:
    ConvolutionDepthWise();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int create_pipeline(const Option& opt);
    virtual int destroy_pipeline(const Option& opt);

    virtual int create_requantize_op(void);

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

    int int8_scale_term;

    // 0=none 1=relu 2=leakyrelu 3=clip 4=sigmoid 5=prelu
    int activation_type;
    Mat activation_params;

    int num_slope; //PRELU

    //batchnorm
    // 0=none 1=fused with convolution 2=normal batchnorm in convolution 3=dequantize batchnorm
    int batchnorm_type;
    int batchnorm_channels;
    float batchnorm_eps;

    // 0=fp32 1=int8 2=fp16
    int top_blob_type;

    // model
    Mat weight_data;
    Mat bias_data;

    Mat weight_data_int8_scales;
    Mat bottom_blob_int8_scales;
    float top_blob_int8_scale;

    //prelu
    Mat slope_data;

    //batch norm
    Mat batchnorm_slope_data;
    Mat batchnorm_mean_data;
    Mat batchnorm_var_data;
    Mat batchnorm_bias_data;

    Mat batchnorm_a_data;
    Mat batchnorm_b_data;

    bool use_int8_inference;
    bool use_int8_requantize;

    std::vector<xnn::Layer*> quantize_ops;
    std::vector<xnn::Layer*> dequantize_ops;
    std::vector<xnn::Layer*> requantize_ops;

    // merge de/requantize op into convolutiondepthwise op
    std::vector<float> dequantize_scales;
    std::vector<float> requantize_scales;

    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_CONVOLUTIONDEPTHWISE_H
