#ifndef _FRAME_MGR_H_
#define _FRAME_MGR_H_

#include "xlist.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif // USE_RTOS
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _frame_fmt_t
{
    FRAME_FMT_YUV422_PLANAR = 0,
    FRAME_FMT_YUV422_SEMIPLANAR,
    FRAME_FMT_YUYV,
    FRAME_FMT_RGB565,
    FRAME_FMT_RGB666,
    FRAME_FMT_RGB888,
    FRAME_FMT_BGR888,
    FRAME_FMT_RGBA8888,
    FRAME_FMT_RAW8,
    FRAME_FMT_RAW10,
    FRAME_FMT_RAW12,
    FRAME_FMT_RAW16,
    FRAME_FMT_RAW32,
    FRAME_FMT_MJPEG,
	FRAME_FMT_I420, 		// YUV420 planar, 		YYYY, U, V
	FRAME_FMT_YV12, 		// YUV420 planar, 		YYYY, V, U
	FRAME_FMT_NV21, 		// YUV420 semi-planar, 	YYYY, VU
	FRAME_FMT_NV12, 		// YUV420 semi-planar, 	YYYY, UV
    FRAME_FMT_H264,
    FRAME_FMT_MAX,
} frame_fmt_t;

typedef enum frame_stereo_mode {
    FRAME_STEREO_NONE = 0,          // not stereo, mono frame
    FRAME_STEREO_H_CONCAT = 1,      // stereo frame, horizontal concat
    FRAME_STEREO_V_CONCAT = 2,      // stereo frame, vertical concat
} frame_stereo_mode_t;

#define FRAME_BUF_MAGIC         (0x6672616d)
#define FRAME_ADDR_ALIGN_SIZE   (64)

#define FRAME_GET_DATA_PTR(p)   ((p) - FRAME_ADDR_ALIGN_SIZE)

typedef struct _frame_buf_t {
    uint32_t magic;
    uint8_t *data[3];
    int      stride[3];   // in bytes
    int      fmt;         // frame format
    int      width;       // in pixels
    int      height;      // in pixels
    int      channels;    // 1~3

    int      used_bytes;  // currently used bytes of data buffers
    int      buf_bytes;   // buffer total size in bytes;
    int      is_continous;// continous memory allocation
    int      stereo_mode; // stereo concat mode if stereo-frame
    uint32_t frame_index;
    //private
    int      _ownbuf;
    int      _refcnt;

#ifdef USE_RTOS
    pthread_mutex_t mutex;
#endif
    void *mem_blk_pool_hdl[3];  // Memory block pool handle. 
                                // The memory is allacated from system if handle is NULL
    struct list_head list;
} frame_buf_t;

#define SET_STRIDE_EN         (1)

typedef struct _frame_param_t {
    int      fmt;
    int      width;
    int      height;
    int      set_stride;
    int      stride[3];
    int      continous_alloc;
    int      stereo_mode;
    int      frame_index;
    void    *mem_blk_pool_hdl[3];
} frame_param_t;

#define FRAME_PARAM_INITIALIZER             \
    ( ( (frame_param_t)                     \
    {                                       \
        .fmt    = FRAME_FMT_MAX,            \
        .width  = 0,                        \
        .height = 0,                        \
        .set_stride = 0,                    \
        .stride = {0},                      \
        .continous_alloc = 1,               \
        .stereo_mode = FRAME_STEREO_NONE,   \
        .frame_index = 0,                   \
        .mem_blk_pool_hdl = {NULL},         \
    }                                       \
    )                                       \
    )

typedef struct _frame_roi_t {
    int     x;
    int     y;
    int     w;
    int     h;
} frame_roi_t;


int          frame_mgr_init(void);
int          frame_mgr_release(void);
frame_buf_t* frame_mgr_alloc_multi_channels_gray(void *mem_pool_hdl[3], int width, int height, int channels, int fmt, int *stride, int stereo_mode);
frame_buf_t* frame_mgr_alloc_one(int width, int height, int fmt, int *stride);
frame_buf_t* frame_mgr_get_one(const frame_param_t *param);
int          frame_mgr_free_one(frame_buf_t **frame_pp);
int          frame_mgr_incr_ref(frame_buf_t *frame_buf);
int          frame_mgr_decr_ref(frame_buf_t *frame_buf);

void frame_mgr_inval_cache(frame_buf_t *frame_buf);
void frame_mgr_flush_cache(frame_buf_t *frame_buf);

int init_frame_from_buffer(frame_buf_t *frame_buf, int width, int height, int fmt, uint8_t *buffer[], int *stride, bool flush_cache);

void mem_stride_copy(void *dst, void *src, 
        size_t dst_x_stride, size_t src_x_stride,
        size_t bytes_per_pixel,
        size_t dst_sx, size_t dst_sy, 
        size_t src_sx, size_t src_sy,
        size_t copy_w, size_t copy_h
        );

void frame_stride_copy(frame_buf_t *frame_dst, frame_buf_t *frame_src, 
        size_t dst_sx, size_t dst_sy, 
        size_t src_sx, size_t src_sy,
        size_t copy_w, size_t copy_h
        );

frame_buf_t *frame_mgr_clone(const frame_buf_t *frame);

void *frame_mgr_mem_pool_create(const char *name, uint32_t blk_size, uint32_t blk_cnt, uint32_t addr_align);
void *frame_mgr_get_mem_pool_hdl(const char *name);
void *frame_mgr_mem_pool_get(void *pool_handle, uint32_t bytes);
void  frame_mgr_mem_pool_put(void *pool_handle, void *ptr);
int   frame_mgr_mem_pool_free_blk_cnt(void *pool_handle);
int   frame_mgr_mem_pool_used_blk_cnt(void *pool_handle);
void  frame_mgr_mem_pool_destroy(void *pool_handle);

#ifdef __cplusplus
}
#endif

#endif
