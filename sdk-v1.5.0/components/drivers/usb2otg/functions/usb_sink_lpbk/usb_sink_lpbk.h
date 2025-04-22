#ifndef __USB_SINK_LPBK_H__
#define __USB_SINK_LPBK_H__

#include "no_os_defs.h"

#define TEST_ISOC

#ifdef __cplusplus
extern "C" {
#endif

struct usb_gadget_driver *usb_func_loopback(void);

typedef struct _f_loopback {
    struct usb_gadget_driver *driver;
    int ifc;
    int cfg;
    int sink;
    struct usb_ep *ep_in;
    uint8_t       *ep_in_buf;
    usb_request_t *ep_in_req;
    struct usb_ep *ep_out;
    uint8_t       *ep_out_buf;
    usb_request_t *ep_out_req;
} f_loopback_t;


#ifdef __cplusplus
}    
#endif

#endif // __USB_SINK_LPBK_H__
