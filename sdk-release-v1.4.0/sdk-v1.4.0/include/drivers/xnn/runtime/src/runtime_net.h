#ifndef XNN_RUNTIME_NET_H
#define XNN_RUNTIME_NET_H

#include <vector>
#include <string>
#include <stdint.h>
#include "layer.h"
#include "mat.h"

namespace xnn {


class RuntimeExtractor;
class RuntimeNet
{
public:
    RuntimeNet();
    virtual ~RuntimeNet();

    virtual int find_input_blob_index_by_name(const char* blob_name) const;
    virtual int find_output_blob_index_by_name(const char* blob_name) const;
    virtual int load_model(uint32_t wgt_addr, int len) = 0;
    virtual int forward_network(std::vector<xnn::Mat>& _all_blob_mats) = 0;
    virtual int get_workbuffer_size(void) = 0;
    virtual int set_workbuffer(uint32_t addr, int size) = 0;
    int get_input_blob_top_scale(float &top_scale);
    int enable_reg_pool();
    int disable_reg_pool();

    RuntimeExtractor create_extractor();

private:
    int flush_instructions(int pool_idx);
    int add_reg_ops_pool(const std::vector<xnn::xnn_instruction_t>& instructions);

protected:
    friend class RuntimeExtractor;

    int finish_forward();
    int start_reg_pool();
    int flush_reg_pool();

    uint32_t                 _wgt_addr;
    int                      _wgt_len;
    uint32_t                 _workbuffer_baseaddr;
    uint32_t                 _workbuffer_size;
    int                      _all_blob_count;
    std::vector<std::string> _all_blob_names;
    std::vector<int>         _all_blob_idx;
    std::vector<std::string> _out_blob_names;
    std::vector<int>         _out_blob_idx;
    std::vector<std::string> _in_blob_names;
    std::vector<int>         _in_blob_idx;
    float                    _in_blob_top_scale;
    bool _enable_reg_pool;
    bool _has_reg_pooled;
    int  _reg_pool_idx;
    std::vector<void*> _all_instructions_buffers;
    std::vector<int> _all_instructions_buffer_lengths;
    std::vector<xnn::xnn_instruction_t> _temp_instructions_pool;
};


class RuntimeExtractor
{
public:
    virtual ~RuntimeExtractor();

    // set input by blob name
    // return 0 if success
    int input(const char* blob_name, const xnn::Mat& in);

    // set input by blob index
    // return 0 if success
    int input(int blob_index, const xnn::Mat& in);

    // get result by blob name
    // return 0 if success
    int extract(const char* blob_name, xnn::Mat& feat);

    // get result by blob index
    // return 0 if success
    int extract(int blob_index, xnn::Mat& feat);

protected:
    friend RuntimeExtractor RuntimeNet::create_extractor();
    RuntimeExtractor(RuntimeNet* net, int blob_count);

private:
    RuntimeNet* net;
    std::vector<xnn::Mat> blob_mats;
};


} // namespace xnn

#endif // XNN_RUNTIME_NET_H
