#ifndef LAYER_HWC2CHW_H
#define LAYER_HWC2CHW_H

#include <stdint.h>
#include "layer.h"

namespace xnn {

class HWC2CHW: public Layer
{
public:
    HWC2CHW();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    uint32_t hwc_addr;
    int hwc_size;
};

} // namespace xnn

#endif // LAYER_HWC2CHW_H
