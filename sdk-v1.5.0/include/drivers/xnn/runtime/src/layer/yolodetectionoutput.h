#ifndef LAYER_YOLODETECTIONOUTPUT_H
#define LAYER_YOLODETECTIONOUTPUT_H

#include "layer.h"

namespace xnn {

class YoloDetectionOutput : public Layer
{
public:
    YoloDetectionOutput();

    virtual int load_param(const ParamDict& pd);

    virtual int create_pipeline(const Option& opt);
    virtual int destroy_pipeline(const Option& opt);

    virtual int forward_inplace(std::vector<Mat>& bottom_top_blobs, const Option& opt) const;

public:
    int num_class;
    int num_box;
    float confidence_threshold;
    float nms_threshold;
    Mat biases;

    xnn::Layer* softmax;
};

} // namespace xnn

#endif // LAYER_YOLODETECTIONOUTPUT_H
