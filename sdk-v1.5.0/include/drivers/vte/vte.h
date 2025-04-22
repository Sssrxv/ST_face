#ifndef __VTE_H__
#define __VTE_H__

#include "vte_ctrl.h"
#include "vte_descriptor.h"
#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

int fast_detect_init(vte_fast_param_t *pparam);
int fast_detect_calc(frame_buf_t *frame, fast_pt_hdr_t *fast_points, uint32_t *points_cnt);
int fast_detect_release(void);

int fast_detect_calc_output_pyr_images(frame_buf_t *frame, frame_buf_t *pyr_frames[], int pyr_cnt, fast_pt_hdr_t *fast_points, uint32_t *points_cnt);
int fast_detect_free_pyr_frames(frame_buf_t *pyr_frames[], int count);

fast_pt_hdr_t *fast_alloc_points(int points_cnt);
int fast_free_points(fast_pt_hdr_t *ptr);

int fast_detect_and_extract_feature(frame_buf_t *frame, fast_pt_hdr_t *fast_points, vte_descriptor_t *descriptors, int *points_cnt);

#ifdef __cplusplus
}
#endif

#endif // __VTE_H__
