#ifndef XNN_LAYER_H
#define XNN_LAYER_H

#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include "platform.h"
#include "mat.h"
#include "modelbin.h"
#include "option.h"
#include "paramdict.h"
#include "layer_type.h"

namespace xnn {

typedef struct _xnn_instruction_t {
    uint8_t rbc_cmd[8];
} __attribute__((packed, aligned(4))) xnn_instruction_t;

typedef enum _xnn_op_t
{
    XNN_OP_NOP,
    XNN_OP_WR,
    XNN_OP_WR_ADDR,
    XNN_OP_RD,
    XNN_OP_MEM_ALLOC,
    XNN_OP_MEM_FREE,
} xnn_op_t;

typedef struct _xnn_instruction_desc_t
{
    xnn_op_t op;
    uint32_t reg_addr;
    uint32_t reg_val;
    std::string base_addr_str;
    int mem_size;
} xnn_instruction_desc_t;


typedef struct _xnn_memory_desc_t
{
    std::string name;
    bool     is_fixed_addr;
    uint32_t addr;
    uint32_t size;
    int      dims;
    int      w;
    int      h;
    int      c;
    int      cn;
    int      elemsize;
#if XNN_SIM
    void*    addr_ptr;
#endif
} xnn_memory_desc_t;


class Layer
{
public:
    // empty
    Layer();
    // virtual destructor
    virtual ~Layer();

    // load layer specific parameter from parsed dict
    // return 0 if success
    virtual int load_param(const ParamDict& pd);

    // load layer specific weight data from model binary
    // return 0 if success
    virtual int load_model(const ModelBin& mb);

    // layer implementation specific setup
    // return 0 if success
    virtual int create_pipeline(const Option& opt = Option());

    // layer implementation specific clean
    // return 0 if success
    virtual int destroy_pipeline(const Option& opt = Option());

public:
    // one input and one output blob
    bool one_blob_only;

    // support inplace inference
    bool support_inplace;

    // accept input blob with packed storage
    bool support_packing;

public:
    // implement inference
    // return 0 if success
    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt = Option()) const;
    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt = Option()) const;

    // implement inplace inference
    // return 0 if success
    virtual int forward_inplace(std::vector<Mat>& bottom_top_blobs, const Option& opt = Option()) const;
    virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt = Option()) const;

public:
    // layer type index
    int typeindex;
    LayerType::LayerType layer_type;
    // layer index in the net layers array
    int index;
    // layer type name
    std::string type;
    std::string old_type;
    // layer name
    std::string name;
    // blob index which this layer needs as input
    std::vector<int> bottoms;
    Mat top_blob_scales;
    Mat bottom_blob_scales;
    // blob index which this layer produces as output
    std::vector<int> tops;
    // xnn hw instructions buffer.
    std::vector<xnn_instruction_t> instructions;
};


} // namespace xnn 

#endif // XNN_LAYER_H
