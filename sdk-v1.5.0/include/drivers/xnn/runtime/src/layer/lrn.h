#ifndef LAYER_LRN_H
#define LAYER_LRN_H

#include "layer.h"

namespace xnn {

class LRN : public Layer
{
public:
    LRN();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

    enum { NormRegion_ACROSS_CHANNELS = 0, NormRegion_WITHIN_CHANNEL = 1 };

public:
    // param
    int region_type;
    int local_size;
    float alpha;
    float beta;
    float bias;
};

} // namespace xnn

#endif // LAYER_LRN_H
