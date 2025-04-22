#ifndef __UVC_ISOC_AUDIO_H__
#define __UVC_ISOC_AUDIO_H__

#include <stdint.h>
#include <stdbool.h>

#include "uvc_comm.h"
#include "uac_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _f_uac_isoc_spk_t {
    int interface_alt;
	/* EP3-OUT, audio streaming iosc-out ep */
    bool ep_enabled;
    struct usb_ep *audio_spk_out_ep;
    usb_endpoint_descriptor_t *audio_spk_out_ep_desc;
    usb_request_t *audio_spk_out_req;
    uint8_t       *audio_spk_out_buf;
} f_uac_isoc_spk_t;

typedef struct _f_uac_isoc_mic_t {
    int interface_alt;
  	/* EP4-IN, audio streaming iosc-in ep */
    bool ep_enabled;
    struct usb_ep *audio_mic_in_ep;
    usb_endpoint_descriptor_t *audio_mic_in_ep_desc;
    usb_request_t *audio_mic_in_req;
    uint8_t       *audio_mic_in_buf;
} f_uac_isoc_mic_t;

typedef enum audio_aec_state {
    AUDIO_AEC_AUTO = 0x00,
    AUDIO_AEC_FORCE_ENABLE = 0x91,
    AUDIO_AEC_FORCE_DISABLE = 0x92,
} audio_aec_state_t;

typedef struct _uac_param_t {
    uint32_t uac_enable;
    uint32_t sample_rate;
    uint32_t max_payload_size;
    uint32_t transfer_interval;
    uint32_t enable_transfer_interval_us;
    uint32_t transfer_interval_us;
    // use bcdDevice to decide enable of audio AEC, this is only suggest to host
    uint8_t aec_state; // audio_aec_state_t
} uac_param_t;

#define UAC_PARAM_INITIALIZER             \
    ( ( (uac_param_t)                     \
    {                                     \
        .uac_enable = 1,                  \
        .sample_rate = 16000,             \
        .max_payload_size = 32,           \
        .transfer_interval = 4,           \
        .enable_transfer_interval_us = 0, \
        .transfer_interval_us = 1000,     \
        .aec_state      = AUDIO_AEC_AUTO, \
    }                                     \
    )                                     \
    )

int uac_spk_req_buf_init(struct usb_gadget *gadget, struct usb_ep *ep, usb_endpoint_descriptor_t *desc);

int uac_mic_req_buf_init(struct usb_gadget *gadget, struct usb_ep *ep, usb_endpoint_descriptor_t *desc);

int uac_set_mute(void *buf, uint32_t len);

int uac_set_volume(void *buf, uint32_t len);

int uac_set_res(void *buf, uint32_t len);

int uac_set_agc(void *buf, uint32_t len);

int uac_handle_ac_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl);

int uac_handle_as_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl);

int uac_set_spk_interface(struct usb_gadget *gadget, int alt);

int uac_set_mic_interface(struct usb_gadget *gadget, int alt);

int uac_get_spk_interface(struct usb_gadget *gadget);

int uac_get_mic_interface(struct usb_gadget *gadget);

void uac_spk_start_stream(void);

void uac_mic_start_stream(void);

void uac_spk_stop_stream(void);

void uac_mic_stop_stream(void);

int uac_mic_isoc_commit_frame(audio_frame_buf_t *cur);

int uac_spk_iso_get_frame(audio_frame_buf_t *cur, uint32_t frame_sample_len_byte);

void uac_spk_register_callback(uac_spk_user_start_func_t start_cb, uac_spk_user_stop_func_t stop_cb);

void uac_mic_register_callback(uac_mic_user_start_func_t start_cb, uac_mic_user_stop_func_t stop_cb);

int uac_isoc_init(uac_param_t *param);

int uac_isoc_get_param(uac_param_t *param);

#ifdef __cplusplus
}
#endif

#endif // __UVC_ISOC_AUDIO_H__
