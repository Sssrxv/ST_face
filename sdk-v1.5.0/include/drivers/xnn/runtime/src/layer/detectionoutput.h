#ifndef LAYER_DETECTIONOUTPUT_H
#define LAYER_DETECTIONOUTPUT_H

#include "layer.h"

namespace xnn {

class DetectionOutput : public Layer
{
public:
    DetectionOutput();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    int num_class;
    float nms_threshold;
    int nms_top_k;
    int keep_top_k;
    float confidence_threshold;
    float variances[4];
};

} // namespace xnn

#endif // LAYER_DETECTIONOUTPUT_H
