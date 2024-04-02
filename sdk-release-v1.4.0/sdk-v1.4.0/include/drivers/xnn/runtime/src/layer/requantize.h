#ifndef LAYER_REQUANTIZE_H
#define LAYER_REQUANTIZE_H

#include "layer.h"

namespace xnn {

class Requantize : public Layer
{
public:
    Requantize();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    float scale_in;	// bottom_blob_scale * weight_scale
	float scale_out;// top_blob_scale / (bottom_blob_scale * weight_scale)
    int bias_term;
    int bias_data_size;

    bool fusion_relu;

    Mat bias_data;
};

} // namespace xnn

#endif // LAYER_REQUANTIZE_H
