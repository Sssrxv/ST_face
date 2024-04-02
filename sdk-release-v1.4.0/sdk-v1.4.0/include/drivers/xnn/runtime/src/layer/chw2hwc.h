#ifndef LAYER_CHW2HWC_H
#define LAYER_CHW2HWC_H

#include <stdint.h>
#include "layer.h"

namespace xnn {

class CHW2HWC: public Layer
{
public:
    CHW2HWC();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    uint32_t hwc_addr;
    int hwc_size;
};

} // namespace xnn

#endif // LAYER_CHW2HWC_H
