#ifndef __JPEG_ENCODER_CTL_H__
#define __JPEG_ENCODER_CTL_H__

#include "jpeg_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

int jpeg_encoder_ctl_init(void);

int jpeg_encoder_ctl_cfg(frame_buf_t *frame, jpeg_proc_param_t *pp_param);

int jpeg_encoder_ctl_process(frame_buf_t *frame_in, frame_buf_t **frame_out);

int jpeg_encoder_ctl_release(void);


#ifdef __cplusplus
}
#endif

#endif //__JPEG_ENCODER_CTL_H__
