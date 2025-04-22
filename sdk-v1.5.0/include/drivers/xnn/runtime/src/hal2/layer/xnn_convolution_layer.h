#ifndef __XNN_CONVOLUTION_LAYER_H__
#define __XNN_CONVOLUTION_LAYER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xnn_cnv_layer_param {
    uint32_t wgt_baseaddr;
    uint32_t input_addr;
    uint32_t output_addr;
    uint32_t weight_offset; 
    int batchnorm_a_offset;
    int batchnorm_b_offset;
    int slope_data_offset;
    int top_scales_offset;
    int dequantize_scale_offset;
    uint16_t top_blob_scales;
    uint16_t dequantize_scale;
    // 0=none 1=relu 2=leakyrelu 3=clip 4=sigmoid 5=prelu 6=relu6
    uint16_t activation_type;
    // 0=cnv 1=fc 2=maxpool 3=avgpool 4=dwc32 5=dwc64 6=gc4x4 7=gc8x8
    uint16_t cnv_mode;
    int16_t top_blob_format; //0-int8, 1-nf16
    int16_t oh;
    int16_t ow;
    int16_t oc;
    int16_t ih;
    int16_t iw;
    int16_t ic;
    int16_t in;
    int16_t sw;
    int16_t sh;
    int16_t kw;
    int16_t kh;
    int16_t pw;
    int16_t ph;
    int16_t ic_whole;
    int16_t iw_whole;
    int16_t oc_whole;
    int16_t ow_whole;
    uint16_t bnr_offs;
    uint16_t rlu_offs;
} xnn_cnv_layer_param_t;

#define XNN_CNV_LAYER_PARAM_INITIALIZER             \
    (((xnn_cnv_layer_param_t){                      \
        .wgt_baseaddr = 0,                          \
        .input_addr = 0,                            \
        .output_addr = 0,                           \
        .weight_offset = 0,                         \
        .batchnorm_a_offset = -1,                   \
        .batchnorm_b_offset = -1,                   \
        .slope_data_offset = -1,                    \
        .top_scales_offset = -1,                    \
        .dequantize_scale_offset = -1,              \
        .top_blob_scales = 0,                       \
        .dequantize_scale = 0,                      \
        .activation_type = 0,                       \
        .cnv_mode = 0,                              \
        .top_blob_format = 0,                       \
        .oh = 0,                                    \
        .ow = 0,                                    \
        .oc = 0,                                    \
        .ih = 0,                                    \
        .iw = 0,                                    \
        .ic = 0,                                    \
        .in = 0,                                    \
        .sw = 0,                                    \
        .sh = 0,                                    \
        .kw = 0,                                    \
        .kh = 0,                                    \
        .pw = 0,                                    \
        .ph = 0,                                    \
        .ic_whole = 0,                              \
        .iw_whole = 0,                              \
        .oc_whole = 0,                              \
        .ow_whole = 0,                              \
        .bnr_offs = 0,                              \
        .rlu_offs = 0,                              \
    }))

int xnn_convolution_forward(xnn_cnv_layer_param_t *param);

#ifdef __cplusplus
}
#endif

#endif //__XNN_CONVOLUTION_LAYER_H__
