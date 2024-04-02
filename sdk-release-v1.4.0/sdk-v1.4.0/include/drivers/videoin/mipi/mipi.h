#ifndef _DRIVER_MIPI_H
#define _DRIVER_MIPI_H

#include <stdint.h>
#include "mipi_types.h"
#include "frame_mgr.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VC_NUM (3)

typedef int (*mipi_event_callback_t)(mipi_id_t mipi_id, void *ctx);

typedef struct _mipi_frame_buf_t
{
    int                 h_offset;
    int                 v_offset;
    int                 h_size;
    int                 v_size;
    int                 channels;
    int                 stride[3];
    uint8_t             *data[3];  // for MIPI_MODE_CONTINUE
    frame_fmt_t         fmt;
} mipi_frame_buf_t;


typedef enum _mipi_mode_t {
    MIPI_MODE_CONTINUE,
    MIPI_MODE_ONE_SHOT
} mipi_mode_t;


typedef struct _mipi_config_t
{
    mipi_mode_t             mode;
    int                     enable_interrupt;
    int                     freq;           // mipi frequency in M bitrate
    int                     lane_num;       // 1 or 2
    int                     vc_num;         // 1~3
    mipi_virtual_channel_t  virtual_channels[MAX_VC_NUM];
    mipi_data_type_t        data_type;
    mipi_frame_buf_t        frame_buf0;
    mipi_frame_buf_t        frame_buf1;
} mipi_config_t;

int mipi_init(mipi_id_t mipi_id, const mipi_config_t *config);
int mipi_start(mipi_id_t mipi_id);
int mipi_stop(mipi_id_t mipi_id);
int mipi_release(mipi_id_t mipi_id);
int mipi_register_framestart_callback(mipi_id_t mipi_id, mipi_event_callback_t callback, void *ctx);
int mipi_register_frameend_callback(mipi_id_t mipi_id, mipi_event_callback_t callback, void *ctx);
int mipi_wait_frame_end(mipi_id_t mipi_id, mipi_wdma_channel_t channel, int timeout_ms);
int mipi_is_current_buffer_same_with(mipi_id_t mipi_id, const mipi_frame_buf_t *frame_buf);
int mipi_set_next_frame_buffer(mipi_id_t mipi_id, const mipi_frame_buf_t *frame_buf);
int mipi_set_one_shot_buffer(mipi_id_t mipi_id, const mipi_frame_buf_t *frame_buf);

void mipi_print_ris(mipi_id_t mipi_id);
void mipi_clear_interrupt(mipi_id_t mipi_id);



#ifdef __cplusplus
}
#endif

#endif
