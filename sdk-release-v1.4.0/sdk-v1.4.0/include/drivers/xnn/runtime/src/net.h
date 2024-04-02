#ifndef XNN_NET_H
#define XNN_NET_H

#include <stdio.h>
#include <vector>
#include "platform.h"
#include "blob.h"
#include "layer.h"
#include "mat.h"
#include "option.h"
#include "layer_registry.h"


#define XNN_CORE_NUM            (2)
#define XNN_DMA_ALIGN_SIZE      (16)
#define XNN_CPU_ALIGN_SIZE      (4)
#define XNN_CACHE_ALIGN_SIZE    (32)
#define XNN_WEIGHT_BUFFER_SIZE  (3*3*16*512)
#define XNN_FEATURE_BUFFER_SIZE (2048 * 16)
#define XNN_MAX_CNV_IC          (512)
#define XNN_MAX_CNV_OC          (512)
#define XNN_MAX_CNV_IH          (512)
#define XNN_MAX_CNV_OH          (512)
#define XNN_MAX_CNV_IW          (512)
#define XNN_MAX_CNV_OW          (512)

namespace xnn {

class OpNode
{
public:
    OpNode()
    {
        _layer = NULL;
        _child_idx = 0;
        _has_processed = 0;
    }
    ~OpNode()
    {
    }
    xnn::Layer *_layer;
    int _has_processed;
    std::vector<OpNode*> _parents;
    std::vector<OpNode*> _childs;
    int _child_idx;
};

class Extractor;
class Net
{
public:
    // empty init
    Net();
    // clear and destroy
    virtual ~Net();

public:
    // option can be changed before loading
    Option opt;

    //// register custom layer by layer type name
    //// return 0 if success
    //int register_custom_layer(const char* type_name, layer_creator_func creator);
    //// register custom layer by layer index
    //// return 0 if success
    //int register_custom_layer(int index, layer_creator_func creator);

#if XNN_STDIO
    // load network structure from plain param file
    // return 0 if success
    int load_param(FILE* fp);
    int load_param(const char* protopath);
    // load network weight data from model file
    // return 0 if success
    int load_model(FILE* fp);
    int load_model(const char* modelpath);

    int load_xparam(FILE* fp);
    int load_xparam(const char* parampath);

    int load_xmodel(FILE* fp);
    int load_xmodel(const char* modelpath);

    int load_xfile(FILE* fp);
    int load_xfile(const char* packpath);
#endif // XNN_STDIO
    int load_xparam(const uint8_t *mem, int len);
    int load_xmodel(const uint8_t *mem, int len);
    int load_xfile(const uint8_t *mem, int len);

    int get_input_blob_top_width_height(int &width, int &height);
    int get_input_blob_top_scale(float &top_scale);
    int get_workbuffer_size(void);
#if XNN_SIM
    int set_workbuffer(void *addr, int size);
#else
    int set_workbuffer(uint32_t addr, int size);
#endif
    int get_mean_value(float mean[3]);
    int get_norm_value(float norm[3]);
    int enable_reg_pool();
    int disable_reg_pool();

    int get_all_blob_names(std::vector<std::string>& blob_names);
    int find_blob_index_by_name(const char* name) const;
    int find_layer_index_by_name(const char* name) const;
    int custom_layer_to_index(const char* type);
    Layer* create_custom_layer(const char* type);
    Layer* create_custom_layer(int index);
    const Layer* get_layer(const char* name) const;

    int forward_layer(int layer_index, std::vector<Mat>& blob_mats, Option& opt) const;

    int forward_layer_one_blob(Layer* layer, std::vector<Mat>& blob_mats, Option& opt);
    int forward_layer_multi_blob(Layer* layer, std::vector<Mat>& blob_mats, Option& opt);
    int forward_network(std::vector<Mat>& blob_mats, Option& opt);

    // construct an Extractor from network
    Extractor create_extractor();

    // unload network structure and weight data
    void clear();

protected:
    int build_graph();
    int build_execute_layers();
    int clean_graph();
    int dfs_forward(OpNode *opnode);
    int set_blob_consumed_status(int blob_idx, int consumer_idx);
    int get_blob_consumed_status(int blob_idx);
    int reset_all_blob_consumed_status();
    int is_setted_as_output_blob(int blob_idx) const;
    int is_setted_as_input_blob(int blob_idx) const;

    int load_xstring(const uint8_t **mem, std::string &str_name);


protected:
    friend class Extractor;

    int set_blob_as_output(const char* name);
    int set_blob_as_output(int blob_index);
    int reset_blob_output_status();

    int set_blob_as_input(const char* name);
    int set_blob_as_input(int blob_index);
    int reset_blob_input_status();

    std::vector<Blob> blobs;
    std::vector<Layer*> layers;
    std::vector<OpNode*> _opnodes;
    std::vector<OpNode*> _input_opnodes;
    std::vector<Layer*> execute_layers;

    std::vector<int> blob_input_status;
    std::vector<int> blob_output_status;

    std::vector<layer_registry_entry> custom_layer_registry;

    const uint8_t *xmodel_mem;
    const uint8_t *xfile_mem;
    void*       weightbuffer;
    int         weight_size;
    void*       workbuffer;
    int         workbuffer_size;

    float       _mean[3];
    float       _norm[3];
};



class Extractor
{
public:
    ~Extractor();

    // enable light mode
    // intermediate blob will be recycled when enabled
    // enabled by default
    void set_light_mode(bool enable);

    // set thread count for this extractor
    // this will overwrite the global setting
    // default count is system depended
    void set_num_threads(int num_threads);

    // set blob memory allocator
    void set_blob_allocator(Allocator* allocator);

    // set workspace memory allocator
    void set_workspace_allocator(Allocator* allocator);

    // enable graph forward mode
    // enabled by default
    void set_graph_forward(bool enable);
#if !XNN_SIM
    // enable out HWC_16N
    // disabled by default
    void set_out_HWC_16N(bool enable);
#endif
    // configure early return for graph forward mode
    // enabled by default
    void set_early_return(bool enable);

    // set input by blob name
    // return 0 if success
    int input(const char* blob_name, const Mat& in);

    // get result by blob name
    // return 0 if success
    int extract(const char* blob_name, Mat& feat);

    // set input by blob index
    // return 0 if success
    int input(int blob_index, const Mat& in);

    // get result by blob index
    // return 0 if success
    int extract(int blob_index, Mat& feat);

protected:
    friend Extractor Net::create_extractor();
    Extractor(Net* net, int blob_count);

private:
    Net* net;
    std::vector<Mat> blob_mats;
    bool graph_forward;
#if !XNN_SIM
    bool out_HWC_16N;
#endif
    Option opt;
};

} // namespace xnn

#endif // XNN_NET_H
