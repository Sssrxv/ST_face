#ifndef LAYER_PADDING_H
#define LAYER_PADDING_H

#include "layer.h"

namespace xnn {

class Padding : public Layer
{
public:
    Padding();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

public:
    // -233 = dynamic offset from reference blob
    int top;
    int bottom;
    int left;
    int right;
    int type;// 0=BORDER_CONSTANT 1=BORDER_REPLICATE
    float value;
};

} // namespace xnn

#endif // LAYER_PADDING_H
