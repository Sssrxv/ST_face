#ifndef XNN_OPTION_H
#define XNN_OPTION_H

#include "platform.h"

namespace xnn {

class Allocator;
class Option
{
public:
    // default option
    Option();

public:
    // blob memory allocator
    Allocator* blob_allocator;

    // workspace memory allocator
    Allocator* workspace_allocator;

    // light mode
    // intermediate blob will be recycled when enabled
    // enabled by default
    bool lightmode;

    // thread count
    // default value is the one returned by get_cpu_count()
    int num_threads;

    // enable winograd convolution optimization
    // improve convolution 3x3 stride1 performace, may consume more memory
    // changes should be applied before loading network structure and weight
    // enabled by default
    bool use_winograd_convolution;

    // enable sgemm convolution optimization
    // improve convolution 1x1 stride1 performace, may consume more memory
    // changes should be applied before loading network structure and weight
    // enabled by default
    bool use_sgemm_convolution;

    // enable quantized int8 inference
    // use low-precision int8 path for quantized model
    // changes should be applied before loading network structure and weight
    // enabled by default
    bool use_int8_inference;
    bool use_int8_storage;
    bool use_int8_arithmetic;

    // for graph forward mode, set this option to true will stop inference
    // the time the extract layer is calculated. if this option is false, the
    // extractor will run the whole graph even the extract layer is middle node
    // in the graph
    bool early_return;
};

} // namespace xnn

#endif // XNN_OPTION_H
