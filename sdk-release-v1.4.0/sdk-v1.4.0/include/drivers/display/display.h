#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include "frame_mgr.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef void * display_handle_t;

typedef enum display_interface_type {
    DISPLAY_INTERFACE_TYPE_DVPO,
    DISPLAY_INTERFACE_TYPE_I80,
    DISPLAY_INTERFACE_TYPE_SPI,
    DISPLAY_INTERFACE_TYPE_UNSUPPORT,
} display_interface_type_t;

typedef struct display_frame_param {
    int w;
    int h;
    int format;
} display_frame_param_t;


display_handle_t display_init(display_interface_type_t type, const char *device_name);
int display_release(display_handle_t handle);

int display_start(display_handle_t handle);
int display_stop(display_handle_t handle);

int display_send_frame(display_handle_t handle, frame_buf_t *frame_buf);

int display_get_frame_param(display_handle_t handle, display_frame_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
