#ifndef LAYER_XPE_H
#define LAYER_XPE_H

#include "layer.h"

namespace xnn {

#define XPE_MBDATA_TAG0 (0x2A3B4C5D)
#define XPE_MBDATA_TAG1 (0x91817161)

// xnn program engine layer execute on xnn core
class XPE : public Layer
{
public:
    XPE();

    virtual int load_param(const ParamDict& pd);

    virtual int load_model(const ModelBin& mb);

    virtual int create_pipeline(const Option& opt);
    virtual int destroy_pipeline(const Option& opt);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const override;
    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt);

private:
    int load_memory_descs(const ModelBin& mb, std::vector<xnn_memory_desc_t> &mem_descs);
    int load_instruction_descs(const ModelBin& mb, std::vector<xnn_instruction_desc_t> &instruction_descs);

    int find_xnn_memory_desc(const std::string &mem_name, xnn_memory_desc_t &mem_desc) const;

public:
    void*    weightbuffer;
    int      weightbuffer_size;
    void*    workbuffer;
    int      workbuffer_size;

public:
    mutable std::vector<xnn_memory_desc_t>      input_memory_descs;
    mutable std::vector<xnn_memory_desc_t>      output_memory_descs;
    mutable std::vector<xnn_memory_desc_t>      local_memory_descs;
    mutable std::vector<xnn_instruction_desc_t> instruction_descs;
};

} // namespace xnn

#endif // LAYER_XPE_H
