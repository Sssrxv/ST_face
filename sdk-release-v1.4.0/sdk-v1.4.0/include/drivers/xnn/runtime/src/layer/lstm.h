#ifndef LAYER_LSTM_H
#define LAYER_LSTM_H

#include "layer.h"

namespace xnn {

class LSTM : public Layer
{
public:
    LSTM();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    // param
    int num_output;
    int weight_data_size;

    // model
    Mat weight_hc_data;
    Mat weight_xc_data;
    Mat bias_c_data;
};

} // namespace xnn

#endif // LAYER_LSTM_H
