#ifndef __UVC_COMM_H__
#define __UVC_COMM_H__

#include "no_os_defs.h"
#include <stdbool.h>
#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Enumeration of the endpoint types.

    **Description**\n
    There are four types of endpoints. This defines the behaviour of
    the endpoint. The control endpoint is a compulsory for any device
    whereas the other endpoints are used as per device requirement.
 */
typedef enum CyU3PUsbEpType_t
{
    USB_EP_CONTROL = 0,          /**< Control Endpoint Type */
    USB_EP_ISO  = 1,             /**< Isochronous Endpoint Type */
    USB_EP_BULK = 2,             /**< Bulk Endpoint Type */
    USB_EP_INTR = 3              /**< Interrupt Endpoint Type */
} CyU3PUsbEpType_t;


/** \brief Enumeration of descriptor types.

    **Description**\n
    A USB device is identified by the descriptors provided. The following
    are among the standard descriptors defined by the USB specification.
 */
typedef enum CyU3PUsbDescType
{
    USB_DEVICE_DESCR = 0x01,      /**< Device descriptor  */
    USB_CONFIG_DESCR,             /**< Configuration descriptor */
    USB_STRING_DESCR,             /**< String descriptor */
    USB_INTRFC_DESCR,             /**< Interface descriptor */
    USB_ENDPNT_DESCR,             /**< Endpoint descriptor */
    USB_DEVQUAL_DESCR,            /**< Device Qualifier descriptor */
    USB_OTHERSPEED_DESCR,         /**< Other Speed Configuration descriptor */
    USB_INTRFC_POWER_DESCR,       /**< Interface power descriptor descriptor */
    USB_OTG_DESCR,                /**< OTG descriptor */
    BOS_DESCR = 0x0F,             /**< BOS descriptor */
    DEVICE_CAPB_DESCR,            /**< Device Capability descriptor */
    USB_HID_DESCR = 0x21,         /**< HID descriptor */
    USB_REPORT_DESCR,             /**< Report descriptor */
    SS_EP_COMPN_DESCR = 0x30      /**< Endpoint companion descriptor */
} CyU3PUsbDescType;

/*
   The following constants are taken from the USB and USB Video Class (UVC) specifications.
   They are defined here for convenient usage in the rest of the application source code.
 */
#define INTF_ASSN_DSCR_TYPE       (0x0B)                  /* Type code for Interface Association Descriptor (IAD) */

#define USB_SETUP_REQ_TYPE_MASK   (uint32_t)(0x000000FF)  /* Mask for bmReqType field from a control request. */
#define USB_SETUP_REQ_MASK        (uint32_t)(0x0000FF00)  /* Mask for bRequest field from a control request. */
#define USB_SETUP_VALUE_MASK      (uint32_t)(0xFFFF0000)  /* Mask for wValue field from a control request. */
#define USB_SETUP_INDEX_MASK      (uint32_t)(0x0000FFFF)  /* Mask for wIndex field from a control request. */
#define USB_SETUP_LENGTH_MASK     (uint32_t)(0xFFFF0000)  /* Mask for wLength field from a control request. */

#define USB_SET_INTF_REQ_TYPE     (uint8_t)(0x01)         /* USB SET_INTERFACE Request Type. */
#define USB_SET_INTERFACE_REQ     (uint8_t)(0x0B)         /* USB SET_INTERFACE Request code. */

#define UVC_MAX_HEADER            (12)                    /* Maximum UVC header size, in bytes. */
#define UVC_HEADER_DEFAULT_BFH    (0x8C)                  /* Default BFH (Bit Field Header) for the UVC Header */
#define UVC_MAX_PROBE_SETTING     (26)                    /* Maximum number of bytes in Probe Control */
#define UVC_MAX_PROBE_SETTING_ALIGNED (32)                /* Probe control data size aligned to 16 bytes. */

#define UVC_HEADER_FRAME          (0)                     /* UVC header value for normal frame indication */
#define UVC_HEADER_EOF            (uint8_t)(1 << 1)       /* UVC header value for end of frame indication */
#define UVC_HEADER_FRAME_ID       (uint8_t)(1 << 0)       /* Frame ID toggle bit in UVC header. */
#define UVC_HEADER_BAD_PAYLOAD    (uint8_t)(1 << 6)       /* The payload in this packet is discrupt */

#define USB_UVC_SET_REQ_TYPE      (uint8_t)(0x21)         /* UVC Interface SET Request Type */
#define USB_UVC_GET_REQ_TYPE      (uint8_t)(0xA1)         /* UVC Interface GET Request Type */
#define USB_UVC_GET_CUR_REQ       (uint8_t)(0x81)         /* UVC GET_CUR Request */
#define USB_UVC_SET_CUR_REQ       (uint8_t)(0x01)         /* UVC SET_CUR Request */
#define USB_UVC_GET_MIN_REQ       (uint8_t)(0x82)         /* UVC GET_MIN Request */
#define USB_UVC_GET_MAX_REQ       (uint8_t)(0x83)         /* UVC GET_MAX Request */
#define USB_UVC_GET_RES_REQ       (uint8_t)(0x84)         /* UVC GET_RES Request */
#define USB_UVC_GET_LEN_REQ       (uint8_t)(0x85)         /* UVC GET_LEN Request */
#define USB_UVC_GET_INFO_REQ      (uint8_t)(0x86)         /* UVC GET_INFO Request */
#define USB_UVC_GET_DEF_REQ       (uint8_t)(0x87)         /* UVC GET_DEF Request */

#define UVC_PROBE_CTRL            (uint16_t)(0x0100)      /* wValue setting used to access PROBE control. */
#define UVC_COMMIT_CTRL           (uint16_t)(0x0200)      /* wValue setting used to access COMMIT control. */

#define UVC_INTERFACE_CTRL        (uint8_t)(0)            /* wIndex value used to select UVC interface control. */
#define UVC_CAMERA_TERMINAL_ID    (uint8_t)(1)            /* wIndex value used to select Camera terminal. */
#define UVC_PROCESSING_UNIT_ID    (uint8_t)(2)            /* wIndex value used to select Processing Unit. */
#define UVC_EXTENSION_UNIT_ID     (uint8_t)(3)            /* wIndex value used to select Extension Unit. */

/* Processing Unit specific UVC control selector codes defined in the USB Video Class specification. */
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL         (uint16_t)(0x0100)
#define UVC_PU_BRIGHTNESS_CONTROL                     (uint16_t)(0x0200)
#define UVC_PU_CONTRAST_CONTROL                       (uint16_t)(0x0300)
#define UVC_PU_GAIN_CONTROL                           (uint16_t)(0x0400)
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL           (uint16_t)(0x0500)
#define UVC_PU_HUE_CONTROL                            (uint16_t)(0x0600)
#define UVC_PU_SATURATION_CONTROL                     (uint16_t)(0x0700)
#define UVC_PU_SHARPNESS_CONTROL                      (uint16_t)(0x0800)
#define UVC_PU_GAMMA_CONTROL                          (uint16_t)(0x0900)
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (uint16_t)(0x0A00)
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (uint16_t)(0x0B00)
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL        (uint16_t)(0x0C00)
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (uint16_t)(0x0D00)
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL             (uint16_t)(0x0E00)
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (uint16_t)(0x0F00)
#define UVC_PU_HUE_AUTO_CONTROL                       (uint16_t)(0x1000)
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL          (uint16_t)(0x1100)
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL             (uint16_t)(0x1200)

/* Camera Terminal specific UVC control selector codes defined in the USB Video Class specification. */
#define UVC_CT_SCANNING_MODE_CONTROL                  (uint16_t)(0x0100)
#define UVC_CT_AE_MODE_CONTROL                        (uint16_t)(0x0200)
#define UVC_CT_AE_PRIORITY_CONTROL                    (uint16_t)(0x0300)
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL         (uint16_t)(0x0400)
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL         (uint16_t)(0x0500)
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL                 (uint16_t)(0x0600)
#define UVC_CT_FOCUS_RELATIVE_CONTROL                 (uint16_t)(0x0700)
#define UVC_CT_FOCUS_AUTO_CONTROL                     (uint16_t)(0x0800)
#define UVC_CT_IRIS_ABSOLUTE_CONTROL                  (uint16_t)(0x0900)
#define UVC_CT_IRIS_RELATIVE_CONTROL                  (uint16_t)(0x0A00)
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL                  (uint16_t)(0x0B00)
#define UVC_CT_ZOOM_RELATIVE_CONTROL                  (uint16_t)(0x0C00)
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL               (uint16_t)(0x0D00)
#define UVC_CT_PANTILT_RELATIVE_CONTROL               (uint16_t)(0x0E00)
#define UVC_CT_ROLL_ABSOLUTE_CONTROL                  (uint16_t)(0x0F00)
#define UVC_CT_ROLL_RELATIVE_CONTROL                  (uint16_t)(0x1000)
#define UVC_CT_PRIVACY_CONTROL                        (uint16_t)(0x1100)

#define EP_IN_TYPE                    0x80    /* USB IN end points have MSB set */
#define EP_OUT_TYPE                   0x00    /* USB OUT end points have MSB set */

#define UVC_YUY2_GUID \
        'Y', 'U', 'Y', '2', \
        0x00,0x00,0x10,0x00,\
        0x80,0x00,0x00,0xAA,\
        0x00,0x38,0x9B,0x71

#define UVC_GRAY8_GUID \
        'Y', '8', '0', '0', \
        0x00,0x00,0x10,0x00,\
        0x80,0x00,0x00,0xAA,\
        0x00,0x38,0x9B,0x71

#define B2VAL(x)                            ((x) & 0xFF),(((x) >> 8) & 0xFF)
#define B3VAL(x)                            ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF)
#define B4VAL(x)                            ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF),(((x) >> 24) & 0xFF)

/* USB Setup Commands Recipient */
#define USB_TARGET_MASK              (0x03)    /* The Target mask */
#define USB_TARGET_DEVICE            (0x00)    /* The USB Target Device */
#define USB_TARGET_INTF              (0x01)    /* The USB Target Interface */
#define USB_TARGET_ENDPT             (0x02)    /* The USB Target Endpoint */
#define USB_TARGET_OTHER             (0x03)    /* The USB Target Other */

#define USB_GS_DEVICE                (0x80)    /* The standard device request, GET_STATUS device */
#define USB_GS_INTERFACE             (0x81)    /* The standard device request, GET_STATUS interface */
#define USB_GS_ENDPOINT              (0x82)    /* The standard device request, GET_STATUS endpoint */

/* USB Request Class types */
#define USB_TYPE_MASK                (0x60)     /* The request type mask */
#define USB_STANDARD_RQT             (0x00)     /* The USB standard request */
#define USB_CLASS_RQT                (0x20)     /* The USB class request */
#define USB_VENDOR_RQT               (0x40)     /* The USB vendor request */
#define USB_RESERVED_RQT             (0x60)     /* The USB reserved request */


#define USB_FUNC_NO_WAIT             (0)
#define USB_FUNC_WAIT_FOREVER        (-1)

/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
typedef struct usb_interface_assoc_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bFirstInterface;
	uByte  bInterfaceCount;
	uByte  bFunctionClass;
	uByte  bFunctionSubClass;
	uByte  bFunctionProtocol;
	uByte  iFunction;
} UPACKED usb_interface_assoc_descriptor_t;

typedef struct uvc_vc_intf_hdr_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uWord  bcdUVC;
	uWord  wTotalLength;
	uDWord dwClockFrequency;
	uByte  bInCollection;
	uByte  baInterfaceNr[1];
} UPACKED uvc_vc_intf_hdr_descriptor_t;

/* 3.7.2.3. Camera Terminal Descriptor */
typedef struct uvc_camera_terminal_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bTerminalID;
	uWord  wTerminalType;
	uByte  bAssocTerminal;
	uByte  iTerminal;
	uWord  wObjectiveFocalLengthMin;
	uWord  wObjectiveFocalLengthMax;
	uWord  wOcularFocalLength;
	uByte  bControlSize;
	uByte  bmControls[3];
} UPACKED uvc_camera_terminal_descriptor_t;

/* 3.7.2.5. Processing Unit Descriptor */
typedef struct uvc_processing_unit_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bUnitID;
	uByte  bSourceID;
	uWord  wMaxMultiplier;
	uByte  bControlSize;
	uByte  bmControls[3];
	uByte  iProcessing;
} UPACKED uvc_processing_unit_descriptor_t;

/* 3.7.2.6. Extension Unit Descriptor */
struct uvc_extension_unit_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bUnitID;
	uByte  guidExtensionCode[16];
	uByte  bNumControls;
	uByte  bNrInPins;
	uByte  baSourceID[0];
	uByte  bControlSize;
	uByte  bmControls[0];
	uByte  iExtension;
} __attribute__((__packed__));

#define UVC_DT_EXTENSION_UNIT_SIZE(p, n)		(24+(p)+(n))

#define UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) \
	uvc_extension_unit_descriptor_##p_##n

#define DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(p, n)	\
struct UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) {		\
	uByte  bLength;					\
	uByte  bDescriptorType;				\
	uByte  bDescriptorSubType;			\
	uByte  bUnitID;					\
	uByte  guidExtensionCode[16];			\
	uByte  bNumControls;				\
	uByte  bNrInPins;				\
	uByte  baSourceID[p];				\
	uByte  bControlSize;				\
	uByte  bmControls[n];				\
	uByte  iExtension;				\
} __attribute__ ((packed))

/* 3.7.2.2. Output Terminal Descriptor */
typedef struct uvc_output_terminal_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bTerminalID;
	uWord  wTerminalType;
	uByte  bAssocTerminal;
	uByte  bSourceID;
	uByte  iTerminal;
} UPACKED uvc_output_terminal_descriptor_t;

/* 3.8.2.2. Video Control Interrupt Endpoint Descriptor */
typedef struct uvc_control_endpoint_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uWord  wMaxTransferSize;
} UPACKED uvc_control_endpoint_descriptor_t;

/* 3.9.2.1. Input Header Descriptor */
struct uvc_input_header_descriptor {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bNumFormats;
	uWord  wTotalLength;
	uByte  bEndpointAddress;
	uByte  bmInfo;
	uByte  bTerminalLink;
	uByte  bStillCaptureMethod;
	uByte  bTriggerSupport;
	uByte  bTriggerUsage;
	uByte  bControlSize;
	uByte  bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_INPUT_HEADER_SIZE(n, p)			(13+(n*p))

#define UVC_INPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_input_header_descriptor_##n_##p

#define DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_INPUT_HEADER_DESCRIPTOR(n, p) {		\
	uByte  bLength;					\
	uByte  bDescriptorType;				\
	uByte  bDescriptorSubType;			\
	uByte  bNumFormats;				\
	uWord  wTotalLength;				\
	uByte  bEndpointAddress;				\
	uByte  bmInfo;					\
	uByte  bTerminalLink;				\
	uByte  bStillCaptureMethod;			\
	uByte  bTriggerSupport;				\
	uByte  bTriggerUsage;				\
	uByte  bControlSize;				\
	uByte  bmaControls[p][n];			\
} __attribute__ ((packed))

#define _UVC_INPUT_HEADER_DESCRIPTOR(n, p) UVC_INPUT_HEADER_DESCRIPTOR(n, p)

/* Uncompressed Payload - 3.1.1. Uncompressed Video Format Descriptor */
typedef struct uvc_format_uncompressed {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bFormatIndex;
	uByte  bNumFrameDescriptors;
	uByte  guidFormat[16];
	uByte  bBitsPerPixel;
	uByte  bDefaultFrameIndex;
	uByte  bAspectRatioX;
	uByte  bAspectRatioY;
	uByte  bmInterlaceFlags;
	uByte  bCopyProtect;
} __attribute__((__packed__)) uvc_format_uncompressed_t;

#define UVC_DT_FORMAT_UNCOMPRESSED_SIZE			27

typedef struct uvc_format_mjpeg {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bFormatIndex;
	uByte  bNumFrameDescriptors;
    uByte  bmFlags;
    uByte  bDefaultFrameIndex;
	uByte  bAspectRatioX;
	uByte  bAspectRatioY;
	uByte  bmInterlaceFlags;
	uByte  bCopyProtect;
} __attribute__((__packed__)) uvc_format_mjpeg_t;

typedef struct uvc_format_h264 {
      uByte  bLength;
      uByte  bDescriptorType;
      uByte  bDescriptorSubType;
      uByte  bFormatIndex;
      uByte  bNumFrameDescriptors;
      uByte  guidFormat[16];
      uByte  bBitsPerPixel;
      uByte  bDefaultFrameIndex;
      uByte  bAspectRatioX;
      uByte  bAspectRatioY;
      uByte  bmInterlaceFlags;
      uByte  bCopyProtect;
      uByte  bVariableSize;
} __attribute__((__packed__)) uvc_format_h264_t;

#define UVC_DT_FORMAT_H264_SIZE                       28

/* Uncompressed Payload - 3.1.2. Uncompressed Video Frame Descriptor */
struct uvc_frame_uncompressed {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bFrameIndex;
	uByte  bmCapabilities;
	uWord  wWidth;
	uWord  wHeight;
	uDWord dwMinBitRate;
	uDWord dwMaxBitRate;
	uDWord dwMaxVideoFrameBufferSize;
	uDWord dwDefaultFrameInterval;
	uByte  bFrameIntervalType;
	uDWord dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_UNCOMPRESSED_SIZE(n)		(26+4*(n))

#define UVC_FRAME_UNCOMPRESSED(n) \
	uvc_frame_uncompressed_##n

#define DECLARE_UVC_FRAME_UNCOMPRESSED(n)		\
struct UVC_FRAME_UNCOMPRESSED(n) {			\
	uByte  bLength;					\
	uByte  bDescriptorType;				\
	uByte  bDescriptorSubType;			\
	uByte  bFrameIndex;				\
	uByte  bmCapabilities;				\
	uWord  wWidth;					\
	uWord  wHeight;					\
	uDWord dwMinBitRate;				\
	uDWord dwMaxBitRate;				\
	uDWord dwMaxVideoFrameBufferSize;		\
	uDWord dwDefaultFrameInterval;			\
	uByte  bFrameIntervalType;			\
	uDWord dwFrameInterval[n];			\
} __attribute__ ((packed))

/* Mjpeg Payload - 3.1.2. Mjpeg Video Frame Descriptor */
struct uvc_frame_mjpeg {
	uByte  bLength;
	uByte  bDescriptorType;
	uByte  bDescriptorSubType;
	uByte  bFrameIndex;
	uByte  bmCapabilities;
	uWord  wWidth;
	uWord  wHeight;
	uDWord dwMinBitRate;
	uDWord dwMaxBitRate;
	uDWord dwMaxVideoFrameBufferSize;
	uDWord dwDefaultFrameInterval;
	uByte  bFrameIntervalType;
	uDWord dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_MJPEG_SIZE(n)		(26+4*(n))

#define UVC_FRAME_MJPEG(n) \
	uvc_frame_mjpeg_##n

#define DECLARE_UVC_FRAME_MJPEG(n)		\
struct UVC_FRAME_MJPEG(n) {			\
	uByte  bLength;					\
	uByte  bDescriptorType;				\
	uByte  bDescriptorSubType;			\
	uByte  bFrameIndex;				\
	uByte  bmCapabilities;				\
	uWord  wWidth;					\
	uWord  wHeight;					\
	uDWord dwMinBitRate;				\
	uDWord dwMaxBitRate;				\
	uDWord dwMaxVideoFrameBufferSize;		\
	uDWord dwDefaultFrameInterval;			\
	uByte  bFrameIntervalType;			\
	uDWord dwFrameInterval[n];			\
} __attribute__ ((packed))

struct uvc_frame_h264 {
      uByte  bLength;
      uByte  bDescriptorType;
      uByte  bDescriptorSubType;
      uByte  bFrameIndex;
      uByte  bmCapabilities;
      uWord  wWidth;
      uWord  wHeight;
      uDWord dwMinBitRate;
      uDWord dwMaxBitRate;
      uDWord dwDefaultFrameInterval;
      uByte  bFrameIntervalType;
      uDWord dwBytesPerLine;
      uDWord dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_H264_SIZE(n)             (26+4*(n))

#define UVC_FRAME_H264(n) \
      uvc_frame_h264_##n

#define DECLARE_UVC_FRAME_H264(n)             \
struct UVC_FRAME_H264(n) {                    \
      uByte  bLength;                                 \
      uByte  bDescriptorType;                         \
      uByte  bDescriptorSubType;                      \
      uByte  bFrameIndex;                             \
      uByte  bmCapabilities;                          \
      uWord  wWidth;                                  \
      uWord  wHeight;                                 \
      uDWord dwMinBitRate;                            \
      uDWord dwMaxBitRate;                            \
      uDWord dwDefaultFrameInterval;                  \
      uByte  bFrameIntervalType;                      \
      uDWord dwBytesPerLine;              \
      uDWord dwFrameInterval[n];                      \
} __attribute__ ((packed))

/* 4.3.1.1. Video Probe and Commit Controls */
typedef struct uvc_streaming_control {
	uWord  bmHint;
	uByte  bFormatIndex;
	uByte  bFrameIndex;
	uDWord dwFrameInterval;
	uWord  wKeyFrameRate;
	uWord  wPFrameRate;
	uWord  wCompQuality;
	uWord  wCompWindowSize;
	uWord  wDelay;
	uDWord dwMaxVideoFrameSize;
	uDWord dwMaxPayloadTransferSize;
	uDWord dwClockFrequency;
	uByte  bmFramingInfo;
	uByte  bPreferedVersion;
	uByte  bMinVersion;
	uByte  bMaxVersion;
} __attribute__((__packed__)) uvc_streaming_control_t;

/* 3.9.2.6. Color matching descriptor */
typedef struct uvc_color_matching_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubType;
    uByte bColorPrimaries;
    uByte bTransferCharacteristics;
    uByte bMatrixCoefficients;
} __attribute__((__packed__)) uvc_color_matching_descriptor_t;

typedef enum {
    UVC_GRAY8 = 0,
    UVC_YUYV  = 1,
    UVC_MJPEG = 2,
    UVC_H264 = 3,
    UVC_MAX,
} uvc_fmt_t;

typedef void (*uvc_user_start_func_t)(uvc_fmt_t fmt, int w, int h, int fps);
typedef void (*uvc_user_stop_func_t)(void);

typedef struct {
	int width;
	int height;
} usb_ext4_frame_resolution_t;

typedef struct _uvc_param_t {
    uint32_t max_payload_size;
    int      ep0_desc_sel;
	int 	 ext4_frame_resolution_num;
	usb_ext4_frame_resolution_t *usb_ext4_frame_resolution;
    int      uvc_h264_enable;
	int      max_request_size;
} uvc_param_t;

#define UVC_PARAM_INITIALIZER    \
    ( ( (uvc_param_t)            \
    {                            \
        .max_payload_size = 512, \
        .ep0_desc_sel     = 0,   \
        .ext4_frame_resolution_num = 0,     \
        .usb_ext4_frame_resolution = NULL,  \
        .uvc_h264_enable  = 0,   \
        .max_request_size  = 512,\
    }                            \
    )                            \
    )


typedef struct _uvc_func_op_t {
    int  (*uvc_init)(uvc_param_t *param);
    int  (*uvc_commit_frame)(frame_buf_t *cur);
    void (*uvc_register_start_cb)(uvc_user_start_func_t func);
    void (*uvc_register_stop_cb)(uvc_user_stop_func_t func);
    bool (*acm_inited)(void);
    int  (*acm_send_buf)(const void *buf, int bytes, int wait_option);
    int  (*acm_recv_buf)(void *buf, int *bytes, int wait_option);
} uvc_func_op_t;

/* --------------------------------------------------------------------------*/
/** 
 * @brief initialize uvc camera
 * 
 * @param dev_name[in]      uvc devcie name
 * @param param[in]         param to initialize device, 
 *                          using default settings if the param pointer is NULL
 * 
 * @return 0: Success, other: Fail
 */
/* --------------------------------------------------------------------------*/
int  uvc_init(const char *dev_name, uvc_param_t *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  release and clean up uvc resource
 * 
 * @return 0: Success, other: Fail
 */
/* --------------------------------------------------------------------------*/
int  uvc_cleanup(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  commit video frame to uvc
 * 
 * @param cur[in]           video frame pointer
 * 
 * @return 0: Success, other: Fail
 */
/* --------------------------------------------------------------------------*/
int  uvc_commit_frame(frame_buf_t *cur);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  register user callback when uvc starts to stream
 * 
 * @param func[in]          user callback to be registered
 */
/* --------------------------------------------------------------------------*/
void uvc_register_user_start_cb(uvc_user_start_func_t func);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  register user callback when uvc stops streaming
 * 
 * @param func[in]          user callback to be registered
 */
/* --------------------------------------------------------------------------*/
void uvc_register_user_stop_cb(uvc_user_stop_func_t func);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  usb_function_acm_inited 
 * 
 * @return true/false  
 */
/* --------------------------------------------------------------------------*/
bool usb_function_acm_inited(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  usb_function_acm_send_buffer 
 * 
 * @param buffer[in]        send buffer pointer
 * @param bytes[in]         bytes to send
 * @param wait_option[in]   wait option
 * 
 * @return bytes which has been sent
 */
/* --------------------------------------------------------------------------*/
int  usb_function_acm_send_buffer(const void *buffer, int bytes, int wait_option);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  usb_function_acm_recv_buffer 
 * 
 * @param buffer[in]        recv buffer pointer
 * @param bytes[in/out]     input the maximum buffer size, output the actual received bytes
 * @param wait_option[in]   wait option
 * 
 * @return bytes which has been received
 */
/* --------------------------------------------------------------------------*/
int  usb_function_acm_recv_buffer(void *buffer, int *bytes, int wait_option);

#ifdef __cplusplus
}
#endif

#endif // __UVC_COMM_H__
