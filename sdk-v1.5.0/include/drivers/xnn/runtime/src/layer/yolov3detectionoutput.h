#ifndef LAYER_YOLOV3DETECTIONOUTPUT_H
#define LAYER_YOLOV3DETECTIONOUTPUT_H

#include "layer.h"

namespace xnn {

class Yolov3DetectionOutput : public Layer
{
public:
	Yolov3DetectionOutput();
    ~Yolov3DetectionOutput();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    int num_class;
    int num_box;
    float confidence_threshold;
    float nms_threshold;
    Mat biases;
	Mat mask;
	Mat anchors_scale;
	int mask_group_num;
    xnn::Layer* softmax;
};

} // namespace xnn

#endif // LAYER_YOLODETECTIONOUTPUT_H
