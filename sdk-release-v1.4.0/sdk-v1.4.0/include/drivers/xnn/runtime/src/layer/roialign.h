#ifndef LAYER_ROIALIGN_H
#define LAYER_ROIALIGN_H

#include "layer.h"

namespace xnn {

class ROIAlign : public Layer
{
public:
    ROIAlign();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    int pooled_width;
    int pooled_height;
    float spatial_scale;
};

} // namespace xnn

#endif // LAYER_ROIALIGN_H
