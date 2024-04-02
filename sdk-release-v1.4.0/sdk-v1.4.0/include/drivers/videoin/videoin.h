#ifndef _VIDEOIN_H_
#define _VIDEOIN_H_

#include <stdint.h>
#include "frame_mgr.h"
#include "cis_dev_driver.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MAX_CAMERA_NUM (4)

typedef enum videoin_id {
    VIDEOIN_ID_MIPI0,
    VIDEOIN_ID_MIPI1,
    VIDEOIN_ID_MIPI2,
    VIDEOIN_ID_DVPI0,
    VIDEOIN_ID_MAX
} videoin_id_t;


typedef enum videoin_mode {
    VIDEOIN_MODE_CONTINUE,
    VIDEOIN_MODE_ONE_SHOT
} videoin_mode_t;


typedef int (*videoin_event_callback_t)(videoin_id_t videoin_id, void *ctx);

typedef struct capture_config
{
    videoin_id_t    videoin_id;
    int             hoffset;
    int             voffset;
    int             hsize;
    int             vsize;
    int             hstride;
    int             channels;
    frame_fmt_t     fmt; // FAME_FMT_RAW8/FRAME_FMT_RAW10/FRAME_FMT_RAW12
} capture_config_t;

#define VIDEOIN_CONFIG_INITIALIZER          {0}
typedef struct videoin_config {
    videoin_mode_t      mode;
    int                 camera_num;
    frame_stereo_mode_t stereo_mode;
    int                 skip_power_on;
    int                 max_frame_cnt;
    capture_config_t    capture_config[MAX_CAMERA_NUM];
} videoin_config_t;

typedef struct videoin_context
{
    videoin_config_t config;
    volatile int frame_end_status[VIDEOIN_ID_MAX];
} videoin_context_t;


typedef struct videoin_frame
{
    int count; // indicate the valid frame_buf number 1~MAX_CAMERA_NUM
    frame_buf_t *frame_buf[MAX_CAMERA_NUM];
} videoin_frame_t;

videoin_context_t* videoin_init(const videoin_config_t *config);
int videoin_start_stream(videoin_context_t *context);
int videoin_stop_stream(videoin_context_t *context);
int videoin_release(videoin_context_t *context);

#define VIDEOIN_WAIT_FOREVER    (-1)
#define VIDEOIN_NO_WAIT         (0)
int videoin_get_frame(videoin_context_t *context, videoin_frame_t *videoin_frame, int timeout_ms);

int videoin_set_exposure(videoin_id_t videoin_id, cis_exposure_t *exposure_param);
int videoin_register_cis_driver(videoin_id_t videoin_id, cis_dev_driver_t *driver);
int videoin_register_framestart_callback(videoin_id_t videoin_id, videoin_event_callback_t callback, void *ctx);
int videoin_register_frameend_callback(videoin_id_t videoin_id, videoin_event_callback_t callback, void *ctx);
int videoin_wake_cis(videoin_id_t videoin_id);
int videoin_sleep_cis(videoin_id_t videoin_id);

#ifdef __cplusplus
}
#endif

#endif
