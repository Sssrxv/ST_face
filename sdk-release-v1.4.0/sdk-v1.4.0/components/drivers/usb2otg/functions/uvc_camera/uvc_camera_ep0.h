#ifndef __UVC_CAMERA_EP0_H__
#define __UVC_CAMERA_EP0_H__

#include "no_os_defs.h"
//#include "dwc_otg_os_dep.h"
#include "uvc_camera.h"
#include "uvc_comm.h"

#ifdef _MSC_VER
#include <pshpack1.h>
#endif
#include "cdc.h"

#ifdef __cplusplus
extern "C" {
#endif

// UVC ctrl and stream interface numbers
#define UVC_STREAM_INTERFACE      (uint8_t)(1)            /* Streaming Interface : Alternate Setting 1 */
#define UVC_CONTROL_INTERFACE     (uint8_t)(0)            /* Control Interface */

// CDC ACM interface numbers
#define CDC_ACM_INTERFACE         (2)

/* DMA socket selection for UVC data transfer. */
#define EP_CONTROL_STATUS_SOCKET      0x01    /* USB Consumer socket 1 is used for the status pipe. */
#define EP_VIDEO_CONS_SOCKET          0x02    /* USB Consumer socket 2 is used for video data. */

/* Endpoint definition for UVC application */
#define EP_CONTROL_STATUS             (EP_CONTROL_STATUS_SOCKET | EP_IN_TYPE)     /* EP 1 IN */
#define EP_BULK_VIDEO                 (EP_VIDEO_CONS_SOCKET     | EP_IN_TYPE)     /* EP 2 IN */

/* Socket and endpoint definitions for the USB extention control interface. */
#define EP_EXT_CTRL_CMD_SOCKET        0x03
#define EP_EXT_CTRL_RSP_SOCKET        0x03
#define EP_EXT_CTRL_CMD               (EP_EXT_CTRL_CMD_SOCKET)                    /* EP 3 OUT */
#define EP_EXT_CTRL_RSP               (EP_EXT_CTRL_RSP_SOCKET | EP_IN_TYPE)       /* EP 3 IN */

/* Socket and endpoint definitions for the USB imu data interface. */
#define EP_IMU_DATA_SOCKET            0x4    /* USB Consumer socket 4 is used as the imu data pipe */
#define EP_IMU_DATA                   (EP_IMU_DATA_SOCKET | EP_IN_TYPE)           /* EP 4 IN */

/* Socket and endpoint definitions for the USB log interface. */
#define EP_LOG_SOCKET                 0x5     /* USB Consumer socket 5 is used as the log pipe */
#define EP_LOG                        (EP_LOG_SOCKET | EP_IN_TYPE)                /* EP 5 IN */


// ---------------------------------------------------------
//          UVC related definitions
// ---------------------------------------------------------
/* Number of VS mjpeg frames for format 1 */
#define VS_NUM_MJPEG_FMT1_FRMS              1

/* Number of VS yuyv frames for format 1 */
#define VS_NUM_YUYV_FMT1_FRMS               1

/* Number of VS formats */
#define VS_NUM_FORMATS                      3
/* Number of VS frames for format 1 */
#define VS_NUM_FMT1_FRMS                    31
/* Number of VS frames for format 2 */
#define VS_NUM_FMT2_FRMS                    3
/* Number of VS frames for format 3 */
#define VS_NUM_FMT3_FRMS                    14


DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3);
DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 1);
DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 2);
DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 3);
DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 4);
DECLARE_UVC_FRAME_UNCOMPRESSED(1);
DECLARE_UVC_FRAME_UNCOMPRESSED(2);
DECLARE_UVC_FRAME_UNCOMPRESSED(3);
DECLARE_UVC_FRAME_MJPEG(1);
DECLARE_UVC_FRAME_MJPEG(2);
DECLARE_UVC_FRAME_MJPEG(3);


/**
 */
typedef struct fs_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
} UPACKED fs_config_desc_t;


/**
 */

typedef struct uvc_vs_frames_formats_descriptor {
    /* Uncompressed: GRAY8 */
    uvc_format_uncompressed_t           vs_uncomp_format_desc_1;
    struct UVC_FRAME_UNCOMPRESSED(3)    vs_uncomp_frames_desc_1[VS_NUM_FMT1_FRMS];
    uvc_color_matching_descriptor_t     vs_color_desc_1;
    /* Uncompressed: YUYV */
    uvc_format_uncompressed_t           vs_uncomp_format_desc_2;
    struct UVC_FRAME_UNCOMPRESSED(3)    vs_uncomp_frames_desc_2[VS_NUM_FMT2_FRMS];
    uvc_color_matching_descriptor_t     vs_color_desc_2;
    /* Compressed  : MJPEG */
    uvc_format_mjpeg_t                  vs_mjpeg_format_desc_3;
    struct UVC_FRAME_MJPEG(3)           vs_mjpeg_frame_desc_3[VS_NUM_FMT3_FRMS];
} uvc_vs_frames_formats_descriptor_t;

typedef struct uvc_vs_frames_mjpeg_formats_descriptor {
    /* Compressed  : MJPEG */
    uvc_format_mjpeg_t                  vs_mjpeg_format_desc_1;
    struct UVC_FRAME_MJPEG(3)           vs_mjpeg_frame_desc_1[VS_NUM_MJPEG_FMT1_FRMS];
} uvc_vs_frames_mjpeg_formats_descriptor_t;

typedef struct uvc_vs_frames_yuyv_formats_descriptor {
    /* Uncompressed: YUYV */
    uvc_format_uncompressed_t           vs_uncomp_format_desc_1;
    struct UVC_FRAME_UNCOMPRESSED(3)    vs_uncomp_frames_desc_1[VS_NUM_YUYV_FMT1_FRMS];
    uvc_color_matching_descriptor_t     vs_color_desc_1;
} uvc_vs_frames_yuyv_formats_descriptor_t;


typedef struct hs_config_desc_st {
	usb_config_descriptor_t			    config_desc;
    // UVC
    usb_interface_assoc_descriptor_t    intf_assoc_desc_0;
    usb_interface_descriptor_t          std_vc_if_ctl_desc;
    uvc_vc_intf_hdr_descriptor_t        vc_intf_hdr_desc;
    uvc_camera_terminal_descriptor_t    camera_term_desc;
    uvc_processing_unit_descriptor_t    proc_unit_desc;
    struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) ext_unit_desc;
    uvc_output_terminal_descriptor_t    output_term_desc;

    usb_interface_descriptor_t          vs_if_alt0_desc;

    struct _UVC_INPUT_HEADER_DESCRIPTOR(1, VS_NUM_FORMATS) vs_input_hdr_desc;
    uvc_vs_frames_formats_descriptor_t  vs_frames_formats_desc;

	usb_endpoint_descriptor_t		    bulk_video_in_ep_desc;

    // external communication
#ifndef USE_CDC_ACM
    usb_interface_descriptor_t          ext_comm_if_desc;
    usb_endpoint_descriptor_t		    bulk_ctrl_out_ep_desc;
    usb_endpoint_descriptor_t		    bulk_resp_in_ep_desc;
#else
    usb_interface_assoc_descriptor_t    intf_assoc_desc_1;
    // CDC ACM interface 0
    usb_interface_descriptor_t          cdc_intf_desc0;
    usb_cdc_header_descriptor_t         cdc_header;
    usb_cdc_acm_descriptor_t            cdc_acm_desc;
    usb_cdc_union_descriptor_t          cdc_union_desc;
    usb_cdc_call_mgmt_descriptor_t      cdc_call_mgmt_desc;
    usb_endpoint_descriptor_t           intrp_acm_ep_desc;
    // CDC ACM interface 1
    usb_interface_descriptor_t          cdc_intf_desc1;
    usb_endpoint_descriptor_t           bulk_resp_in_ep_desc;
    usb_endpoint_descriptor_t           bulk_ctrl_out_ep_desc;
#endif

} UPACKED hs_config_desc_t;

typedef struct hs_config_mjpeg_desc_st {
	usb_config_descriptor_t			    config_desc;
    // UVC
    usb_interface_assoc_descriptor_t    intf_assoc_desc_0;
    usb_interface_descriptor_t          std_vc_if_ctl_desc;
    uvc_vc_intf_hdr_descriptor_t        vc_intf_hdr_desc;
    uvc_camera_terminal_descriptor_t    camera_term_desc;
    uvc_processing_unit_descriptor_t    proc_unit_desc;
    struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) ext_unit_desc;
    uvc_output_terminal_descriptor_t    output_term_desc;

    usb_interface_descriptor_t          vs_if_alt0_desc;

    struct _UVC_INPUT_HEADER_DESCRIPTOR(1, 1) vs_input_hdr_desc;
    uvc_vs_frames_mjpeg_formats_descriptor_t  vs_frames_formats_desc;

	usb_endpoint_descriptor_t		    bulk_video_in_ep_desc;

    usb_interface_assoc_descriptor_t    intf_assoc_desc_1;
    // CDC ACM interface 0
    usb_interface_descriptor_t          cdc_intf_desc0;
    usb_cdc_header_descriptor_t         cdc_header;
    usb_cdc_acm_descriptor_t            cdc_acm_desc;
    usb_cdc_union_descriptor_t          cdc_union_desc;
    usb_cdc_call_mgmt_descriptor_t      cdc_call_mgmt_desc;
    usb_endpoint_descriptor_t           intrp_acm_ep_desc;
    // CDC ACM interface 1
    usb_interface_descriptor_t          cdc_intf_desc1;
    usb_endpoint_descriptor_t           bulk_resp_in_ep_desc;
    usb_endpoint_descriptor_t           bulk_ctrl_out_ep_desc;

} UPACKED hs_config_mjpeg_desc_t;

typedef struct hs_config_yuyv_desc_st {
	usb_config_descriptor_t			    config_desc;
    // UVC
    usb_interface_assoc_descriptor_t    intf_assoc_desc_0;
    usb_interface_descriptor_t          std_vc_if_ctl_desc;
    uvc_vc_intf_hdr_descriptor_t        vc_intf_hdr_desc;
    uvc_camera_terminal_descriptor_t    camera_term_desc;
    uvc_processing_unit_descriptor_t    proc_unit_desc;
    struct UVC_EXTENSION_UNIT_DESCRIPTOR(1, 3) ext_unit_desc;
    uvc_output_terminal_descriptor_t    output_term_desc;

    usb_interface_descriptor_t          vs_if_alt0_desc;

    struct _UVC_INPUT_HEADER_DESCRIPTOR(1, 1) vs_input_hdr_desc;
    uvc_vs_frames_yuyv_formats_descriptor_t  vs_frames_formats_desc;

	usb_endpoint_descriptor_t		    bulk_video_in_ep_desc;

    usb_interface_assoc_descriptor_t    intf_assoc_desc_1;
    // CDC ACM interface 0
    usb_interface_descriptor_t          cdc_intf_desc0;
    usb_cdc_header_descriptor_t         cdc_header;
    usb_cdc_acm_descriptor_t            cdc_acm_desc;
    usb_cdc_union_descriptor_t          cdc_union_desc;
    usb_cdc_call_mgmt_descriptor_t      cdc_call_mgmt_desc;
    usb_endpoint_descriptor_t           intrp_acm_ep_desc;
    // CDC ACM interface 1
    usb_interface_descriptor_t          cdc_intf_desc1;
    usb_endpoint_descriptor_t           bulk_resp_in_ep_desc;
    usb_endpoint_descriptor_t           bulk_ctrl_out_ep_desc;

} UPACKED hs_config_yuyv_desc_t;


/**
 */
typedef struct ss_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	ss_endpoint_companion_descriptor_t	bulk_in_ss_ep_comp_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
	ss_endpoint_companion_descriptor_t	bulk_out_ss_ep_comp_desc;
} UPACKED ss_config_desc_t;


int uvc_camera_get_desc(struct usb_gadget *gadget, const usb_device_request_t *ctrl);

int uvc_camera_enable_eps(struct usb_gadget *gadget, f_uvc_camera_t *uvc);

int uvc_camera_disable_eps(struct usb_gadget *gadget, f_uvc_camera_t *uvc);

int uvc_bulk_get_vs_params(struct usb_gadget *gadget, int fmt_idx, int frm_idx, uvc_fmt_t *fmt_ptr, int *pw, int *ph);

/* Stop packing structure type definitions */
#ifdef _MSC_VER
#include <poppack.h>
#endif

#ifdef __cplusplus
}    
#endif

#endif // __UVC_CAMERA_EP0_H__
