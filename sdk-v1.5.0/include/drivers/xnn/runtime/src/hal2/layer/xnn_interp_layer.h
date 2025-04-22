#ifndef __XNN_INTERP_LAYER_H__
#define __XNN_INTERP_LAYER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xnn_interp_layer_param {
    uint32_t input_addr;
    uint32_t output_addr;
    int16_t oh;
    int16_t ow;
    int16_t oc;
    int16_t ih;
    int16_t iw;
    int16_t ic;
    int16_t ups_sw;
    int16_t ups_sh;
    // 0: duplicate 1: set 0
    int16_t upsample_mode;
} xnn_interp_layer_param_t;

#define XNN_INTERP_LAYER_PARAM_INITIALIZER          \
    (((xnn_interp_layer_param_t){                   \
        .input_addr = 0,                            \
        .output_addr = 0,                           \
        .oh = 0,                                    \
        .ow = 0,                                    \
        .oc = 0,                                    \
        .ih = 0,                                    \
        .iw = 0,                                    \
        .ic = 0,                                    \
        .ups_sw = 0,                                \
        .ups_sh = 0,                                \
        .upsample_mode = 0,                         \
    }))

int xnn_interp_forward(const xnn_interp_layer_param_t *param);

#ifdef __cplusplus
}
#endif

#endif //__XNN_INTERP_LAYER_H__
