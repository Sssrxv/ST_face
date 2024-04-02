#ifndef __H264_ENCODER_H__
#define __H264_ENCODER_H__
#include "frame_mgr.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define H264_INPUT_WIDTH_MIN      96
#define H264_INPUT_WIDTH_MAX      4080
#define H264_INPUT_HEIGHT_MIN     96
#define H264_INPUT_HEIGHT_MAX     4080

#define H264_GOP_MIN              2
#define H264_GOP_MAX              300
      
#define H264_BITRATE_MAX          60000000


typedef enum {
    H264_ROTATE_0 = 0,
    H264_ROTATE_90R,
    H264_ROTATE_90L,
    H264_ROTATE_180,
    H264_ROTATE_MAX = H264_ROTATE_180
} h264_rotate_type_t;


typedef struct {
    uint16_t w;             // input width
    uint16_t h;             // input height
    uint16_t fps;
    uint16_t gop;
    frame_fmt_t input_type;
    h264_rotate_type_t rotate;
    int32_t  qp_hdr;        // QP for next encoded picture
    uint32_t qp_min;        // Minimum QP for any picture
    uint32_t qp_max;        // Maximum QP for any picture
    uint32_t picture_rc;    // Adjust QP between pictures
    uint32_t mb_rc;         // Adjust QP inside picture
    uint32_t picture_skip;  // Allow rate control to skip pictures
    uint32_t hrd;           // Hypothetical Reference Decoder
    uint32_t bitrate;       // Target bitrate in bits/second, this is needed if picture_rc, mb_rc, picture_skip or hrd is enabled
    uint8_t is_need_crop;
    uint16_t crop_out_width;  // coding width
    uint16_t crop_out_height; // coding height
    int qp_min_delta;
} h264_cfg_t;

#define H264_CFG_PARAM_INITIALIZER              \
    ( (h264_cfg_t)                              \
        {                                       \
            .w = 0,                             \
            .h = 0,                             \
            .fps = 15,                          \
            .gop = 150,                         \
            .input_type = FRAME_FMT_I420,       \
            .rotate = H264_ROTATE_0,            \
            .qp_hdr = 26,                       \
            .qp_min = 10,                       \
            .qp_max = 51,                       \
            .picture_rc = 0,                    \
            .mb_rc = 0,                         \
            .picture_skip = 0,                  \
            .hrd = 0,                           \
            .bitrate = 2000000,                 \
            .is_need_crop = 0,                  \
            .crop_out_width = 0,                \
            .crop_out_height = 0,               \
            .qp_min_delta = 0,                  \
        }                                       \
    )


typedef const void* h264_encoder_t;


typedef enum {
    H264_OK = 0,
    H264_ERROR = -1,
    H264_NULL_ARGUMENT = -2,
    H264_INVALID_ARGUMENT = -3,
    H264_MEMORY_ERROR = -4,
    H264_EWL_ERROR = -5,
    H264_EWL_MEMORY_ERROR = -6,
    H264_INVALID_STATUS = -7,
    H264_OUTPUT_BUFFER_OVERFLOW = -8,
    H264_HW_BUS_ERROR = -9,
    H264_HW_DATA_ERROR = -10,
    H264_HW_TIMEOUT = -11,
    H264_HW_RESERVED = -12,
    H264_SYSTEM_ERROR = -13,
    H264_INSTANCE_ERROR = -14,
    H264_HRD_ERROR = -15,
    H264_HW_RESET = -16

} h264_ret_code_t;


/** 
 * @brief initialize one h264 encoder instance
 * 
 * @param h264_cfg      h264 instance config structure
 * @param h264_encoder  to save the initialized h264 encoder instance
 * 
 * @return H264_OK - OK, other - FAIL
 */
h264_ret_code_t h264_init(h264_cfg_t *h264_cfg, h264_encoder_t *h264_encoder);


/* --------------------------------------------------------------------------*/
/** 
 * @brief release one h264 encoder instance
 * 
 * @param h264_encoder  pointer of initialized h264 encoder instance
 * 
 * @return H264_OK - OK, other - FAIL
 */
/* --------------------------------------------------------------------------*/
h264_ret_code_t h264_release(h264_encoder_t *h264_encoder);


/* --------------------------------------------------------------------------*/
/** 
 * @brief start h264 encoding
 * 
 * @param h264_encoder  pointer of initialized h264 encoder instance
 * @param frame_out     to save the h264 header
 * 
 * @return H264_OK - OK, other - FAIL
 */
/* --------------------------------------------------------------------------*/
h264_ret_code_t h264_start(h264_encoder_t *h264_encoder, frame_buf_t **frame_out);


/* --------------------------------------------------------------------------*/
/** 
 * @brief encode one picture
 * 
 * @param h264_encoder        pointer of initialized h264 encoder instance
 * @param frame_in            input frame 
 * @param frame_out           output frame 
 * 
 * @return H264_OK - OK, other - FAIL
 */
/* --------------------------------------------------------------------------*/
h264_ret_code_t h264_encode(h264_encoder_t *h264_encoder, frame_buf_t *frame_in, frame_buf_t **frame_out);


/* --------------------------------------------------------------------------*/
/** 
 * @brief stop h264 encoding
 * 
 * @param h264_encoder  pointer of initialized h264 encoder instance
 * @param frame_out     to save the h264 tail
 * 
 * @return H264_OK - OK, other - FAIL
 */
/* --------------------------------------------------------------------------*/
h264_ret_code_t h264_stop(h264_encoder_t *h264_encoder, frame_buf_t **frame_out);


#ifdef __cplusplus
}
#endif

#endif // __H264_ENCODER_H__
