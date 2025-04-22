#ifndef LAYER_INTERP_H
#define LAYER_INTERP_H

#include "layer.h"

namespace xnn {

class Interp : public Layer
{
public:
    Interp();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat &bottom_blob, Mat &top_blob, const Option& opt) const;

public:
    // param
    int resize_type;//1=nearest  2=bilinear  3=bicubic
    float width_scale;
    float height_scale;
    int output_width;
    int output_height;

    std::string prefix_name;
};

} // namespace xnn

#endif // LAYER_INTERP_H
