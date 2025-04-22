#include <stdint.h>
#include "mem_AI3100.h"
#include "no_os_defs.h"
#include "usb.h"
#include "dwc_os.h"
#include "syslog.h"
#include "uvc_camera.h"
#include "aiva_utils.h"

#include "uvc_camera_ep0.h"

#define VS_UNCOMP_FRAME_DESC(idx, w, h, max_frm_sz)   \
    {\
        0x26,                           /* Descriptor size */\
        0x24,                           /* Descriptor type*/\
        0x05,                           /* Subtype: uncompressed frame I/F */\
        idx,                            /* Frame Descriptor Index */\
        0x03,                           /* Still image capture method 1 supported, fixed frame rate */\
        B2VAL(w),                       /* Width  in pixel */\
        B2VAL(h),                       /* Height in pixel */\
        0x00,0x50,0x97,0x31,            /* Min bit rate bits/s. Not specified, taken from MJPEG */\
        0x00,0x50,0x97,0x31,            /* Max bit rate bits/s. Not specified, taken from MJPEG */\
        B4VAL(max_frm_sz),              /* Maximum video or still frame size in bytes(Deprecated) */\
        B4VAL(10000000/30),             /* Default Frame Interval  (30 fps) */\
        0x03,                           /* Frame interval(Frame Rate) types: Only one frame interval supported */\
        B4VAL(10000000/60),             /* Shortest Frame Interval (60 fps) */\
        B4VAL(10000000/30),             /* Shortest Frame Interval (30 fps) */\
        B4VAL(10000000/15),             /* Shortest Frame Interval (15 fps) */\
    }

#define VS_MJPEG_FRAME_DESC(idx, w, h, max_frm_sz)      \
    {\
        0x26,                           /* Descriptor size */\
        0x24,                           /* Descriptor type*/\
        0x07,                           /* Subtype: VS_FORMAT_MJPEG */\
        idx,                            /* Frame Descriptor Index */\
        0x00,                           /* Still image capture method not supported */\
        B2VAL(w),                       /* Width in pixel */\
        B2VAL(h),                       /* Height in pixel */\
        0x00,0x50,0x97,0x31,            /* Min bit rate bits/s. Not specified, taken from MJPEG */\
        0x00,0x50,0x97,0x31,            /* Max bit rate bits/s. Not specified, taken from MJPEG */\
        B4VAL(max_frm_sz),              /* Maximum video or still frame size in bytes(Deprecated) */\
        B4VAL(10000000/30),             /* Default Frame Interval  (30 fps) */\
        0x03,                           /* Frame interval(Frame Rate) types: Only one frame interval supported */\
        B4VAL(10000000/60),             /* Shortest Frame Interval (60 fps) */\
        B4VAL(10000000/30),             /* Shortest Frame Interval (30 fps) */\
        B4VAL(10000000/15),             /* Shortest Frame Interval (15 fps) */\
    }
/*=======================================================================*/
/*
 * EP0 routines
 */
# define DWC_VENDOR_ID     0x1e08
# define DWC_PRODUCT_ID    0x0001

/* Make the following structure type definitions "packed" if using a Microsoft
 * compiler. The UPACKED attribute (defined in no_os_defs.h) embedded in the
 * structure type definitions does the same thing for GCC. Other compilers may
 * need something different.
 */

#ifdef _MSC_VER
#include <pshpack1.h>
#endif

/* The language string always has string index 0 */
static struct {
    uByte        bLength;
    uByte        bDescriptorType;
    uWord        wString[1];
} UPACKED language_string = {
    4,                      /* bLength (size of string array + 2) */
    UDESC_STRING,           /* bDescriptorType */
    {                       /* wString[] */
        UCONSTW(0x0409),    /* US English */
                            /* others can be added here */
    },
};

static struct {
    uByte        bLength;
    uByte        bDescriptorType;
    uWord        wString[8];
} UPACKED manuf_string = {
    18,                     /* bLength (size of string array + 2) */
    UDESC_STRING,           /* bDescriptorType */
    {                       /* wString[] */
        UCONSTW('A'), UCONSTW('I'), UCONSTW('V'), UCONSTW('A'),
        UCONSTW('T'), UCONSTW('E'), UCONSTW('C'), UCONSTW('H'),
    },
};
#define DWC_STRING_MANUFACTURER    1

static struct {
    uByte        bLength;
    uByte        bDescriptorType;
    uWord        wString[8];
} UPACKED product_string = {
    18,                     /* bLength (size of string array + 2) */
    UDESC_STRING,           /* bDescriptorType */
    {                       /* wString[] */
        UCONSTW('A'), UCONSTW('I'), UCONSTW('V'), UCONSTW('A'),
        UCONSTW('U'), UCONSTW('S'), UCONSTW('B'), UCONSTW('3'),
    },
};
#define DWC_STRING_PRODUCT    2

static struct {
    uByte        bLength;
    uByte        bDescriptorType;
    uWord        wString[10];
} UPACKED serial_string = {
    22,                     /* bLength (size of string array + 2) */
    UDESC_STRING,           /* bDescriptorType */
    {                       /* wString[] */
        UCONSTW('0'), UCONSTW('1'), UCONSTW('2'), UCONSTW('3'),
        UCONSTW('4'), UCONSTW('5'), UCONSTW('6'), UCONSTW('7'),
        UCONSTW('8'), UCONSTW('9'),
    },
};
#define DWC_STRING_SERIAL    3

/* Stop packing structure type definitions */
#ifdef _MSC_VER
#include <poppack.h>
#endif

/* These standard USB descriptor types are defined in usb.h */
static usb_device_descriptor_t device_desc = {
    USB_DEVICE_DESCRIPTOR_SIZE,    /* bLength */
    UDESC_DEVICE,                  /* bDescriptorType */

    UCONSTW(0),                    /* bcdUSB (filled in later) */

    0xEF,                          /* bDeviceClass: Miscellaneous*/
    2,                             /* bDeviceSubClass */
    1,                             /* bDeviceProtocol */
    64,                            /* bMaxPacketSize: 64 bytes */

    UCONSTW(0x8088),               /* idVendor */
    UCONSTW(0x0001),               /* idProduct */
    UCONSTW(0x0),                  /* bcdDevice */

    DWC_STRING_MANUFACTURER,       /* iManufacturer */
    DWC_STRING_PRODUCT,            /* iProduct */
    DWC_STRING_SERIAL,             /* iSerialNumber */

    1,                             /* bNumConfigurations */
};

static usb_device_qualifier_t dev_qualifier = {
    USB_DEVICE_QUALIFIER_SIZE,     /* bLength */
    UDESC_DEVICE_QUALIFIER,        /* bDescriptorType */

    UCONSTW(0),                    /* bcdUSB (filled in later) */

    0xEF,                          /* bDeviceClass */
    2,                             /* bDeviceSubClass */
    1,                             /* bDeviceProtocol */
    64,                            /* bMaxPacketSize0 */
    1,                             /* bNumConfigurations */
    0,                             /* bReserved */
};

/* These application-specific config descriptor types are defined in
 * no_os_defs.h
 */
/*
static fs_config_desc_t fs_config_desc = {
};
*/

static hs_config_desc_t hs_config_desc = {
    /* Configuration Descriptor Type */
    {
        0x09,                           /* Descriptor Size */
        USB_CONFIG_DESCR,               /* Configuration Descriptor Type */
        B2VAL(0),                       /* TODO: Length of this descriptor and all sub descriptors */
#ifndef USE_CDC_ACM
        0x03,                           /* Number of interfaces */
#else
        0x04,                           /* Number of interfaces */
#endif
        0x01,                           /* Configuration number */
        0x00,                           /* COnfiguration string index */
        0x80,                           /* Config characteristics - Bus powered */
        0xFA,                           /* Max power consumption of device (in 2mA unit) : 500mA */
    },
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x00,                           /* I/f number of first VideoControl i/f */
        0x02,                           /* Number of Video i/f */
        0x0E,                           /* CC_VIDEO : Video i/f class code */
        0x03,                           /* SC_VIDEO_INTERFACE_COLLECTION : Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },
    /* Standard Video Control Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x00,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x00,                           /* Number of end points, disable interrupt EP */
        0x0E,                           /* CC_VIDEO : Interface class */
        0x01,                           /* CC_VIDEOCONTROL : Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Class specific VC Interface Header Descriptor */
    {
        0x0D,                           /* Descriptor size */
        0x24,                           /* Class Specific I/f Header Descriptor type */
        0x01,                           /* Descriptor Sub type : VC_HEADER */
        0x00,0x01,                      /* Revision of class spec : 1.0 */
        0x50,0x00,                      /* Total Size of class specific descriptors (till Output terminal) */
        0x00,0x6C,0xDC,0x02,            /* Clock frequency : 48MHz(Deprecated) */
        0x01,                           /* Number of streaming interfaces */
        0x01,                           /* Video streaming I/f 1 belongs to VC i/f */
    },
    /* Input (Camera) Terminal Descriptor */
    {
        0x12,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x02,                           /* Input Terminal Descriptor type */
        0x01,                           /* ID of this terminal */
        0x01,0x02,                      /* Camera terminal type */
        0x00,                           /* No association terminal */
        0x00,                           /* String desc index : Not used */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 indicates that the mentioned Control is
                                         * supported for the video stream in the bmControls field
                                         * D0: Scanning Mode
                                         * D1: Auto-Exposure Mode
                                         * D2: Auto-Exposure Priority
                                         * D3: Exposure Time (Absolute)
                                         * D4: Exposure Time (Relative)
                                         * D5: Focus (Absolute)
                                         * D6: Focus (Relative)
                                         * D7: Iris (Absolute)
                                         * D8: Iris (Relative)
                                         * D9: Zoom (Absolute)
                                         * D10: Zoom (Relative)
                                         * D11: PanTilt (Absolute)
                                         * D12: PanTilt (Relative)
                                         * D13: Roll (Absolute)
                                         * D14: Roll (Relative)
                                         * D15: Reserved
                                         * D16: Reserved
                                         * D17: Focus, Auto
                                         * D18: Privacy
                                         * D19: Focus, Simple
                                         * D20: Window
                                         * D21: Region of Interest
                                         * D22 – D23: Reserved, set to zero
                                         */
        0x00,0x00,0x00,                 /* bmControls field of camera terminal: No controls supported */

    },

    /* Processing Unit Descriptor */
    {
        0x0C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x05,                           /* Processing Unit Descriptor type */
        0x02,                           /* ID of this terminal */
        0x01,                           /* Source ID : 1 : Conencted to input terminal */
        0x00,0x40,                      /* Digital multiplier */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 in the bmControls field indicates that
                                         * the mentioned Control is supported for the video stream.
                                         * D0: Brightness
                                         * D1: Contrast
                                         * D2: Hue
                                         * D3: Saturation
                                         * D4: Sharpness
                                         * D5: Gamma
                                         * D6: White Balance Temperature
                                         * D7: White Balance Component
                                         * D8: Backlight Compensation
                                         * D9: Gain
                                         * D10: Power Line Frequency
                                         * D11: Hue, Auto
                                         * D12: White Balance Temperature, Auto
                                         * D13: White Balance Component, Auto
                                         * D14: Digital Multiplier
                                         * D15: Digital Multiplier Limit
                                         * D16: Analog Video Standard
                                         * D17: Analog Video Lock Status
                                         * D18: Contrast, Auto
                                         * D19 – D23: Reserved. Set to zero.
                                         */
        //0x01,0x00,0x00,                 /* bmControls field of processing unit: Brightness control supported */
        0x00,0x00,0x00,                 /* bmControls field of processing unit: None */
        0x00,                           /* String desc index : Not used */
    },
    /* Extension Unit Descriptor */
    {
        0x1C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x06,                           /* Extension Unit Descriptor type */
        0x03,                           /* ID of this terminal */
        0xFF,0xFF,0xFF,0xFF,            /* 16 byte GUID */
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0x00,                           /* Number of controls in this terminal */
        0x01,                           /* Number of input pins in this terminal */
        0x02,                           /* Source ID : 2 : Connected to Proc Unit */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
        0x00,0x00,0x00,                 /*[> No controls supported <]*/
        /*0xFF,0x5D,0x00,                 No controls supported */
        0x00,                           /* String desc index : Not used */
    },

    /* Output Terminal Descriptor */
    {
        0x09,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x03,                           /* Output Terminal Descriptor type */
        0x04,                           /* ID of this terminal */
        0x01,0x01,                      /* USB Streaming terminal type */
        0x00,                           /* No association terminal */
        0x03,                           /* Source ID : 3 : Connected to Extn Unit */
        0x00,                           /* String desc index : Not used */
    },

    /* Standard Video Streaming Interface Descriptor (Alternate Setting 0) */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x01,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points : Zero Bandwidth */
        0x0E,                           /* Interface class : CC_VIDEO */
        0x02,                           /* Interface sub class : CC_VIDEOSTREAMING */
        0x00,                           /* Interface protocol code : Undefined */
        0x00,                           /* Interface descriptor string index */
    },

    /* Class-specific Video Streaming Input Header Descriptor */
    {
        0x0D+VS_NUM_FORMATS,            /* Descriptor size */
        0x24,                           /* Class-specific VS I/f Type */
        0x01,                           /* Descriptotor Subtype : Input Header */
        VS_NUM_FORMATS,                 /* 1 format desciptor follows */
        0x00,0x00,                      /* Total size of Class specific VS descr: TODO: To be filled later */
        EP_BULK_VIDEO,                  /* EP address for BULK video data */
        0x00,                           /* No dynamic format change supported */
        0x04,                           /* Output terminal ID : 4 */
        0x01,                           /* Still image capture method 1 supported */
        0x00,                           /* Hardware trigger NOT supported */
        0x00,                           /* Hardware to initiate still image capture NOT supported */
        0x01,                           /* Size of controls field : 1 byte */
#if   (VS_NUM_FORMATS == 3)
        0x00,                           /* D2 : Compression quality supported */
#elif (VS_NUM_FORMATS == 2)
        0x00,                           /* D2 : Compression quality supported */
#elif (VS_NUM_FORMATS == 1)
        0x00,                           /* D2 : Compression quality supported */
#else
# error must define VS_NUM_FORMATS!!!
#endif
    },
    {
        /**
         * Class specific Uncompressed VS format descriptor 
         * -- VS Format1: Uncompressed GRAY8
         */
        {
            0x1B,                           /* Descriptor size */
            0x24,                           /* Class-specific VS I/f Type */
            0x04,                           /* Subtype : uncompressed format I/F */
            0x01,                           /* Format desciptor index (only one format is supported) */
            VS_NUM_FMT1_FRMS,               /* number of frame descriptor followed */
            UVC_GRAY8_GUID,                 /* GUID used to identify streaming-encoding format: GRAY8  */
            0x08,                           /* Number of bits per pixel used to specify color in the decoded video frame.*/
            0x01,                           /* Optimum Frame Index for this stream: 1 */
            0x08,                           /* X dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan */
            0x06,                           /* Y dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan*/
            0x00,                           /* Interlace Flags: Progressive scanning, no interlace */
            0x00,                           /* duplication of the video stream restriction: 0 - no restriction */
        },
        {
            /* Class specific Uncompressed VS Frame descriptor (1), 640*480,  60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(1,  640, 480,  640*480),
            /* Class specific Uncompressed VS Frame descriptor (2), 1280*480, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(2, 1280, 480, 1280*480),
            /* Class specific Uncompressed VS Frame descriptor (3), 1280*1440, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(3, 1280, 1440, 1280*1440),
            /* Class specific Uncompressed VS Frame descriptor (4), 480*640, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(4,  480, 640, 480*640),
            /* Class specific Uncompressed VS Frame descriptor (5), 1600*1584, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(5, 1600, 1584, 1600*1584),
            /* Class specific Uncompressed VS Frame descriptor (6), 1920*1080, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(6, 1920, 1080, 1920*1080),	    
            /* Class specific Uncompressed VS Frame descriptor (7), 1280*800, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(7, 1280, 800, 1280*800),
            /* Class specific Uncompressed VS Frame descriptor (8), 1280*720, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(8, 1280, 720, 1280*720),	    
            /* Class specific Uncompressed VS Frame descriptor (9), 1600*1200, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(9, 1600, 1200, 1600*1200),	    
            /* Class specific Uncompressed VS Frame descriptor (10), 320*240, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(10, 320, 240, 320*240),
            /* Class specific Uncompressed VS Frame descriptor (11), 480*1280, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(11, 480, 1280, 480*1280),
            /* Class specific Uncompressed VS Frame descriptor (12), 960*640, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(12, 960, 640, 960*640),
            /* Class specific Uncompressed VS Frame descriptor (13), 1600*2400, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(13, 1600, 2400, 1600*2400),
            /* Class specific Uncompressed VS Frame descriptor (14), 1024*640, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(14, 1024, 640, 1024*640),
            /* Class specific Uncompressed VS Frame descriptor (15), 3200*1200, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(15, 3200, 1200, 3200*1200),
            /* Class specific Uncompressed VS Frame descriptor (16), 3840*1080, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(16, 3840, 1080, 3840*1080),
            /* Class specific Uncompressed VS Frame descriptor (17), 512*640, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(17,  512, 640, 512*640),
            /* Class specific Uncompressed VS Frame descriptor (18), 1024*960, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(18,  1024, 960, 1024*960),
            /* Class specific Uncompressed VS Frame descriptor (19), 1280*1952, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(19,  1280, 1952, 1280*1952),
            /* Class specific Uncompressed VS Frame descriptor (20), 1280*1920, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(20,  1280, 1920, 1280*1920),
            /* Class specific Uncompressed VS Frame descriptor (21), 2560*720, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(21,  2560, 720, 2560*720),
            /* Class specific Uncompressed VS Frame descriptor (22), 1280*960, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(22,  1280, 960, 1280*960),
            /* Class specific Uncompressed VS Frame descriptor (23), 1024*1280, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(23,  1024, 1280, 1024*1280),	    
            /* Class specific Uncompressed VS Frame descriptor (24), 1088*1280, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(24,  1088, 1280, 1088*1280),
            /* Class specific Uncompressed VS Frame descriptor (25), 640*240, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(25,  640, 240, 640*240),
            /* Class specific Uncompressed VS Frame descriptor (26), 3840*480, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(26,  3840, 480, 3840*480),
            /* Class specific Uncompressed VS Frame descriptor (27), 1920*240, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(27,  1920, 240, 1920*240),
            /* Class specific Uncompressed VS Frame descriptor (28), 1920*240, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(28,  1280, 512, 1280*512),
            /* Class specific Uncompressed VS Frame descriptor (29), 2304*1536, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(29,  2304, 1536, 2304*1536),
            /* Class specific Uncompressed VS Frame descriptor (30), 2400*1880, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(30,  2400, 1880, 2400*1880),
            /* Class specific Uncompressed VS Frame descriptor (31), 2400*1600, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(31,  2400, 1600, 2400*1600),
	},
        /* Color matching descriptor */
        {   0x06,                           /* Descriptor size */
            0x24,                           /* Descriptor type (CS_INTERFACE) */
            0x0D,                           /* Subtype: VS_COLORFORMAT */
            0x01,                           /* 1: BT.709, sRGB (default) */
            0x01,                           /* 1: BT.709 (default) */
            0x04,                           /* 1: SMPTE 170M (BT.601) */
        },
        /**
         * Class specific Uncompressed VS format descriptor 
         * -- VS Format2: Uncompressed YUYV
         */
        {
            0x1B,                           /* Descriptor size */
            0x24,                           /* Class-specific VS I/f Type */
            0x04,                           /* Subtype : uncompressed format I/F */
            0x02,                           /* Format desciptor index (only one format is supported) */
            VS_NUM_FMT2_FRMS,               /* number of frame descriptor followed */
            UVC_YUY2_GUID,                  /* GUID used to identify streaming-encoding format: YUY2  */
            0x10,                           /* Number of bits per pixel used to specify color in the decoded video frame.*/
            0x01,                           /* Optimum Frame Index for this stream: 1 */
            0x08,                           /* X dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan */
            0x06,                           /* Y dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan*/
            0x00,                           /* Interlace Flags: Progressive scanning, no interlace */
            0x00,                           /* duplication of the video stream restriction: 0 - no restriction */
        },
        {
            /* Class specific Uncompressed VS Frame descriptor (1), 640*480,  60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(1,  640, 480,  640*480),
            /* Class specific Uncompressed VS Frame descriptor (2), 1280*480, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(2, 1280, 480, 1280*480),
            /* Class specific Uncompressed VS Frame descriptor (3), 1280*720, 60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(3, 1280, 720, 1280*720),
        },
        /* Color matching descriptor */
        {   0x06,                           /* Descriptor size */
            0x24,                           /* Descriptor type (CS_INTERFACE) */
            0x0D,                           /* Subtype: VS_COLORFORMAT */
            0x01,                           /* 1: BT.709, sRGB (default) */
            0x01,                           /* 1: BT.709 (default) */
            0x04,                           /* 1: SMPTE 170M (BT.601) */
        },
        /**
         * Class specific MJPEG VS format descriptor 
         * -- VS Format3: MJPEG
         */
        {
            0x0B,                           /* Descriptor size */
            0x24,                           /* Class-specific VS i/f type */
            0x06,                           /* Descriptotor subtype : VS_FORMAT_MJPEG */
            0x03,                           /* Format desciptor index */
            VS_NUM_FMT3_FRMS,               /* 1 Frame desciptor follows */
            0x01,                           /* Uses fixed size samples */
            0x01,                           /* Default frame index is 1 */
            0x00,                           /* Non interlaced stream not reqd. */
            0x00,                           /* Non interlaced stream not reqd. */
            0x00,                           /* Non interlaced stream */
            0x00,                           /* CopyProtect: duplication unrestricted */
        },
        {
            /* Class specific MJPEG VS Frame descriptor (1),  640*480,  60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(1,  640,  480,  640*480),
            /* Class specific MJPEG VS Frame descriptor (2), 1280*480,  60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(2, 1280,  480, 1280*480),
            /* Class specific MJPEG VS Frame descriptor (3), 1600*1200, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(3, 1600, 1200, 1600*1200),
            /* Class specific MJPEG VS Frame descriptor (4), 1600*2400, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(4, 1600, 2400, 1600*2400),
            /* Class specific MJPEG VS Frame descriptor (5), 1280*1440, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(5, 1280, 1440, 1280*1440),
            /* Class specific MJPEG VS Frame descriptor (6), 1920*1080, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(6, 1920, 1080, 1920*1080),	    
            /* Class specific MJPEG VS Frame descriptor (7), 1280*800, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(7, 1280, 800, 1280*800),
            /* Class specific MJPEG VS Frame descriptor (8), 1280*720, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(8, 1280, 720, 1280*720),
            /* Class specific MJPEG VS Frame descriptor (9), 320*240, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(9, 320, 240, 320*240),
            /* Class specific MJPEG VS Frame descriptor (10), 640*240, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(10, 640, 240, 640*240),
            /* Class specific MJPEG VS Frame descriptor (11), 1280*960, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(11, 1280, 960, 1280*960),
            /* Class specific MJPEG VS Frame descriptor (12), 480*640, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(12, 480, 640, 480*640),
            /* Class specific MJPEG VS Frame descriptor (13), 960*640, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(13, 960, 640, 960*640),
            /* Class specific MJPEG VS Frame descriptor (14), 1024*640, 60fps/30fps/15fps */
            VS_MJPEG_FRAME_DESC(14, 1024, 640, 1024*640),
	    },
    },
    /* Endpoint Descriptor for BULK Streaming Video Data */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint Descriptor Type */
        EP_BULK_VIDEO,                  /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK End point */
        B2VAL(512),                     /* High speed max packet size is always 512 bytes. */
        0x01,                           /* Servicing interval for data transfers */
    },

#ifndef USE_CDC_ACM
    /* Interface descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x02,
        0x00,                           /* Alternate setting number */
        0x02,                           /* Number of end points */
        0xFF,                           /* Interface class: class vendor */
        0x00,                           /* Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },

    /* Endpoint descriptor for ext_ctrl OUT EP */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_CMD,                /* Endpoint address and description */
        USB_EP_BULK,                    /* Bulk endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers : 0 for bulk */
    },

    /* Endpoint descriptor for ext_resp IN EP */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_RSP,                /* Endpoint address and description */
        USB_EP_BULK,                    /* Bulk endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers : 0 for Bulk */
    },
#else
    // CDC ACM start!
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x02,                           /* TODO: I/f number of first i/f */
        0x02,                           /* Number of Video i/f */
        0x02,                           /* bFunctionClass: CDC */
        0x02,                           /* Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },

    /* Interface descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x02,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points */
        0x02,                           /* Interface class: communication */
        USB_CDC_SUBCLASS_ACM,           /* Interface sub class: ACM */
        0x01,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* CDC Class-specific Descriptors */
    // TODO check following code
    /* Header functional Descriptor */
    {
        0x05,                           /* Descriptors length(5) */
        0x24,                           /* Descriptor type : CS_Interface */
        0x00,                           /* DescriptorSubType : Header Functional Descriptor */
        0x10,                           /* bcdCDC : CDC Release Number, V1.1 */
        0x01,
    },
    /* Abstract Control Management Functional Descriptor */
    {
        0x04,                           /* Descriptors Length (4) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x02,                           /* bDescriptorSubType: Abstract Control Model Functional Descriptor */
        0x00,                           /* bmCapabilities: Supports the request combination of Set_Line_Coding,
                                        Set_Control_Line_State, Get_Line_Coding and the notification Serial_State */
    },
    /* Union Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x06,                           /* bDescriptorSubType: Union Functional Descriptor */
        0x02,                           /* TODO: bMasterInterface */
        0x03,                           /* TODO: bSlaveInterface */
    },
    /* Call Management Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x01,                           /* bDescriptorSubType: Call Management Functional Descriptor */
        0x00,                           /* bmCapabilities: Device sends/receives call management information
                                           only over the Communication Class Interface. */
        0x03,                           /* TODO: Interface Number of Data Class interface */
    },
    /* Endpoint descriptor for interrupt IN EP */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_CONTROL_STATUS,              /* Endpoint address and description */
        USB_EP_INTR,                    /* Bulk endpoint type */
        B2VAL(16),                      /* Max packet size = 16 bytes */
        0x08,                           /* Servicing interval for data transfers : 0 for bulk */
    },
    /* Data Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x03,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x02,                           /* Number of endpoints */
        0x0A,                           /* Interface class: Data interface */
        0x00,                           /* Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Endpoint Descriptor(BULK-IN) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_RSP,                /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    },
    /* Endpoint Descriptor(BULK-OUT) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_CMD,                /* Endpoint address and description */
        USB_EP_BULK,                    /* Bulk endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    }
#endif // #ifndef USE_CDC_ACM
};

static hs_config_mjpeg_desc_t hs_mjpeg_config_desc = {
    /* Configuration Descriptor Type */
    {
        0x09,                           /* Descriptor Size */
        USB_CONFIG_DESCR,               /* Configuration Descriptor Type */
        B2VAL(0),                       /* TODO: Length of this descriptor and all sub descriptors */
        0x04,                           /* Number of interfaces */
        0x01,                           /* Configuration number */
        0x00,                           /* COnfiguration string index */
        0x80,                           /* Config characteristics - Bus powered */
        0xFA,                           /* Max power consumption of device (in 2mA unit) : 500mA */
    },
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x00,                           /* I/f number of first VideoControl i/f */
        0x02,                           /* Number of Video i/f */
        0x0E,                           /* CC_VIDEO : Video i/f class code */
        0x03,                           /* SC_VIDEO_INTERFACE_COLLECTION : Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },
    /* Standard Video Control Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x00,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x00,                           /* Number of end points, disable interrupt EP */
        0x0E,                           /* CC_VIDEO : Interface class */
        0x01,                           /* CC_VIDEOCONTROL : Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Class specific VC Interface Header Descriptor */
    {
        0x0D,                           /* Descriptor size */
        0x24,                           /* Class Specific I/f Header Descriptor type */
        0x01,                           /* Descriptor Sub type : VC_HEADER */
        0x00,0x01,                      /* Revision of class spec : 1.0 */
        0x50,0x00,                      /* Total Size of class specific descriptors (till Output terminal) */
        0x00,0x6C,0xDC,0x02,            /* Clock frequency : 48MHz(Deprecated) */
        0x01,                           /* Number of streaming interfaces */
        0x01,                           /* Video streaming I/f 1 belongs to VC i/f */
    },
    /* Input (Camera) Terminal Descriptor */
    {
        0x12,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x02,                           /* Input Terminal Descriptor type */
        0x01,                           /* ID of this terminal */
        0x01,0x02,                      /* Camera terminal type */
        0x00,                           /* No association terminal */
        0x00,                           /* String desc index : Not used */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 indicates that the mentioned Control is
                                         * supported for the video stream in the bmControls field
                                         * D0: Scanning Mode
                                         * D1: Auto-Exposure Mode
                                         * D2: Auto-Exposure Priority
                                         * D3: Exposure Time (Absolute)
                                         * D4: Exposure Time (Relative)
                                         * D5: Focus (Absolute)
                                         * D6: Focus (Relative)
                                         * D7: Iris (Absolute)
                                         * D8: Iris (Relative)
                                         * D9: Zoom (Absolute)
                                         * D10: Zoom (Relative)
                                         * D11: PanTilt (Absolute)
                                         * D12: PanTilt (Relative)
                                         * D13: Roll (Absolute)
                                         * D14: Roll (Relative)
                                         * D15: Reserved
                                         * D16: Reserved
                                         * D17: Focus, Auto
                                         * D18: Privacy
                                         * D19: Focus, Simple
                                         * D20: Window
                                         * D21: Region of Interest
                                         * D22 – D23: Reserved, set to zero
                                         */
        0x00,0x00,0x00,                 /* bmControls field of camera terminal: No controls supported */

    },

    /* Processing Unit Descriptor */
    {
        0x0C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x05,                           /* Processing Unit Descriptor type */
        0x02,                           /* ID of this terminal */
        0x01,                           /* Source ID : 1 : Conencted to input terminal */
        0x00,0x40,                      /* Digital multiplier */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 in the bmControls field indicates that
                                         * the mentioned Control is supported for the video stream.
                                         * D0: Brightness
                                         * D1: Contrast
                                         * D2: Hue
                                         * D3: Saturation
                                         * D4: Sharpness
                                         * D5: Gamma
                                         * D6: White Balance Temperature
                                         * D7: White Balance Component
                                         * D8: Backlight Compensation
                                         * D9: Gain
                                         * D10: Power Line Frequency
                                         * D11: Hue, Auto
                                         * D12: White Balance Temperature, Auto
                                         * D13: White Balance Component, Auto
                                         * D14: Digital Multiplier
                                         * D15: Digital Multiplier Limit
                                         * D16: Analog Video Standard
                                         * D17: Analog Video Lock Status
                                         * D18: Contrast, Auto
                                         * D19 – D23: Reserved. Set to zero.
                                         */
        //0x01,0x00,0x00,                 /* bmControls field of processing unit: Brightness control supported */
        0x00,0x00,0x00,                 /* bmControls field of processing unit: None */
        0x00,                           /* String desc index : Not used */
    },
    /* Extension Unit Descriptor */
    {
        0x1C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x06,                           /* Extension Unit Descriptor type */
        0x03,                           /* ID of this terminal */
        0xFF,0xFF,0xFF,0xFF,            /* 16 byte GUID */
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0x00,                           /* Number of controls in this terminal */
        0x01,                           /* Number of input pins in this terminal */
        0x02,                           /* Source ID : 2 : Connected to Proc Unit */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
        0x00,0x00,0x00,                 /*[> No controls supported <]*/
        /*0xFF,0x5D,0x00,               No controls supported */
        0x00,                           /* String desc index : Not used */
    },

    /* Output Terminal Descriptor */
    {
        0x09,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x03,                           /* Output Terminal Descriptor type */
        0x04,                           /* ID of this terminal */
        0x01,0x01,                      /* USB Streaming terminal type */
        0x00,                           /* No association terminal */
        0x03,                           /* Source ID : 3 : Connected to Extn Unit */
        0x00,                           /* String desc index : Not used */
    },

    /* Standard Video Streaming Interface Descriptor (Alternate Setting 0) */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x01,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points : Zero Bandwidth */
        0x0E,                           /* Interface class : CC_VIDEO */
        0x02,                           /* Interface sub class : CC_VIDEOSTREAMING */
        0x00,                           /* Interface protocol code : Undefined */
        0x00,                           /* Interface descriptor string index */
    },

    /* Class-specific Video Streaming Input Header Descriptor */
    {
        0x0D+0x01,                      /* Descriptor size */
        0x24,                           /* Class-specific VS I/f Type */
        0x01,                           /* Descriptotor Subtype : Input Header */
        0x01,                           /* 1 format desciptor follows */
        0x00,0x00,                      /* Total size of Class specific VS descr: TODO: To be filled later */
        EP_BULK_VIDEO,                  /* EP address for BULK video data */
        0x00,                           /* No dynamic format change supported */
        0x04,                           /* Output terminal ID : 4 */
        0x01,                           /* Still image capture method 1 supported */
        0x00,                           /* Hardware trigger NOT supported */
        0x00,                           /* Hardware to initiate still image capture NOT supported */
        0x01,                           /* Size of controls field : 1 byte */
        0x00,                           /* D2 : Compression quality supported */
    },
    {   
        /**
         * Class specific MJPEG VS format descriptor 
         * -- VS Format1: MJPEG
         */
        {
            0x0B,                           /* Descriptor size */
            0x24,                           /* Class-specific VS i/f type */
            0x06,                           /* Descriptotor subtype : VS_FORMAT_MJPEG */
            0x01,                           /* Format desciptor index */
            VS_NUM_MJPEG_FMT1_FRMS,         /* 1 Frame desciptor follows */
            0x01,                           /* Uses fixed size samples */
            0x01,                           /* Default frame index is 1 */
            0x00,                           /* Non interlaced stream not reqd. */
            0x00,                           /* Non interlaced stream not reqd. */
            0x00,                           /* Non interlaced stream */
            0x00,                           /* CopyProtect: duplication unrestricted */
        },
        {
            /* Class specific MJPEG VS Frame descriptor (1), 1600*1200, 30fps */
            VS_MJPEG_FRAME_DESC(1, 1600, 1200, 1600*1200),
        },
    },
    /* Endpoint Descriptor for BULK Streaming Video Data */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint Descriptor Type */
        EP_BULK_VIDEO,                  /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK End point */
        B2VAL(512),                     /* High speed max packet size is always 512 bytes. */
        0x01,                           /* Servicing interval for data transfers */
    },

    // CDC ACM start!
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x02,                           /* TODO: I/f number of first i/f */
        0x02,                           /* Number of Video i/f */
        0x02,                           /* bFunctionClass: CDC */
        0x02,                           /* Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },

    /* Interface descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x02,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points */
        0x02,                           /* Interface class: communication */
        USB_CDC_SUBCLASS_ACM,           /* Interface sub class: ACM */
        0x01,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* CDC Class-specific Descriptors */
    // TODO check following code
    /* Header functional Descriptor */
    {
        0x05,                           /* Descriptors length(5) */
        0x24,                           /* Descriptor type : CS_Interface */
        0x00,                           /* DescriptorSubType : Header Functional Descriptor */
        0x10,                           /* bcdCDC : CDC Release Number, V1.1 */
        0x01,
    },
    /* Abstract Control Management Functional Descriptor */
    {
        0x04,                           /* Descriptors Length (4) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x02,                           /* bDescriptorSubType: Abstract Control Model Functional Descriptor */
        0x00,                           /* bmCapabilities: Supports the request combination of Set_Line_Coding,
                                        Set_Control_Line_State, Get_Line_Coding and the notification Serial_State */
    },
    /* Union Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x06,                           /* bDescriptorSubType: Union Functional Descriptor */
        0x02,                           /* TODO: bMasterInterface */
        0x03,                           /* TODO: bSlaveInterface */
    },
    /* Call Management Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x01,                           /* bDescriptorSubType: Call Management Functional Descriptor */
        0x00,                           /* bmCapabilities: Device sends/receives call management information
                                           only over the Communication Class Interface. */
        0x03,                           /* TODO: Interface Number of Data Class interface */
    },
    /* Endpoint descriptor for interrupt IN EP */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_CONTROL_STATUS,              /* Endpoint address and description */
        USB_EP_INTR,                    /* Bulk endpoint type */
        B2VAL(16),                      /* Max packet size = 16 bytes */
        0x08,                           /* Servicing interval for data transfers : 0 for bulk */
    },
    /* Data Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x03,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x02,                           /* Number of endpoints */
        0x0A,                           /* Interface class: Data interface */
        0x00,                           /* Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Endpoint Descriptor(BULK-IN) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_RSP,                /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    },
    /* Endpoint Descriptor(BULK-OUT) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_CMD,                /* Endpoint address and description */
        USB_EP_BULK,                    /* Bulk endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    }
};

static hs_config_yuyv_desc_t hs_yuyv_config_desc = {
    /* Configuration Descriptor Type */
    {
        0x09,                           /* Descriptor Size */
        USB_CONFIG_DESCR,               /* Configuration Descriptor Type */
        B2VAL(0),                       /* TODO: Length of this descriptor and all sub descriptors */
        0x04,                           /* Number of interfaces */
        0x01,                           /* Configuration number */
        0x00,                           /* COnfiguration string index */
        0x80,                           /* Config characteristics - Bus powered */
        0xFA,                           /* Max power consumption of device (in 2mA unit) : 500mA */
    },
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x00,                           /* I/f number of first VideoControl i/f */
        0x02,                           /* Number of Video i/f */
        0x0E,                           /* CC_VIDEO : Video i/f class code */
        0x03,                           /* SC_VIDEO_INTERFACE_COLLECTION : Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },
    /* Standard Video Control Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x00,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x00,                           /* Number of end points, disable interrupt EP */
        0x0E,                           /* CC_VIDEO : Interface class */
        0x01,                           /* CC_VIDEOCONTROL : Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Class specific VC Interface Header Descriptor */
    {
        0x0D,                           /* Descriptor size */
        0x24,                           /* Class Specific I/f Header Descriptor type */
        0x01,                           /* Descriptor Sub type : VC_HEADER */
        0x00,0x01,                      /* Revision of class spec : 1.0 */
        0x50,0x00,                      /* Total Size of class specific descriptors (till Output terminal) */
        0x00,0x6C,0xDC,0x02,            /* Clock frequency : 48MHz(Deprecated) */
        0x01,                           /* Number of streaming interfaces */
        0x01,                           /* Video streaming I/f 1 belongs to VC i/f */
    },
    /* Input (Camera) Terminal Descriptor */
    {
        0x12,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x02,                           /* Input Terminal Descriptor type */
        0x01,                           /* ID of this terminal */
        0x01,0x02,                      /* Camera terminal type */
        0x00,                           /* No association terminal */
        0x00,                           /* String desc index : Not used */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x00,0x00,                      /* No optical zoom supported */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 indicates that the mentioned Control is
                                         * supported for the video stream in the bmControls field
                                         * D0: Scanning Mode
                                         * D1: Auto-Exposure Mode
                                         * D2: Auto-Exposure Priority
                                         * D3: Exposure Time (Absolute)
                                         * D4: Exposure Time (Relative)
                                         * D5: Focus (Absolute)
                                         * D6: Focus (Relative)
                                         * D7: Iris (Absolute)
                                         * D8: Iris (Relative)
                                         * D9: Zoom (Absolute)
                                         * D10: Zoom (Relative)
                                         * D11: PanTilt (Absolute)
                                         * D12: PanTilt (Relative)
                                         * D13: Roll (Absolute)
                                         * D14: Roll (Relative)
                                         * D15: Reserved
                                         * D16: Reserved
                                         * D17: Focus, Auto
                                         * D18: Privacy
                                         * D19: Focus, Simple
                                         * D20: Window
                                         * D21: Region of Interest
                                         * D22 – D23: Reserved, set to zero
                                         */
        0x00,0x00,0x00,                 /* bmControls field of camera terminal: No controls supported */

    },

    /* Processing Unit Descriptor */
    {
        0x0C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x05,                           /* Processing Unit Descriptor type */
        0x02,                           /* ID of this terminal */
        0x01,                           /* Source ID : 1 : Conencted to input terminal */
        0x00,0x40,                      /* Digital multiplier */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
                                        /* A bit set to 1 in the bmControls field indicates that
                                         * the mentioned Control is supported for the video stream.
                                         * D0: Brightness
                                         * D1: Contrast
                                         * D2: Hue
                                         * D3: Saturation
                                         * D4: Sharpness
                                         * D5: Gamma
                                         * D6: White Balance Temperature
                                         * D7: White Balance Component
                                         * D8: Backlight Compensation
                                         * D9: Gain
                                         * D10: Power Line Frequency
                                         * D11: Hue, Auto
                                         * D12: White Balance Temperature, Auto
                                         * D13: White Balance Component, Auto
                                         * D14: Digital Multiplier
                                         * D15: Digital Multiplier Limit
                                         * D16: Analog Video Standard
                                         * D17: Analog Video Lock Status
                                         * D18: Contrast, Auto
                                         * D19 – D23: Reserved. Set to zero.
                                         */
        //0x01,0x00,0x00,                 /* bmControls field of processing unit: Brightness control supported */
        0x00,0x00,0x00,                 /* bmControls field of processing unit: None */
        0x00,                           /* String desc index : Not used */
    },
    /* Extension Unit Descriptor */
    {
        0x1C,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x06,                           /* Extension Unit Descriptor type */
        0x03,                           /* ID of this terminal */
        0xFF,0xFF,0xFF,0xFF,            /* 16 byte GUID */
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,
        0x00,                           /* Number of controls in this terminal */
        0x01,                           /* Number of input pins in this terminal */
        0x02,                           /* Source ID : 2 : Connected to Proc Unit */
        0x03,                           /* Size of controls field for this terminal : 3 bytes */
        0x00,0x00,0x00,                 /*[> No controls supported <]*/
        /*0xFF,0x5D,0x00,               No controls supported */
        0x00,                           /* String desc index : Not used */
    },

    /* Output Terminal Descriptor */
    {
        0x09,                           /* Descriptor size */
        0x24,                           /* Class specific interface desc type */
        0x03,                           /* Output Terminal Descriptor type */
        0x04,                           /* ID of this terminal */
        0x01,0x01,                      /* USB Streaming terminal type */
        0x00,                           /* No association terminal */
        0x03,                           /* Source ID : 3 : Connected to Extn Unit */
        0x00,                           /* String desc index : Not used */
    },

    /* Standard Video Streaming Interface Descriptor (Alternate Setting 0) */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x01,                           /* Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points : Zero Bandwidth */
        0x0E,                           /* Interface class : CC_VIDEO */
        0x02,                           /* Interface sub class : CC_VIDEOSTREAMING */
        0x00,                           /* Interface protocol code : Undefined */
        0x00,                           /* Interface descriptor string index */
    },

    /* Class-specific Video Streaming Input Header Descriptor */
    {
        0x0D+0x01,                      /* Descriptor size */
        0x24,                           /* Class-specific VS I/f Type */
        0x01,                           /* Descriptotor Subtype : Input Header */
        0x01,                           /* 1 format desciptor follows */
        0x00,0x00,                      /* Total size of Class specific VS descr: TODO: To be filled later */
        EP_BULK_VIDEO,                  /* EP address for BULK video data */
        0x00,                           /* No dynamic format change supported */
        0x04,                           /* Output terminal ID : 4 */
        0x01,                           /* Still image capture method 1 supported */
        0x00,                           /* Hardware trigger NOT supported */
        0x00,                           /* Hardware to initiate still image capture NOT supported */
        0x01,                           /* Size of controls field : 1 byte */
        0x00,                           /* D2 : Compression quality supported */
    },
    {   
        /**
         * Class specific Uncompressed VS format descriptor 
         * -- VS Format1: Uncompressed YUYV
         */
        {
            0x1B,                           /* Descriptor size */
            0x24,                           /* Class-specific VS I/f Type */
            0x04,                           /* Subtype : uncompressed format I/F */
            0x01,                           /* Format desciptor index (only one format is supported) */
            VS_NUM_YUYV_FMT1_FRMS,          /* number of frame descriptor followed */
            UVC_YUY2_GUID,                  /* GUID used to identify streaming-encoding format: YUY2  */
            0x10,                           /* Number of bits per pixel used to specify color in the decoded video frame.*/
            0x01,                           /* Optimum Frame Index for this stream: 1 */
            0x08,                           /* X dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan */
            0x06,                           /* Y dimension of the picture aspect ratio: Non-interlaced in
                                               progressive scan*/
            0x00,                           /* Interlace Flags: Progressive scanning, no interlace */
            0x00,                           /* duplication of the video stream restriction: 0 - no restriction */
        },
        {
            /* Class specific Uncompressed VS Frame descriptor (1), 800*600,  60fps/30fps/15fps */
            VS_UNCOMP_FRAME_DESC(1,  800, 600,  800*600*2),
        },
        /* Color matching descriptor */
        {   0x06,                           /* Descriptor size */
            0x24,                           /* Descriptor type (CS_INTERFACE) */
            0x0D,                           /* Subtype: VS_COLORFORMAT */
            0x01,                           /* 1: BT.709, sRGB (default) */
            0x01,                           /* 1: BT.709 (default) */
            0x04,                           /* 1: SMPTE 170M (BT.601) */
        },
    },
    /* Endpoint Descriptor for BULK Streaming Video Data */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint Descriptor Type */
        EP_BULK_VIDEO,                  /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK End point */
        B2VAL(512),                     /* High speed max packet size is always 512 bytes. */
        0x01,                           /* Servicing interval for data transfers */
    },

    // CDC ACM start!
    /* Interface Association Descriptor */
    {
        0x08,                           /* Descriptor Size */
        INTF_ASSN_DSCR_TYPE,            /* Interface Association Descr Type: 11 */
        0x02,                           /* TODO: I/f number of first i/f */
        0x02,                           /* Number of Video i/f */
        0x02,                           /* bFunctionClass: CDC */
        0x02,                           /* Subclass code */
        0x00,                           /* Protocol : Not used */
        0x00,                           /* String desc index for interface */
    },

    /* Interface descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x02,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x01,                           /* Number of end points */
        0x02,                           /* Interface class: communication */
        USB_CDC_SUBCLASS_ACM,           /* Interface sub class: ACM */
        0x01,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* CDC Class-specific Descriptors */
    // TODO check following code
    /* Header functional Descriptor */
    {
        0x05,                           /* Descriptors length(5) */
        0x24,                           /* Descriptor type : CS_Interface */
        0x00,                           /* DescriptorSubType : Header Functional Descriptor */
        0x10,                           /* bcdCDC : CDC Release Number, V1.1 */
        0x01,
    },
    /* Abstract Control Management Functional Descriptor */
    {
        0x04,                           /* Descriptors Length (4) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x02,                           /* bDescriptorSubType: Abstract Control Model Functional Descriptor */
        0x00,                           /* bmCapabilities: Supports the request combination of Set_Line_Coding,
                                        Set_Control_Line_State, Get_Line_Coding and the notification Serial_State */
    },
    /* Union Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x06,                           /* bDescriptorSubType: Union Functional Descriptor */
        0x02,                           /* TODO: bMasterInterface */
        0x03,                           /* TODO: bSlaveInterface */
    },
    /* Call Management Functional Descriptor */
    {
        0x05,                           /* Descriptors Length (5) */
        0x24,                           /* bDescriptorType: CS_INTERFACE */
        0x01,                           /* bDescriptorSubType: Call Management Functional Descriptor */
        0x00,                           /* bmCapabilities: Device sends/receives call management information
                                           only over the Communication Class Interface. */
        0x03,                           /* TODO: Interface Number of Data Class interface */
    },
    /* Endpoint descriptor for interrupt IN EP */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_CONTROL_STATUS,              /* Endpoint address and description */
        USB_EP_INTR,                    /* Bulk endpoint type */
        B2VAL(16),                      /* Max packet size = 16 bytes */
        0x08,                           /* Servicing interval for data transfers : 0 for bulk */
    },
    /* Data Interface Descriptor */
    {
        0x09,                           /* Descriptor size */
        USB_INTRFC_DESCR,               /* Interface Descriptor type */
        0x03,                           /* TODO: Interface number */
        0x00,                           /* Alternate setting number */
        0x02,                           /* Number of endpoints */
        0x0A,                           /* Interface class: Data interface */
        0x00,                           /* Interface sub class */
        0x00,                           /* Interface protocol code */
        0x00,                           /* Interface descriptor string index */
    },
    /* Endpoint Descriptor(BULK-IN) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_RSP,                /* Endpoint address and description */
        USB_EP_BULK,                    /* BULK endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    },
    /* Endpoint Descriptor(BULK-OUT) */
    {
        0x07,                           /* Descriptor size */
        USB_ENDPNT_DESCR,               /* Endpoint descriptor type */
        EP_EXT_CTRL_CMD,                /* Endpoint address and description */
        USB_EP_BULK,                    /* Bulk endpoint type */
        B2VAL(512),                     /* Max packet size = 512 bytes */
        0x00,                           /* Servicing interval for data transfers */
    }
};

/*
static ss_config_desc_t ss_config_desc = {
};
*/

/*static void ep0_complete(usb_ep_t *ep, usb_request_t *req)*/
/*{*/
    /*if (req->status || req->actual != req->length) {*/
        /*LOGW(__func__, "setup complete --> %d, %d/%d", req->status, req->actual, req->length);*/
    /*}*/
/*}*/

int uvc_camera_get_desc(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    int ret;
    u8 desc_type = UGETW(ctrl->wValue) >> 8;
    u8 desc_idx = UGETW(ctrl->wValue);
    u16 max = UGETW(ctrl->wLength);
    u32 len;
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    u8 *buf = req->buf;
    configASSERT(buf != NULL);
    struct usb_ep *ep0 = gadget->ep0;
    int ep0_desc_sel = 0;

    LOGD(__func__, "GET_DESCRIPTOR %02x.%02x v%04x i%04x l%04x",
           ctrl->bmRequestType, ctrl->bRequest, UGETW(ctrl->wValue),
           UGETW(ctrl->wIndex), UGETW(ctrl->wLength));

    req->zero = 0;
    req->length = 0;
    /*req->complete = ep0_complete;*/
    
    if (gadget->priv != NULL) {
        ep0_desc_sel = *(int *)gadget->priv;
        /*LOGD(__func__, "gadget->priv is 0x%x, ep0_desc_sel is %d", gadget->priv, ep0_desc_sel);*/
    }

    switch (desc_type) {
        case UDESC_DEVICE:
            if (desc_idx) {
                return -DWC_E_NOT_SUPPORTED;
            }
            // set bMaxPacketSize
            USETW(device_desc.bcdUSB, 0x200);
            device_desc.bMaxPacketSize = 64;
            // copy device descriptor
            aiva_memcpy(buf, (const uint8_t *)&device_desc, sizeof(device_desc));
            len = sizeof(device_desc);
            break;

        case UDESC_CONFIG:
            if (desc_idx) {
                return -DWC_E_NOT_SUPPORTED;
            }

            if (ep0_desc_sel == 1) {
                // Only one format: MJPEG
                USETW(hs_mjpeg_config_desc.config_desc.wTotalLength,
                      sizeof(hs_mjpeg_config_desc));
                USETW(hs_mjpeg_config_desc.vs_input_hdr_desc.wTotalLength,
                      sizeof(hs_mjpeg_config_desc.vs_input_hdr_desc) + 
                      sizeof(hs_mjpeg_config_desc.vs_frames_formats_desc)
                      );
                // copy config descriptor
                aiva_memcpy(buf, (const uint8_t *)&hs_mjpeg_config_desc, sizeof(hs_mjpeg_config_desc));
                len = sizeof(hs_mjpeg_config_desc);
            }
            else if (ep0_desc_sel == 2) {
                // Only one format: YUYV 
                USETW(hs_yuyv_config_desc.config_desc.wTotalLength,
                      sizeof(hs_yuyv_config_desc));
                USETW(hs_yuyv_config_desc.vs_input_hdr_desc.wTotalLength,
                      sizeof(hs_yuyv_config_desc.vs_input_hdr_desc) + 
                      sizeof(hs_yuyv_config_desc.vs_frames_formats_desc)
                      );
                // copy config descriptor
                aiva_memcpy(buf, (const uint8_t *)&hs_yuyv_config_desc, sizeof(hs_yuyv_config_desc));
                len = sizeof(hs_yuyv_config_desc);
            }
            else {
                USETW(hs_config_desc.config_desc.wTotalLength,
                      sizeof(hs_config_desc));
                USETW(hs_config_desc.vs_input_hdr_desc.wTotalLength,
                      sizeof(hs_config_desc.vs_input_hdr_desc) + 
                      sizeof(hs_config_desc.vs_frames_formats_desc)
                      );
                // copy config descriptor
                aiva_memcpy(buf, (const uint8_t *)&hs_config_desc, sizeof(hs_config_desc));
                len = sizeof(hs_config_desc);
            }
            break;

        case UDESC_STRING:
            switch (desc_idx) {
                case 0:
                    aiva_memcpy(buf, (const uint8_t *)&language_string, sizeof(language_string));
                    len = sizeof(language_string);
                    break;

                case DWC_STRING_MANUFACTURER:
                    aiva_memcpy(buf, (const uint8_t *)&manuf_string, sizeof(manuf_string));
                    len = sizeof(manuf_string);
                    break;

                case DWC_STRING_PRODUCT:
                    aiva_memcpy(buf, (const uint8_t *)&product_string, sizeof(product_string));
                    len = sizeof(product_string);
                    break;

                case DWC_STRING_SERIAL:
                    aiva_memcpy(buf, (const uint8_t *)&serial_string, sizeof(serial_string));
                    len = sizeof(serial_string);
                    break;

                default:
                    return -DWC_E_NOT_SUPPORTED;
            }
            break;

        case UDESC_DEVICE_QUALIFIER:
            USETW(dev_qualifier.bcdUSB, 0x200);
            /*dev_qualifier.bMaxPacketSize0 = 9;*/
            aiva_memcpy(buf, (const uint8_t *)&dev_qualifier, sizeof(dev_qualifier));
            len = sizeof(dev_qualifier);
            break;

        case UDESC_OTHER_SPEED_CONFIGURATION:
            if (ep0_desc_sel == 1) {
                // Only one format: MJPEG
                hs_mjpeg_config_desc.config_desc.bDescriptorType = UDESC_OTHER_SPEED_CONFIGURATION;
                USETW(hs_mjpeg_config_desc.config_desc.wTotalLength, sizeof(hs_mjpeg_config_desc));
                aiva_memcpy(buf, (const uint8_t *)&hs_mjpeg_config_desc, sizeof(hs_mjpeg_config_desc));
                hs_mjpeg_config_desc.config_desc.bDescriptorType = UDESC_CONFIG;
                len = sizeof(hs_mjpeg_config_desc);
            }
            else {
                hs_config_desc.config_desc.bDescriptorType = UDESC_OTHER_SPEED_CONFIGURATION;
                USETW(hs_config_desc.config_desc.wTotalLength, sizeof(hs_config_desc));
                aiva_memcpy(buf, (const uint8_t *)&hs_config_desc, sizeof(hs_config_desc));
                hs_config_desc.config_desc.bDescriptorType = UDESC_CONFIG;
                len = sizeof(hs_config_desc);
            }
            break;

        default:
            /*configASSERT(!"unsupport desc_type");*/
            LOGE(__func__, "Unsupported desc_type 0x%x", desc_type);
            return -DWC_E_NOT_SUPPORTED;
    }

    configASSERT((uint32_t)len <= USB_EP0_BUF_SIZE);
    /*dcache_flush(buf, len);*/

    req->length = len < max ? len : max;

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

static struct usb_ep *uvc_get_video_in_ep(struct usb_gadget *gadget)
{
    // EP2-IN bulk
    return gadget_find_eq_by_name(gadget, "ep2in");
}

static struct usb_ep *uvc_get_ext_cmd_out_ep(struct usb_gadget *gadget)
{
    // EP3-OUT bulk
    return gadget_find_eq_by_name(gadget, "ep3out");
}

static struct usb_ep *uvc_get_ext_resp_in_ep(struct usb_gadget *gadget)
{
    // EP3-IN bulk
    return gadget_find_eq_by_name(gadget, "ep3in");
}

int uvc_camera_enable_eps(struct usb_gadget *gadget, f_uvc_camera_t *uvc)
{
    int ret;
    int ep0_desc_sel = 0;

    uvc->video_in_ep = uvc_get_video_in_ep(gadget);
    configASSERT(uvc->video_in_ep != NULL);

    uvc->ext_cmd_out_ep = uvc_get_ext_cmd_out_ep(gadget);
    configASSERT(uvc->ext_cmd_out_ep != NULL);

    uvc->ext_resp_in_ep = uvc_get_ext_resp_in_ep(gadget);
    configASSERT(uvc->ext_resp_in_ep != NULL);
    
    if (gadget->priv != NULL) {
        ep0_desc_sel = *(int *)gadget->priv;
        LOGD(__func__, "gadget->priv is 0x%x, ep0_desc_sel is %d", gadget->priv, ep0_desc_sel);
    }

    if (ep0_desc_sel == 1) {
        // Only one format: MJPEG
        ret = uvc->video_in_ep->ops->enable(uvc->video_in_ep, &hs_mjpeg_config_desc.bulk_video_in_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable video_in_ep error!");
            goto cleanup;
        }
        ret = uvc->ext_cmd_out_ep->ops->enable(uvc->ext_cmd_out_ep, &hs_mjpeg_config_desc.bulk_ctrl_out_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable ext_cmd_out_ep error!");
            goto cleanup;
        }
        ret = uvc->ext_resp_in_ep->ops->enable(uvc->ext_resp_in_ep, &hs_mjpeg_config_desc.bulk_resp_in_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable ext_cmd_out_ep error!");
            goto cleanup;
        }
    }
    else {
        ret = uvc->video_in_ep->ops->enable(uvc->video_in_ep, &hs_config_desc.bulk_video_in_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable video_in_ep error!");
            goto cleanup;
        }
        ret = uvc->ext_cmd_out_ep->ops->enable(uvc->ext_cmd_out_ep, &hs_config_desc.bulk_ctrl_out_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable ext_cmd_out_ep error!");
            goto cleanup;
        }
        ret = uvc->ext_resp_in_ep->ops->enable(uvc->ext_resp_in_ep, &hs_config_desc.bulk_resp_in_ep_desc);
        if (ret < 0) {
            LOGE(__func__, "Enable ext_cmd_out_ep error!");
            goto cleanup;
        }
    }


    return 0;

cleanup:
    uvc_camera_disable_eps(gadget, uvc);
    return -DWC_E_INVALID;
}

int uvc_camera_disable_eps(struct usb_gadget *gadget, f_uvc_camera_t *uvc)
{
    (void)(gadget);

    if (uvc->video_in_ep != NULL) {
        uvc->video_in_ep->ops->disable(uvc->video_in_ep);
        uvc->video_in_ep = NULL;
    }

    if (uvc->ext_cmd_out_ep != NULL) {
        uvc->ext_cmd_out_ep->ops->disable(uvc->ext_cmd_out_ep);
        uvc->ext_cmd_out_ep = NULL;
    }

    if (uvc->ext_resp_in_ep != NULL) {
        uvc->ext_resp_in_ep->ops->disable(uvc->ext_resp_in_ep);
        uvc->ext_resp_in_ep = NULL;
    }

    return 0;
}

int uvc_bulk_get_vs_params(struct usb_gadget *gadget, int fmt_idx, int frm_idx, uvc_fmt_t *fmt_ptr, int *pw, int *ph)
{
    uvc_fmt_t fmt;
    int w, h;
    int ep0_desc_sel = 0;

    configASSERT((fmt_idx >= 1) && (fmt_idx <= VS_NUM_FORMATS));
    
    if (gadget->priv != NULL) {
        ep0_desc_sel = *(int *)gadget->priv;
        LOGD(__func__, "gadget->priv is 0x%x, ep0_desc_sel is %d", gadget->priv, ep0_desc_sel);
    }

    if (ep0_desc_sel == 1) {
        // Only one format: MJPEG
        fmt = UVC_MJPEG;
        configASSERT((frm_idx >= 1) && (frm_idx <= VS_NUM_MJPEG_FMT1_FRMS));
        w = UGETW(hs_mjpeg_config_desc.vs_frames_formats_desc.vs_mjpeg_frame_desc_1[frm_idx-1].wWidth);
        h = UGETW(hs_mjpeg_config_desc.vs_frames_formats_desc.vs_mjpeg_frame_desc_1[frm_idx-1].wHeight);
    }
    else if (ep0_desc_sel == 2) {
        // Only one format: MJPEG
        fmt = UVC_YUYV;
        configASSERT((frm_idx >= 1) && (frm_idx <= VS_NUM_YUYV_FMT1_FRMS));
        w = UGETW(hs_yuyv_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_1[frm_idx-1].wWidth);
        h = UGETW(hs_yuyv_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_1[frm_idx-1].wHeight);
    }
    else {
        switch (fmt_idx) {
            case 1:
                fmt = UVC_GRAY8;
                break;
            case 2:
                fmt = UVC_YUYV;
                break;
            case 3:
                fmt = UVC_MJPEG;
                break;
            default:
                fmt = UVC_MJPEG;
                LOGE(__func__, "Unknown format index %d", fmt_idx);
                break;
        }
        if (fmt == UVC_GRAY8) {
            configASSERT((frm_idx >= 1) && (frm_idx <= VS_NUM_FMT1_FRMS));
            w = UGETW(hs_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_1[frm_idx-1].wWidth);
            h = UGETW(hs_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_1[frm_idx-1].wHeight);
        }
        else if (fmt == UVC_YUYV) {
            configASSERT((frm_idx >= 1) && (frm_idx <= VS_NUM_FMT2_FRMS));
            w = UGETW(hs_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_2[frm_idx-1].wWidth);
            h = UGETW(hs_config_desc.vs_frames_formats_desc.vs_uncomp_frames_desc_2[frm_idx-1].wHeight);
        }
        else {
            configASSERT((frm_idx >= 1) && (frm_idx <= VS_NUM_FMT3_FRMS));
            w = UGETW(hs_config_desc.vs_frames_formats_desc.vs_mjpeg_frame_desc_3[frm_idx-1].wWidth);
            h = UGETW(hs_config_desc.vs_frames_formats_desc.vs_mjpeg_frame_desc_3[frm_idx-1].wHeight);
        }
    }

    LOGI(__func__, "fmt %d, w %d, %d", fmt, w, h);
    *fmt_ptr = fmt;
    *pw = w;
    *ph = h;

    return 0;
}
