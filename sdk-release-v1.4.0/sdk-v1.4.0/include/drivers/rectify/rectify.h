#ifndef __RECTIFY_H__
#define __RECTIFY_H__

#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float rec_type_t;

typedef struct _vde_cam_param_t {
    uint32_t img_w;
    uint32_t img_h;
    rec_type_t  u0;
    rec_type_t  v0;
    rec_type_t  fx;
    rec_type_t  fy;
    rec_type_t  k1;
    rec_type_t  k2;
    rec_type_t  p1;
    rec_type_t  p2;
    rec_type_t  r0;
    rec_type_t  r1;
    rec_type_t  r2;
    rec_type_t  r3;
    rec_type_t  r4;
    rec_type_t  r5;
    rec_type_t  r6;
    rec_type_t  r7;
    rec_type_t  r8;
} __attribute__((packed, aligned(4))) vde_cam_param_t;

typedef struct _vde_q_param_t {
    rec_type_t cx;
    rec_type_t cy;
    rec_type_t f;
    rec_type_t invTx;
    rec_type_t scale;  // depth scale factor
} vde_q_param_t;

//frame_buf_t *rectify_mono_frame_roi(frame_buf_t *frame, vde_cam_param_t *cam_param, frame_roi_t *roi);
int rectify_mono_frame_roi(frame_buf_t *frame, vde_cam_param_t *cam_param, frame_roi_t *roi, frame_buf_t *frame_rec, int crop);

frame_buf_t *rectify_mono_frame(frame_buf_t *frame, vde_cam_param_t *cam_param);

int rectify_stereo_frame_roi(frame_buf_t *frame, 
        vde_cam_param_t *cam_param_l, 
        vde_cam_param_t *cam_param_r,
        frame_roi_t *roi,
        frame_buf_t *frame_rec,
        int crop
        );

frame_buf_t *rectify_stereo_frame(frame_buf_t *frame, 
        vde_cam_param_t *param_l, 
        vde_cam_param_t *param_r
        );

int rectify_stereo_update_param(vde_cam_param_t *param_l, vde_cam_param_t *param_r);

#ifdef __cplusplus
}
#endif

#endif // __RECTIFY_H__
