#ifndef LAYER_SIGMOID_H
#define LAYER_SIGMOID_H

#include "layer.h"
#include <vector>
#include <string>

namespace xnn {

class Sigmoid : public Layer
{
public:
    Sigmoid();

    virtual int create_pipeline(const Option& opt);
    virtual int destroy_pipeline(const Option& opt);

    virtual int load_param(const ParamDict& pd);

    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt) const;

public:
    // param
    // 0=fp32 1=int8 2=fp16
    int top_blob_type; // TODO use enum
    std::string prefix_name; // TODO move to xnn::Layer?

private:
    std::vector<unsigned short> lookup_table;
};

} // namespace xnn

#endif // LAYER_SIGMOID_H
