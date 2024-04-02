#ifndef __USB_EP0_H__
#define __USB_EP0_H__

#include <stdint.h>
#include <stdbool.h>

#include "no_os_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ep0_receive_state {
    EP0_RECEIVE_UVC_PROBE_CTRL = 0,
    EP0_RECEIVE_UVC_COMMIT_CTRL,
    EP0_RECEIVE_UVC_BRIGHTNESS,
    EP0_RECEIVE_UVC_BACKLIGHT_COMPENSATION,
    EP0_RECEIVE_UAC_VOICE_MUTE,
    EP0_RECEIVE_UAC_VOICE_VOLUME,
    EP0_RECEIVE_UAC_VOICE_RES,
    EP0_RECEIVE_UAC_AUTOMATIC_GAIN,
    EP0_RECEIVE_UAC_SAMPLE_RATE,
    EP0_RECEIVE_NULL,
} ep0_receive_state_t;

typedef struct {
    ep0_receive_state_t recv_state;
    void *recv_buf;
} ep0_recv_t;

int dwc_usb_ep0_wait_nrdy(struct usb_gadget *gadget);

int dwc_usb_ep0_send_data(struct usb_gadget *gadget, void *data, int bytes);

int dwc_usb_ep0_recv_data(struct usb_gadget *gadget, void *buf, int bytes);

int dwc_usb_ep0_recv_data_with_state_change(struct usb_gadget *gadget, void *buf, int bytes, ep0_receive_state_t recv_state);

#ifdef __cplusplus
}
#endif

#endif // __USB_EP0_H__
