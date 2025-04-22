#ifndef LAYER_BINARYOP_H
#define LAYER_BINARYOP_H

#include <string>
#include "layer.h"

namespace xnn {

class BinaryOp : public Layer
{
public:
    BinaryOp();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

    enum {
        Operation_ADD   = 0,
        Operation_SUB   = 1,
        Operation_MUL   = 2,
        Operation_DIV   = 3,
        Operation_MAX   = 4,
        Operation_MIN   = 5,
        Operation_POW   = 6,
        Operation_RSUB  = 7,
        Operation_RDIV  = 8
    };

private:
    int binary_op_int8_add_same_size(const std::string &name, const Mat& bottom_blob, const Mat& bottom_blob1, Mat& top_blob, const Option& opt) const;


public:
    // param
    int op_type;
    int with_scalar;
    float b;

    // 0=none 1=relu 2=leakyrelu 3=clip 4=sigmoid 5=prelu
    int activation_type;
    Mat activation_params;

    int num_slope; //PRELU

    //prelu
    Mat slope_data;

    // 0=fp32 1=int8 2=fp16
    int top_blob_type;



    Mat bottom_blob_int8scale_invert_0;
    Mat bottom_blob_int8scale_invert_1;

    std::string prefix_name;


    bool use_int8_inference;
};

} // namespace xnn

#endif // LAYER_BINARYOP_H
