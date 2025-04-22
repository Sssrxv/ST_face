#ifndef LAYER_NORMALIZE_H
#define LAYER_NORMALIZE_H

#include "layer.h"

namespace xnn {

class Normalize : public Layer
{
public:
    Normalize();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    // param
    int across_spatial;
    int across_channel;
    int channel_shared;
    float eps;
    int scale_data_size;

    Mat scale_data;
};

} // namespace xnn

#endif // LAYER_NORMALIZE_H
