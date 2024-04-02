#ifndef LAYER_EMBED_H
#define LAYER_EMBED_H

#include "layer.h"

namespace xnn {

class Embed : public Layer
{
public:
    Embed();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // param
    int num_output;
    int input_dim;
    int bias_term;

    int weight_data_size;

    // model
    Mat weight_data;
    Mat bias_data;
};

} // namespace xnn

#endif // LAYER_EMBED_H
