#ifndef LAYER_LOG_H
#define LAYER_LOG_H

#include "layer.h"

namespace xnn {

class Log : public Layer
{
public:
    Log();

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    float base;
    float scale;
    float shift;
};

} // namespace xnn

#endif // LAYER_LOG_H
