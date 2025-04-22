#ifndef LAYER_CHANNELALIGN_H
#define LAYER_CHANNELALIGN_H

#include <stdint.h>
#include "layer.h"

namespace xnn {

class ChannelAlign: public Layer
{
public:
    ChannelAlign();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    void*    workbuffer;
    int      workbuffer_size;

    int align_type; // 0: chw to chw bypass, 1: hwc to hwc channel align
    int output_cn;
    //uint32_t top_addr;
    //int top_size;
    int use_mem_desc;
    xnn_memory_desc_t input_mem_desc;
    xnn_memory_desc_t output_mem_desc;
};

} // namespace xnn

#endif // LAYER_CHANNELALIGN_H
