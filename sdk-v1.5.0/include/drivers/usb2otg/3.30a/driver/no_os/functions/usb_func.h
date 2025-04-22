#ifndef _DRIVER_USB_FUNC_H_
#define _DRIVER_USB_FUNC_H_

#include <stdint.h>
#include <stddef.h>
#include "no_os_defs.h"


#ifdef __cplusplus
extern "C" {
#endif

int usb_device_has_inited(void);
int usb_device_init(struct usb_gadget_driver *driver);
int usb_device_cleanup();


#ifdef __cplusplus
}
#endif


#endif
