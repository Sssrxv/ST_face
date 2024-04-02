#ifndef __UVC_CAMERA_H__
#define __UVC_CAMERA_H__

#include <stdint.h>
#include <stdbool.h>
#include "frame_mgr.h"
#include "no_os_defs.h"

#include "uvc_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

struct usb_gadget_driver *usb_func_uvc_camera(void);

typedef struct _f_uvc_camera_t {
    struct usb_gadget_driver *driver;
    int ifc;
    int cfg;
	/* EP1-IN, ctrl status interrupt ep */
    struct usb_ep *ctrl_stat_in_ep;
    uint8_t       *ctrl_stat_in_buf;
    usb_request_t *ctrl_stat_in_req;
	/* EP2-IN, video streaming bulk-in ep */
    struct usb_ep *video_in_ep;
    usb_request_t *video_in_req;
	/* EP3-OUT, external command bulk-out ep*/
    struct usb_ep *ext_cmd_out_ep;
    uint8_t       *ext_cmd_out_buf;
    usb_request_t *ext_cmd_out_req;
	/* EP3-IN, external response bulk-in ep*/
    struct usb_ep *ext_resp_in_ep;
    uint8_t       *ext_resp_in_buf;
    usb_request_t *ext_resp_in_req;
} f_uvc_camera_t;

#ifdef __cplusplus
}
#endif

#endif // __UVC_CAMERA_H__
