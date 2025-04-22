#ifndef __XNN_CONCAT_LAYER_H__
#define __XNN_CONCAT_LAYER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xnn_concat_layer_input_param {
    uint32_t input_addr;
    int16_t ic;
    uint16_t bottom_blob_scales;
} xnn_concat_layer_input_param_t;

typedef struct xnn_concat_layer_param {
    uint32_t output_addr;
    uint32_t wgt_baseaddr;
    int bottom_scales_offset;
    int top_scales_offset;
    uint16_t top_blob_scales;
    int16_t h;
    int16_t w;
    int16_t oc;
    uint16_t res;
    uint16_t in_num;
    xnn_concat_layer_input_param_t in[0];
} xnn_concat_layer_param_t;

#define XNN_CONCAT_LAYER_PARAM_INITIALIZER          \
    (((xnn_concat_layer_param_t){                   \
        .output_addr = 0,                           \
        .wgt_baseaddr = 0,                          \
        .bottom_scales_offset = -1,                 \
        .top_scales_offset = -1,                    \
        .top_blob_scales = 0,                       \
        .h = 0,                                     \
        .w = 0,                                     \
        .oc = 0,                                    \
        .res = 0,                                   \
        .in_num = 0,                                \
    }))

int xnn_concat_forward(const xnn_concat_layer_param_t *param);

#ifdef __cplusplus
}
#endif

#endif //__XNN_CONCAT_LAYER_H__
