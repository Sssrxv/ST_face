#ifndef __JPEG_ENCODER_H__
#define __JPEG_ENCODER_H__

#include <stdint.h>
#include "frame_mgr.h"
#include "xvic.h"
#include "libjpeg_hardware.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum _jpeg_enc_rotate_t
{
    JPEG_ENC_ROTATE_0     = 0,
    JPEG_ENC_ROTATE_90R   = 1,  /**< Rotate 90 degrees clockwise */
    JPEG_ENC_ROTATE_90L   = 2,  /**< Rotate 90 degrees counter-clockwise */
    JPEG_ENC_ROTATE_180R  = 3,  /**< Rotate 180 degrees clockwise */
    JPEG_ENC_ROTATE_MAX         /**< Invalid Rotate */
} jpeg_enc_rotate_t;

typedef struct _jpeg_proc_param_t
{
    mjpeg_comp_level_t level;
    jpeg_enc_rotate_t rotate;
    int is_need_crop;
    int crop_out_width;
    int crop_out_height;
} jpeg_proc_param_t;

#define JPEG_PROC_PARAM_INITIALIZER      \
    ( ( (jpeg_proc_param_t)              \
    {                                                       \
        .level                       = 11,                  \
        .rotate                      = JPEG_ENC_ROTATE_0,   \
        .is_need_crop                = 0,                   \
        .crop_out_width              = 0,                   \
        .crop_out_height             = 0,                   \
    }                                       \
    )                                       \
    )

/* --------------------------------------------------------------------------*/
/**
 * @brief  jpeg_compress, compress one frame with jpeg
 *
 * @param frame         input frame
 * @param level         compressing level
 *
 * @return   NULL: error
 *
 */
/* --------------------------------------------------------------------------*/
// int jpeg_compress_set_cfg(int framein_width, int framein_height, int frameoutput_width, int frameoutput_height, jpeg_enc_rotate_t rotate, frame_fmt_t fmt, mjpeg_comp_level_t level);

frame_buf_t *jpeg_compress(frame_buf_t *frame_in, jpeg_proc_param_t *pp_param);

#ifdef __cplusplus
}
#endif

#endif // __JPEG_ENCODER_H__
