#ifndef LAYER_RNN_H
#define LAYER_RNN_H

#include "layer.h"

namespace xnn {

class RNN : public Layer
{
public:
    RNN();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    // param
    int num_output;
    int weight_data_size;

    // model
    Mat weight_hh_data;
    Mat weight_xh_data;
    Mat weight_ho_data;
    Mat bias_h_data;
    Mat bias_o_data;
};

} // namespace xnn

#endif // LAYER_RNN_H
