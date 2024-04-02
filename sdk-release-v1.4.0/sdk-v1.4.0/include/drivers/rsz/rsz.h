#ifndef __RSZ_H__
#define __RSZ_H__

#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct _crop_param_t {
    int crop_w;
    int crop_h;
    int crop_xo;
    int crop_yo;
} crop_param_t;

typedef struct _rsz_param_t {
    int out_w;
    int out_h;
    frame_fmt_t out_fmt;
    int vflip;
    int crop;
    crop_param_t crop_param;
} rsz_param_t;

typedef enum {
    RSZ_YUVsp2YUVsp = 0,
    RSZ_GRAY2GRAY   = 1,
    RSZ_YUVsp2RGBA  = 2,
    RSZ_YUVsp2YUYV  = 3,
    RSZ_YUYV2YUVsp  = 4,
    RSZ_YUYV2YUYV   = 5,
    RSZ_YUYV2RGBA   = 6,
    RSZ_CVT_MAX,
} rsz_cvt_fmt_t;

#define RSZ_PARAM_INITIALIZER       {0}

int rsz_frame_transform(const frame_buf_t *src, rsz_param_t *param, frame_buf_t *dst);

frame_buf_t *rsz_cvt_frame(const frame_buf_t *frame, rsz_param_t *param);

frame_buf_t *rsz_vflip_frame(const frame_buf_t *frame);

frame_buf_t *rsz_crop_frame(const frame_buf_t *frame, int cw, int ch, int x, int y);

frame_buf_t *rsz_cvt_yuv422sp_to_rgba(const frame_buf_t *frame);

frame_buf_t *rsz_resize(const frame_buf_t *frame, int wo, int ho, rsz_cvt_fmt_t cvt);

frame_buf_t *rsz_cvt_raw8_to_yuv422sp(const frame_buf_t *frame);

frame_buf_t *rsz_cvt_yuv_to_raw8(const frame_buf_t *frame);

frame_buf_t *rsz_cvt_yuv422_to_yuv422sp(const frame_buf_t *frame);

frame_buf_t *rsz_cvt_yuv422sp_to_yuv422(const frame_buf_t *frame);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __RSZ_H__
