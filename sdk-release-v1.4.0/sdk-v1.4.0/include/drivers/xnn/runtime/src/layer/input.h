#ifndef LAYER_INPUT_H
#define LAYER_INPUT_H

#include "layer.h"

namespace xnn {

class Input : public Layer
{
public:
    Input();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    int w;
    int h;
    int c;
    int uint8_input;
};

} // namespace xnn

#endif // LAYER_INPUT_H
