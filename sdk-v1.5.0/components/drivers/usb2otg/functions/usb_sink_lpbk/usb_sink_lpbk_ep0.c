#include "usb_sink_lpbk.h"
#include "usb_sink_lpbk_ep0.h"

#include "syslog.h"

/* --------------------------------------------------------------------------*/
/* Macros */
/* --------------------------------------------------------------------------*/
#define USB_VENDOR_ID       0x8088
#define USB_PRODUCT_ID      0x0001

/* --------------------------------------------------------------------------*/
/* Descriptors */
/* --------------------------------------------------------------------------*/
typedef struct hs_config_desc_st {
	usb_config_descriptor_t			config_desc;
	usb_interface_descriptor_t		intf_desc;
	usb_endpoint_descriptor_t		bulk_in_ep_desc;
	usb_endpoint_descriptor_t		bulk_out_ep_desc;
} UPACKED hs_config_desc_t;

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
#define USB_STRING_MANUFACTURER    1

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
#define USB_STRING_PRODUCT    2

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
#define USB_STRING_SERIAL    3

static usb_device_descriptor_t device_desc = {
    USB_DEVICE_DESCRIPTOR_SIZE,    /* bLength */
    UDESC_DEVICE,                  /* bDescriptorType */

    UCONSTW(0),                    /* bcdUSB (filled in later) */

#ifdef USB_MASS_STORAGE_GADGET
    UDCLASS_IN_INTERFACE,          /* bDeviceClass */
#else
    UDCLASS_VENDOR,                /* bDeviceClass */
#endif
    0,                             /* bDeviceSubClass */
    0,                             /* bDeviceProtocol */
    0,                             /* bMaxPacketSize */

    UCONSTW(USB_VENDOR_ID),        /* idVendor */
    UCONSTW(USB_PRODUCT_ID),       /* idProduct */
    UCONSTW(0xffff),               /* bcdDevice */

    USB_STRING_MANUFACTURER,       /* iManufacturer */
    USB_STRING_PRODUCT,            /* iProduct */
    USB_STRING_SERIAL,             /* iSerialNumber */

    1,                             /* bNumConfigurations */
};

static usb_device_qualifier_t dev_qualifier = {
    USB_DEVICE_QUALIFIER_SIZE,     /* bLength */
    UDESC_DEVICE_QUALIFIER,        /* bDescriptorType */

    UCONSTW(0),                    /* bcdUSB (filled in later) */

#ifdef USB_MASS_STORAGE_GADGET
    UDCLASS_IN_INTERFACE,          /* bDeviceClass */
#else
    UDCLASS_VENDOR,                /* bDeviceClass */
#endif
    0,                             /* bDeviceSubClass */
    0,                             /* bDeviceProtocol */
    9,                             /* bMaxPacketSize0 */
    1,                             /* bNumConfigurations */
    0,                             /* bReserved */
};

static hs_config_desc_t hs_config_desc = {
    /* config descriptor */
    {
        USB_CONFIG_DESCRIPTOR_SIZE,    /* bLength */
        UDESC_CONFIG,                  /* bDescriptorType */
        UCONSTW(0),                    /* wTotalLength (filled in later) */
        1,                             /* bNumInterface */
        1,                             /* bConfigurationValue */
        0,                             /* iConfiguration */
        UC_ATT_ONE | UC_ATT_SELFPOWER, /* bmAttributes */
        100 / UC_POWER_FACTOR,         /* bMaxPower (100 ma) */
    },
    /* interface descriptor */
    {
        USB_INTERFACE_DESCRIPTOR_SIZE, /* bLength */
        UDESC_INTERFACE,               /* bDescriptorType */
        0,                             /* bInterfaceNumber */
        0,                             /* bAlternateSetting */
        2,                             /* bNumEndpoints */

#ifdef USB_MASS_STORAGE_GADGET
        UICLASS_MASS,                  /* bInterfaceClass */
        UISUBCLASS_SCSI,               /* bInterfaceSubClass */
        UIPROTO_MASS_BBB,              /* bInterfaceProtocol */
#else
        UICLASS_VENDOR,                /* bInterfaceClass */
        0,                             /* bInterfaceSubClass */
        0,                             /* bInterfaceProtocol */
#endif
        0,                             /* iInterface */
    },
    /* bulk IN endpoint descriptor */
    {
        USB_ENDPOINT_DESCRIPTOR_SIZE,  /* bLength */
        UDESC_ENDPOINT,                /* bDescriptorType */
        1 | UE_DIR_IN,                 /* bEndpointAddress */
#ifdef TEST_ISOC
        UE_ISOCHRONOUS | UE_ISO_ASYNC, /* bmAttributes */
#else
        UE_BULK,                       /* bmAttributes */
#endif
        UCONSTW(512),                  /* wMaxPacketSize */
        1,                             /* bInterval */
    },
    /* bulk OUT endpoint descriptor */
    {
        USB_ENDPOINT_DESCRIPTOR_SIZE,  /* bLength */
        UDESC_ENDPOINT,                /* bDescriptorType */
        2 | UE_DIR_OUT,                /* bEndpointAddress */
#ifdef TEST_ISOC
        UE_ISOCHRONOUS | UE_ISO_ASYNC, /* bmAttributes */
#else
        UE_BULK,                       /* bmAttributes */
#endif
        UCONSTW(512),                  /* wMaxPacketSize */
        1,                             /* bInterval (every uframe) */
    },
};

static void ep0_complete(usb_ep_t *ep, usb_request_t *req)
{
    // Void
    /*LOGI(__func__, "in...");*/
    if (req->status || req->actual != req->length) {
        LOGW(__func__, "setup complete --> %d, %d/%d", req->status, req->actual, req->length);
    }
}

/* --------------------------------------------------------------------------*/
/* Functions */
/* --------------------------------------------------------------------------*/
int usb_sink_lpbk_get_desc(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    int ret;
    u8 desc_type = UGETW(ctrl->wValue) >> 8;
    u8 desc_idx = UGETW(ctrl->wValue);
    u16 max = UGETW(ctrl->wLength);
    u16 len;
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    u8 *buf = req->buf;
    configASSERT(buf != NULL);
    struct usb_ep *ep0 = gadget->ep0;

    LOGD(__func__, "GET_DESCRIPTOR %02x.%02x v%04x i%04x l%04x\n",
           ctrl->bmRequestType, ctrl->bRequest, UGETW(ctrl->wValue),
           UGETW(ctrl->wIndex), UGETW(ctrl->wLength));

    req->zero = 0;
    req->length = 0;
    req->complete = ep0_complete;

    switch (desc_type) {
        case UDESC_DEVICE:
            if (desc_idx) {
                return -ENOTSUPP;
            }
            // set bMaxPacketSize
            USETW(device_desc.bcdUSB, 0x200);
            device_desc.bMaxPacketSize = 64;
            // copy device descriptor
            memcpy(buf, &device_desc, sizeof(device_desc));
            len = sizeof(device_desc);
            break;

        case UDESC_CONFIG:
            if (desc_idx) {
                return -ENOTSUPP;
            }
            USETW(hs_config_desc.config_desc.wTotalLength,
                  sizeof(hs_config_desc));
            // copy config descriptor
            memcpy(buf, &hs_config_desc, sizeof(hs_config_desc));
            len = sizeof(hs_config_desc);
            break;

        case UDESC_STRING:
            switch (desc_idx) {
                case 0:
                    memcpy(buf, &language_string, sizeof(language_string));
                    len = sizeof(language_string);
                    break;

                case USB_STRING_MANUFACTURER:
                    memcpy(buf, &manuf_string, sizeof(manuf_string));
                    len = sizeof(manuf_string);
                    break;

                case USB_STRING_PRODUCT:
                    memcpy(buf, &product_string, sizeof(product_string));
                    len = sizeof(product_string);
                    break;

                case USB_STRING_SERIAL:
                    memcpy(buf, &serial_string, sizeof(serial_string));
                    len = sizeof(serial_string);
                    break;

                default:
                    return -ENOTSUPP;
            }
            break;

        case UDESC_DEVICE_QUALIFIER:
            USETW(dev_qualifier.bcdUSB, 0x200);
            /*dev_qualifier.bMaxPacketSize0 = 9;*/
            memcpy(buf, &dev_qualifier, sizeof(dev_qualifier));
            len = sizeof(dev_qualifier);
            break;

        case UDESC_OTHER_SPEED_CONFIGURATION:
            hs_config_desc.config_desc.bDescriptorType =
                    UDESC_OTHER_SPEED_CONFIGURATION;
            USETW(hs_config_desc.config_desc.wTotalLength,
                  sizeof(hs_config_desc));
            memcpy(buf, &hs_config_desc, sizeof(hs_config_desc));
            hs_config_desc.config_desc.bDescriptorType = UDESC_CONFIG;
            len = sizeof(hs_config_desc);
            break;

        default:
            configASSERT(!"unsupport desc_type");
            return -ENOTSUPP;
    }

    configASSERT((uint32_t)len <= USB_EP0_BUF_SIZE);
    /*dcache_flush(buf, len);*/

    req->length = len < max ? len : max;

    LOGD(__func__, "req->length is %d", req->length);

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

struct usb_ep *usb_sink_lpbk_get_bulk_in_ep(struct usb_gadget *gadget)
{
    return gadget_find_eq_by_name(gadget, "ep1in");
}

struct usb_ep *usb_sink_lpbk_get_bulk_out_ep(struct usb_gadget *gadget)
{
    return gadget_find_eq_by_name(gadget, "ep2out");
}

int usb_sink_lpbk_enable_eps(struct usb_gadget *gadget, f_loopback_t *lpbk)
{
    int ret;

    lpbk->ep_in  = usb_sink_lpbk_get_bulk_in_ep(gadget);
    configASSERT(lpbk->ep_in != NULL);
    lpbk->ep_out = usb_sink_lpbk_get_bulk_out_ep(gadget);
    configASSERT(lpbk->ep_out != NULL);

    ret = lpbk->ep_in->ops->enable(lpbk->ep_in, &hs_config_desc.bulk_in_ep_desc);
    if (ret < 0) {
        LOGE (__func__, "Enable lpbk->ep_in error!");
        usb_sink_lpbk_disable_eps(gadget, lpbk);
        return ret;
    }

    ret = lpbk->ep_out->ops->enable(lpbk->ep_out, &hs_config_desc.bulk_out_ep_desc);
    if (ret < 0) {
        LOGE (__func__, "Enable lpbk->ep_out error!");
        usb_sink_lpbk_disable_eps(gadget, lpbk);
        return ret;
    }

    return 0;
}

int usb_sink_lpbk_disable_eps(struct usb_gadget *gadget, f_loopback_t *lpbk)
{
    if (lpbk->ep_in != NULL) {
        lpbk->ep_in->ops->disable(lpbk->ep_in);
        lpbk->ep_in = NULL;
    }

    if (lpbk->ep_out != NULL) {
        lpbk->ep_out->ops->disable(lpbk->ep_out);
        lpbk->ep_out = NULL;
    }

    return 0;
}
