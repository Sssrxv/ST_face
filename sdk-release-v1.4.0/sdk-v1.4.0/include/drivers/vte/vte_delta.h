#ifndef _VTE_DELTA_H_
#define _VTE_DELTA_H_

#include "vte.h"
#include "vte_match.h"

#ifdef __cplusplus
extern "C" {
#endif


int vte_init_get_mean_delta_y(vte_fast_param_t *pparam);
int vte_get_mean_delta_y(frame_buf_t *frame_left, frame_buf_t *frame_right, float *delta_y, int show_pair, frame_buf_t **frame_show);
int vte_deinit_get_mean_delta_y(void);


#ifdef __cplusplus
}
#endif


#endif
