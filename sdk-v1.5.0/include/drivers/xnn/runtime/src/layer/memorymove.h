#ifndef LAYER_MEMORYMOVE_H
#define LAYER_MEMORYMOVE_H

#include <stdint.h>
#include "layer.h"

namespace xnn {

class MemoryMove: public Layer
{
public:
    MemoryMove();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    int align_type; // 0: chw to chw bypass, 1: hwc to hwc channel align

    // default move c
    Mat sub_channels;
    Mat sub_aligned_channels;
};

} // namespace xnn

#endif // LAYER_MEMORYMOVE_H
