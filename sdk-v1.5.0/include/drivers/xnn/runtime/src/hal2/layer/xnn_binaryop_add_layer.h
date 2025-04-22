#ifndef __XNN_BINARYOP_ADD_LAYER_H__
#define __XNN_BINARYOP_ADD_LAYER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xnn_binaryop_add_layer_param {
    uint32_t wgt_baseaddr;
    uint32_t input_addr0;
    uint32_t input_addr1;
    uint32_t output_addr;
    int slope_data_offset;
    int bottom_scales_offset;
    int top_scales_offset;
    uint16_t bottom_blob_scale0;
    uint16_t bottom_blob_scale1;
    uint16_t top_blob_scales;
    // 0=none 1=relu 2=leakyrelu 3=clip 4=sigmoid 5=prelu 6=relu6
    uint16_t activation_type;
    int16_t h;
    int16_t w;
    int16_t c;
} xnn_binaryop_add_layer_param_t;

#define XNN_BINARYOP_ADD_LAYER_PARAM_INITIALIZER    \
    (((xnn_binaryop_add_layer_param_t){             \
        .wgt_baseaddr = 0,                          \
        .input_addr0 = 0,                           \
        .input_addr1 = 0,                           \
        .output_addr = 0,                           \
        .slope_data_offset = -1,                    \
        .bottom_scales_offset = -1,                 \
        .top_scales_offset = -1,                    \
        .bottom_blob_scale0 = 0,                    \
        .bottom_blob_scale1 = 0,                    \
        .top_blob_scales = 0,                       \
        .activation_type = 0,                       \
        .h = 0,                                     \
        .w = 0,                                     \
        .c = 0,                                     \
    }))

int xnn_binaryop_add_forward(const xnn_binaryop_add_layer_param_t *param);

#ifdef __cplusplus
}
#endif

#endif //__XNN_BINARYOP_ADD_LAYER_H__
