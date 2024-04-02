#include "uvc_camera.h"
#include "uvc_camera_ep0.h"
#include "syslog.h"
#include "udevice.h"

#include "system_AI3100.h"
#include "mem_AI3100.h"
#include "xvic.h"
#include "aiva_utils.h"
#include "mjpeg.h"
#include "rsz.h"
#include "uvc_camera_ep0.h"
#include "usb_debug.h"
#include "aiva_sleep.h"
#include "aiva_malloc.h"
#include "xlist.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif // USE_RTOS

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#if 0
#define UVC_DEBUG(TAG, ...)   LOGD(TAG, ##__VA_ARGS__)
#else
#define UVC_DEBUG(TAG, ...)
#endif

/*#define SINGLE_BUFFER*/

#define DWC_REQ_SIZE                (1024)
#define USB_ACM_MAX_BUF_CNT         (512)
/*#define USB_ACM_MAX_BUF_CNT         (3)*/

/*#define VIDEO_REQ_SIZE              (512 * 1024 - 32)*/
/*#define VIDEO_REQ_SIZE              (16 * 1024)*/
static uint32_t VIDEO_REQ_SIZE = 16 * 1024;
static int EP0_DESC_SEL   = 0;
#define VIDEO_MAX_DATA_SIZE         (VIDEO_REQ_SIZE - UVC_MAX_HEADER)


#define SWAP_FRAME_BUFS(a, b) do {                                        \
    configASSERT(((frame_buf_t *)(a))->magic == FRAME_BUF_MAGIC);         \
    configASSERT(((frame_buf_t *)(b))->magic == FRAME_BUF_MAGIC);         \
    configASSERT(((frame_buf_t *)(a))->fmt == ((frame_buf_t *)(b))->fmt); \
    void *tmp = (void *)(a);                                              \
    (a) = (b);                                                            \
    (b) = tmp;                                                            \
} while (0)

typedef enum _usb_stream_state_t {
    STREAM_STATE_UNINITED   = 0,
    STREAM_STATE_INITED     = 1,
    STREAM_STATE_IDLE       = 2,
    STREAM_STATE_BUSY       = 3,
    STREAM_STATE_LAST_DATA  = 4,
} usb_stream_state_t;

static volatile usb_stream_state_t m_uvc_stream_state = STREAM_STATE_UNINITED;
static volatile usb_stream_state_t m_acm_send_state   = STREAM_STATE_UNINITED;
static volatile usb_stream_state_t m_acm_recv_state   = STREAM_STATE_UNINITED;

static uvc_user_start_func_t _user_start_stream_cb = NULL;
static uvc_user_stop_func_t  _user_stop_stream_cb  = NULL;


static volatile int  m_remain_size    = 0;
static volatile bool m_dwc_usb3_acm_inited = false;

/** @} */

typedef struct _usb_func_acm_buf_t {
    uint8_t *buffer;    // pointer to data buffer
    uint16_t offset;    // offset bytes of data buffer
    uint16_t count;     // byte count of valid data in buffer
    uint16_t size;      // actual size of the buffer in bytes
    struct list_head list;
} __attribute__((aligned(4))) usb_func_acm_buf_t;


/* CDC ACM send list */
static struct list_head m_acm_send_list;
/* CDC ACM recv list */
static struct list_head m_acm_recv_list;
/* CDC ACM free list */
static struct list_head m_acm_send_free_list;
static struct list_head m_acm_recv_free_list;

static usb_func_acm_buf_t m_acm_send_bufs[USB_ACM_MAX_BUF_CNT];
static usb_func_acm_buf_t m_acm_recv_bufs[USB_ACM_MAX_BUF_CNT];

static usb_func_acm_buf_t *usb_acm_get_buffer(struct list_head *list_hdr);
static int usb_acm_add_buffer_tail(struct list_head *list_hdr, usb_func_acm_buf_t *buf);
static int usb_acm_add_buffer_head(struct list_head *list_hdr, usb_func_acm_buf_t *buf);

static frame_buf_t *m_ready_frame = NULL;
static frame_buf_t *m_cur_frame   = NULL;
static volatile bool m_uvc_strm_started = false;
static volatile bool m_ready_frame_updated = false;
static volatile bool m_usb_function_binded = false;

static __ATTR_ALIGN__(32) uint8_t m_acm_send_mem_buf[USB_ACM_MAX_BUF_CNT * DWC_REQ_SIZE];
static __ATTR_ALIGN__(32) uint8_t m_acm_recv_mem_buf[USB_ACM_MAX_BUF_CNT * DWC_REQ_SIZE];

/* UVC Probe Control Setting for a USB 2.0 connection. */
static uvc_streaming_control_t g_probe_ctl20 = {
    0x00, 0x00,                 /* bmHint : no hit */
    0x01,                       /* Use 1st Video format index */
    0x01,                       /* Use 1st Video frame index */
    0x15, 0x16, 0x05, 0x00,     /* Desired frame interval in the unit of 100ns: 30 fps */
    0x00, 0x00,                 /* Key frame rate in key frame/video frame units: only applicable
                                   to video streaming with adjustable compression parameters */
    0x00, 0x00,                 /* PFrame rate in PFrame / key frame units: only applicable to
                                   video streaming with adjustable compression parameters */
    0x00, 0x00,                 /* Compression quality control: only applicable to video streaming
                                   with adjustable compression parameters */
    0x00, 0x00,                 /* Window size for average bit rate: only applicable to video
                                   streaming with adjustable compression parameters */
    0x00, 0x00,                 /* Internal video streaming i/f latency in ms */
    0x00, 0x00, 0x80, 0x00,     /* Max video frame size in bytes */
    /*B4VAL(VIDEO_REQ_SIZE),      [> No. of bytes device can rx in single payload <]*/
    B4VAL(0),                   /* TODO: fill later. No. of bytes device can rx in single payload */
};

/* UVC Header to be prefixed at the top of each 16 KB video data buffer. */
static uint8_t g_uvc_header[UVC_MAX_HEADER] __ATTR_ALIGN__(32)=
{
    0x0C,                               /* Header Length */
    0x8C,                               /* Bit field header field */
    0x00, 0x00, 0x00, 0x00,             /* Presentation time stamp field */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* Source clock reference field */
};

/* Video Probe Commit Control. This array is filled out when the host sends down the SET_CUR request. */
static uvc_streaming_control_t g_commit_ctl;
static int g_set_cur_updated = 0;

/* Scratch buffer used for handling UVC class requests with a data phase. */
static uint8_t g_ep0_buf[32] __ATTR_ALIGN__(32);

#ifdef USE_RTOS
static pthread_mutex_t m_usb_func_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/** Data packet buffer used to return data for GET_STATUS and
 *  GET_DESCRIPTOR requests, up to 512 bytes in length
 */
static __DMA_ALIGNED__(32) uint8_t g_ep0_status_buf[USB_EP0_BUF_SIZE];

static int  bind(struct usb_gadget *gadget, struct usb_gadget_driver *driver);
static void unbind(struct usb_gadget *gadget);
static int  setup(struct usb_gadget *gadget, const usb_device_request_t *ctrl);
static void disconnect(struct usb_gadget *gadget);
static void suspend(struct usb_gadget *gadget);
static void resume(struct usb_gadget *gadget);
static void reset(struct usb_gadget *gadget);
static void complete(usb_ep_t *ep, usb_request_t *req);

static int  uvc_bulk_init(uvc_param_t *param);
static int  usb_bulk_commit_frame(frame_buf_t *cur);
static void uvc_bulk_register_user_start_cb(uvc_user_start_func_t func);
static void uvc_bulk_register_user_stop_cb(uvc_user_stop_func_t func);
static bool usb_bulk_acm_inited(void);
static int  usb_bulk_acm_send_buffer(const void *buffer, int bytes, int wait_option);
static int  usb_bulk_acm_recv_buffer(void *buffer, int *bytes, int wait_option);

static uvc_func_op_t func_op = {
    .uvc_init              = uvc_bulk_init,
    .uvc_commit_frame      = usb_bulk_commit_frame,
    .uvc_register_start_cb = uvc_bulk_register_user_start_cb,
    .uvc_register_stop_cb  = uvc_bulk_register_user_stop_cb,
    .acm_inited            = usb_bulk_acm_inited,
    .acm_send_buf          = usb_bulk_acm_send_buffer,
    .acm_recv_buf          = usb_bulk_acm_recv_buffer,
};

static struct usb_gadget_driver m_uvc_bulk_gadget = {
    .function  = "uvc_camera",
    .max_speed = USB_SPEED_HIGH,
    .bind      = bind,
    .unbind    = unbind,
    .setup     = setup,
    .connect   = NULL,
    .disconnect= disconnect,
    .suspend   = suspend,
    .resume    = resume,
    .reset     = reset,
    .dev_ops   = &func_op,
};

static f_uvc_camera_t m_uvc_camera = {
    .driver = &m_uvc_bulk_gadget,
    .ifc = 0,
    .cfg = 0,
    .ctrl_stat_in_ep  = NULL,
    .ctrl_stat_in_buf = NULL,
    .ctrl_stat_in_req = NULL,
    .video_in_ep      = NULL,
    .video_in_req     = NULL,
    .ext_cmd_out_ep   = NULL,
    .ext_cmd_out_buf  = NULL,
    .ext_cmd_out_req  = NULL,
    .ext_resp_in_ep   = NULL,
    .ext_resp_in_buf  = NULL,
    .ext_resp_in_req  = NULL,
};

static int usb_function_get_lock(void)
{
    int ret = 0;
#ifdef USE_RTOS
    if ((!xPortIsInISR()) && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) { 
        ret = pthread_mutex_lock(&m_usb_func_lock);
    }
#endif
    return ret;
}

static int usb_function_release_lock(void)
{
    int ret = 0;
#ifdef USE_RTOS
    if ((!xPortIsInISR()) && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) { 
        ret = pthread_mutex_unlock(&m_usb_func_lock);
    }
#endif
    return ret;
}

static int usb_func_set_stream_state(usb_request_t *req, usb_stream_state_t state)
{
    configASSERT(req != NULL);
    configASSERT(req->context != NULL);
    volatile usb_stream_state_t *s = req->context;
    *s = state;
    /*UVC_DEBUG(__func__, "req 0x%x, state %d", req, state);*/
    return 0;
}

static usb_stream_state_t usb_func_get_stream_state(usb_request_t *req)
{
    configASSERT(req != NULL);
    configASSERT(req->context != NULL);
    return *(usb_stream_state_t *)req->context;
}

static void uvc_stop_stream(struct usb_gadget *gadget)
{
    (void)(gadget);

    // TODO
    LOGI(__func__, "in...");
    if (m_uvc_strm_started) {
        m_uvc_strm_started = false;
        m_uvc_stream_state = STREAM_STATE_UNINITED;
        if (_user_stop_stream_cb != NULL) {
            _user_stop_stream_cb();
        }
    }
    LOGI(__func__, "out...");
}

static void uvc_start_stream(struct usb_gadget *gadget)
{
    // TODO
    LOGI(__func__, "in...");
    if ((m_uvc_stream_state != STREAM_STATE_UNINITED) && (m_uvc_stream_state != STREAM_STATE_IDLE)) {
        LOGW(__func__, "m_uvc_stream_state is %d now! Force it to be STREAM_STATE_UNINITED!", m_uvc_stream_state);
        m_uvc_stream_state = STREAM_STATE_UNINITED;
    }
#if 0
    if (!m_uvc_strm_started) {
        if (_user_start_stream_cb != NULL) {
            uvc_fmt_t fmt;
            int w, h;
            int fps = 10000000 / (g_commit_ctl.dwFrameInterval[0] | g_commit_ctl.dwFrameInterval[1] << 8 | g_commit_ctl.dwFrameInterval[2] << 16 | g_commit_ctl.dwFrameInterval[3] << 24);
            uvc_bulk_get_vs_params(g_commit_ctl.bFormatIndex, g_commit_ctl.bFrameIndex, &fmt, &w, &h);
            _user_start_stream_cb(fmt, w, h, fps);
        }
        m_uvc_strm_started    = true;
    }
#else
    if (m_uvc_strm_started) {
        uvc_stop_stream(gadget);
    }
    if (_user_start_stream_cb != NULL) {
        uvc_fmt_t fmt;
        int w, h;
        int fps = 10000000 / (g_commit_ctl.dwFrameInterval[0] | g_commit_ctl.dwFrameInterval[1] << 8 | g_commit_ctl.dwFrameInterval[2] << 16 | g_commit_ctl.dwFrameInterval[3] << 24);
        uvc_bulk_get_vs_params(gadget, g_commit_ctl.bFormatIndex, g_commit_ctl.bFrameIndex, &fmt, &w, &h);
        _user_start_stream_cb(fmt, w, h, fps);
    }
    m_uvc_strm_started    = true;
#endif
    m_ready_frame_updated = false;
    LOGI(__func__, "out...");
}


static int dwc_usb_ep0_wait_nrdy(struct usb_gadget *gadget)
{
    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
   
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    
    req->length = 0;
    req->zero   = 0;
    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);
    if (ret < 0) {
        LOGE(__func__, "error, ep0->ops->queue error! ep0 0x%x, req 0x%x",
                ep0, req);
    }

    return ret;
}


static int dwc_usb_ep0_send_data(struct usb_gadget *gadget, void *data, int bytes)
{
    int ret = 0;
   
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
   
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    
    u8 *buf = req->buf;
    configASSERT(buf != NULL);

    aiva_memcpy(buf, data, bytes);
    req->length = bytes;
    req->zero   = 0;

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

static int dwc_usb_ep0_recv_data(struct usb_gadget *gadget, void *buf, int bytes)
{
    (void)(buf);

    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
   
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    
    req->length = bytes;
    req->zero   = 0;
    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

/*
 * Handler for control requests addressed to the Processing Unit.
 */
static int uvc_handle_proc_unit_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    int ret;
    //uint16_t readCount;

    uint8_t  bType;
    uint8_t  bTarget;

    uint8_t bRequest = ctrl->bRequest;
    uint8_t bReqType = ctrl->bmRequestType;
    
    //uint16_t windex  = UGETW(ctrl->wIndex);
    uint16_t wValue  = UGETW(ctrl->wValue);
    //uint16_t wlength = UGETW(ctrl->wLength);

    /* Obtain Request Type and Request */
    bType    = (bReqType & USB_TYPE_MASK);
    bTarget  = (bReqType & USB_TARGET_MASK);

    ret = 0;

    switch (wValue)
    {
        case UVC_PU_BRIGHTNESS_CONTROL:
            switch (bRequest)
            {
                case USB_UVC_GET_LEN_REQ: /* Length of brightness data = 1 byte. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_LEN_REQ\n");
                    g_ep0_buf[0] = 1;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_CUR_REQ: /* Current brightness value. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_CUR_REQ\n");
                    g_ep0_buf[0] = 64;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_MIN_REQ: /* Minimum brightness = 0. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_MIN_REQ\n");
                    g_ep0_buf[0] = 0;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_MAX_REQ: /* Maximum brightness = 255. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_MAX_REQ\n");
                    g_ep0_buf[0] = 255;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_RES_REQ: /* Resolution = 1. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_RES_REQ\n");
                    g_ep0_buf[0] = 1;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_INFO_REQ: /* Both GET and SET requests are supported, auto modes not supported */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_INFO_REQ\n");
                    g_ep0_buf[0] = 3;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_DEF_REQ: /* Default brightness value = 55. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: GET_DEF_REQ\n");
                    g_ep0_buf[0] = 55;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_SET_CUR_REQ: /* Update brightness value. */
                    UVC_DEBUG(__func__, "BIRGHTNESS: SET_CUR_REQ\n");
                    dwc_usb_ep0_recv_data(gadget, g_ep0_buf, UVC_MAX_PROBE_SETTING_ALIGNED);
                    break;
                default:
                    UVC_DEBUG(__func__, "Unsupported PU Brightness Control Request:0x%x, Stall USB ...\n", bRequest);
                    ret = -ENOTSUPP;
                    break;
            }
            break;

        default:
            /*
             * Only the brightness control is supported as of now. Add additional code here to support
             * other controls.
             */
            UVC_DEBUG(__func__, "Unsupported Processing Unit Request:0x%x, Stall USB ...\n", wValue);
            ret = -ENOTSUPP;
            break;
    }
    return ret;
}

/*
 * Handler for control requests addressed to the UVC Camera Terminal unit.
 */
static int uvc_handle_cam_term_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    (void)(gadget);
    (void)(ctrl);

    LOGW(__func__, "Unsupported Interface Ctrl Request, Stall USB ...");
    return -ENOTSUPP;
}

/*
 * Handler for UVC Interface control requests.
 */
static int uvc_handle_intf_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    (void)(gadget);
    (void)(ctrl);

    /* No requests supported as of now. Just stall EP0 to fail the request. */
    LOGW(__func__, "Unsupported Interface Ctrl Request, Stall USB ...");
    return -ENOTSUPP;
}

/*
 * Handler for control requests addressed to the Extension Unit.
 */
static int uvc_handle_ext_unit_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    (void)(gadget);

    /* No requests supported as of now. Just stall EP0 to fail the request. */
    //uint8_t cmd = UGETW(ctrl->wValue) >> 8;
    //int  length = UGETW(ctrl->wLength);
    LOGW(__func__, "Unsupported Extension Unit Request, Stall USB ...");
    {
        LOGD(__func__, "wIndex 0x%x, wValue 0x%x, wLength 0x%x", 
                UGETW(ctrl->wIndex), UGETW(ctrl->wValue), UGETW(ctrl->wLength));
    }

    /*dwc_usb_ep0_recv_data(gadget, g_ep0_buf, length);*/
    
    /*return -ENOTSUPP;*/
    return 0;
}

/*
 * Handler for the video control requests.
 */
static int uvc_handle_vc_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    uint16_t wIndex  = UGETW(ctrl->wIndex);
    int ret;

    switch ((wIndex >> 8))
    {
        case UVC_PROCESSING_UNIT_ID:
            ret = uvc_handle_proc_unit_reqs(gadget, ctrl);
            break;

        case UVC_CAMERA_TERMINAL_ID:
            ret = uvc_handle_cam_term_reqs(gadget, ctrl);
            break;

        case UVC_INTERFACE_CTRL:
            ret = uvc_handle_intf_reqs(gadget, ctrl);
            break;

        case UVC_EXTENSION_UNIT_ID:
            ret = uvc_handle_ext_unit_reqs(gadget, ctrl);
            break;

        default:
            /* Unsupported request. Fail by stalling the control endpoint. */
            LOGW(__func__, "Unsupported UVC_VIDEO_CONTROL request:0x%x, stall USB ...\n", wIndex);
            dwc_usb_ep0_wait_nrdy(gadget);
            /*ret = -ENOTSUPP;*/
            ret = 0;
            break;
    }
    return ret;
}


/*
 * Handler for the video streaming requests.
 */
static int uvc_handle_vs_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    uint8_t  bType;
    uint8_t  bTarget;

    uint8_t bRequest = ctrl->bRequest;
    uint8_t bReqType = ctrl->bmRequestType;
    
    //uint16_t windex  = UGETW(ctrl->wIndex);
    uint16_t wValue  = UGETW(ctrl->wValue);
    //uint16_t wlength = UGETW(ctrl->wLength);
    int ret;

    /* Obtain Request Type and Request */
    bType    = (bReqType & USB_TYPE_MASK);
    bTarget  = (bReqType & USB_TARGET_MASK);


    //uint16_t readCount;

    ret = 0;

    switch (wValue)
    {
        case UVC_PROBE_CTRL:
            switch (bRequest)
            {
                case USB_UVC_GET_INFO_REQ:
                    LOGD(__func__, "PROBE_CTRL: GET_INFO_REQ");
                    g_ep0_buf[0] = 3;                /* GET/SET requests are supported. */
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_LEN_REQ:
                    LOGD(__func__, "PROBE_CTRL: GET_LEN_REQ");
                    g_ep0_buf[0] = UVC_MAX_PROBE_SETTING;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_CUR_REQ:
                case USB_UVC_GET_MIN_REQ:
                case USB_UVC_GET_MAX_REQ:
                case USB_UVC_GET_DEF_REQ: /* There is only one setting per USB speed. */
                    LOGD(__func__, "PROBE_CTRL: GET_CUR/MIN/MAX/DEF_REQ");
                    if (g_set_cur_updated) {
                        /*dcache_inval(&g_commit_ctl, UVC_MAX_PROBE_SETTING);*/
                        aiva_memcpy((void *)&g_commit_ctl, g_ep0_status_buf, UVC_MAX_PROBE_SETTING);
                        g_probe_ctl20.bFormatIndex = g_commit_ctl.bFormatIndex;
                        g_probe_ctl20.bFrameIndex  = g_commit_ctl.bFrameIndex;
                        g_probe_ctl20.dwFrameInterval[0] = g_commit_ctl.dwFrameInterval[0];
                        g_probe_ctl20.dwFrameInterval[1] = g_commit_ctl.dwFrameInterval[1];
                        g_probe_ctl20.dwFrameInterval[2] = g_commit_ctl.dwFrameInterval[2];
                        g_probe_ctl20.dwFrameInterval[3] = g_commit_ctl.dwFrameInterval[3];
                        USETDW(g_probe_ctl20.dwMaxPayloadTransferSize, VIDEO_REQ_SIZE);
                        LOGD(__func__, "bFormatIndex is %d", g_commit_ctl.bFormatIndex);
                        LOGD(__func__, "bFrameIndex  is %d", g_commit_ctl.bFrameIndex);
                        LOGD(__func__, "dwFrameInterval is %d", UGETDW(g_probe_ctl20.dwFrameInterval));
                        g_set_cur_updated = 0;
                    }
                    dwc_usb_ep0_send_data(gadget, &g_probe_ctl20, UVC_MAX_PROBE_SETTING);
                    break;
                case USB_UVC_SET_CUR_REQ:
                    LOGD(__func__, "PROBE_CTRL: SET_CUR_REQ");
                    dwc_usb_ep0_recv_data(gadget, &g_commit_ctl, UVC_MAX_PROBE_SETTING);
                    g_set_cur_updated = 1;
                    break;
                default:
                    LOGW(__func__, "Unsupported UVC_PROBE_REQUEST:0x%x, Stall USB ...\n", bRequest);
                    ret = -ENOTSUPP;
                    break;
            }
            break;

        case UVC_COMMIT_CTRL:
            switch (bRequest)
            {
                case USB_UVC_GET_INFO_REQ:
                    LOGD(__func__, "COMMIT_CTRL: GET_INFO_REQ");
                    g_ep0_buf[0] = 3;                        /* GET/SET requests are supported. */
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_LEN_REQ:
                    LOGD(__func__, "COMMIT_CTRL: GET_LEN_REQ");
                    g_ep0_buf[0] = UVC_MAX_PROBE_SETTING;
                    dwc_usb_ep0_send_data(gadget, g_ep0_buf, 1);
                    break;
                case USB_UVC_GET_CUR_REQ:
                    LOGD(__func__, "COMMIT_CTRL: GET_CUR_REQ");
                    dwc_usb_ep0_send_data(gadget, &g_probe_ctl20, UVC_MAX_PROBE_SETTING);
                    break;
                case USB_UVC_SET_CUR_REQ:
                    LOGD(__func__, "COMMIT_CTRL: SET_CUR_REQ");
                    /* The host has selected the parameters for the video stream. Check the desired
                       resolution settings, configure the sensor and start the video stream.
                       */
                    LOGD(__func__, "bFormatIndex is %d", g_commit_ctl.bFormatIndex);
                    LOGD(__func__, "bFrameIndex  is %d", g_commit_ctl.bFrameIndex);
                    LOGD(__func__, "dwMaxVideoFrameSize(commit): %d", UGETDW(g_commit_ctl.dwMaxVideoFrameSize));
                    LOGD(__func__, "dwMaxVideoFrameSize(probe):  %d", UGETDW(g_probe_ctl20.dwMaxVideoFrameSize));
                    LOGD(__func__, "dwMaxPayloadTransferSize(commit): %d", UGETDW(g_commit_ctl.dwMaxPayloadTransferSize));
                    LOGD(__func__, "dwMaxPayloadTransferSize(probe):  %d", UGETDW(g_probe_ctl20.dwMaxPayloadTransferSize));
                    dwc_usb_ep0_recv_data(gadget, &g_commit_ctl, UVC_MAX_PROBE_SETTING);
                    uvc_start_stream(gadget);
                    break;

                default:
                    LOGW(__func__, "Unsupported UVC_COMMIT_REQUEST:0x%x, Stall USB ...\n", bRequest);
                    ret = -ENOTSUPP;
                    break;
            }
            break;

        default:
            LOGW(__func__, "Unsupported VIDEO STREAM REQUEST:0x%x, Stall USB ...\n", wValue);
            ret = -ENOTSUPP;
            break;
    }
    return ret;
}

/*
 * Handler for CDC ACM requests
 */
static int cdc_acm_handle_reqs(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    uint8_t  bType;
    uint8_t  bTarget;

    uint8_t bRequest = ctrl->bRequest;
    uint8_t bReqType = ctrl->bmRequestType;
    
    //uint16_t windex  = UGETW(ctrl->wIndex);
    //uint16_t wValue  = UGETW(ctrl->wValue);
    uint16_t wlength = UGETW(ctrl->wLength);

    /* Obtain Request Type and Request */
    bType    = (bReqType & USB_TYPE_MASK);
    bTarget  = (bReqType & USB_TARGET_MASK);

    switch (bRequest) {
        case USB_CDC_REQ_SET_LINE_CODING:
            UVC_DEBUG(__func__, "USB_CDC_REQ_SET_LINE_CODING");
            dwc_usb_ep0_recv_data(gadget, g_ep0_buf, wlength);
            break;
        case USB_CDC_REQ_GET_LINE_CODING:
            UVC_DEBUG(__func__, "USB_CDC_REQ_GET_LINE_CODING");
            dwc_usb_ep0_send_data(gadget, g_ep0_buf, wlength);
            break;
        case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
            UVC_DEBUG(__func__, "USB_CDC_REQ_SET_CONTROL_LINE_STATE");
            dwc_usb_ep0_wait_nrdy(gadget);
            break;
        case USB_CDC_REQ_SEND_BREAK:
            UVC_DEBUG(__func__, "USB_CDC_REQ_SEND_BREAK");
            dwc_usb_ep0_wait_nrdy(gadget);
            break;
        default:
            LOGE(__func__, "Unknown request 0x%02x", bRequest);
            dwc_usb_ep0_wait_nrdy(gadget);
    }

    return 0;
}

// -----------------------------------------------------------------------------

static void usb_acm_buffer_list_release(void)
{
    usb_func_acm_buf_t *cur, *next;
    
    m_dwc_usb3_acm_inited = false;

    list_for_each_entry_safe(cur, next, &m_acm_recv_list, list, usb_func_acm_buf_t) {
        list_del(&cur->list);
    }
    
    list_for_each_entry_safe(cur, next, &m_acm_recv_free_list, list, usb_func_acm_buf_t) {
        list_del(&cur->list);
    }
    
    list_for_each_entry_safe(cur, next, &m_acm_send_list, list, usb_func_acm_buf_t) {
        list_del(&cur->list);
    }
    
    list_for_each_entry_safe(cur, next, &m_acm_send_free_list, list, usb_func_acm_buf_t) {
        list_del(&cur->list);
    }
}

static int usb_acm_buffer_lists_init(void)
{
    int i;
    uint32_t send_off, recv_off;

    if (m_dwc_usb3_acm_inited) {
        usb_acm_buffer_list_release();
    }

    list_init(&m_acm_send_free_list);
    list_init(&m_acm_recv_free_list);
    list_init(&m_acm_send_list);
    list_init(&m_acm_recv_list);

    memset(m_acm_send_mem_buf, 0xff, sizeof(m_acm_send_mem_buf));
    dcache_flush(m_acm_send_mem_buf, sizeof(m_acm_send_mem_buf));
    memset(m_acm_recv_mem_buf, 0xff, sizeof(m_acm_recv_mem_buf));
    dcache_flush(m_acm_recv_mem_buf, sizeof(m_acm_recv_mem_buf));

    send_off = 0;
    recv_off = 0;
    for (i = 0; i < USB_ACM_MAX_BUF_CNT; i++) {
        m_acm_send_bufs[i].buffer = &m_acm_send_mem_buf[send_off];
        m_acm_send_bufs[i].offset = 0;
        m_acm_send_bufs[i].count  = 0;
        m_acm_send_bufs[i].size   = DWC_REQ_SIZE;
        send_off += DWC_REQ_SIZE;
        if (m_acm_send_bufs[i].buffer == NULL) {
            LOGE(__func__, "alloc m_acm_send_bufs[%d] error!", i);
            goto error_out;
        }
        list_add_tail(&m_acm_send_bufs[i].list, &m_acm_send_free_list);

        m_acm_recv_bufs[i].buffer = &m_acm_recv_mem_buf[recv_off];
        m_acm_recv_bufs[i].offset = 0;
        m_acm_recv_bufs[i].count  = 0;
        m_acm_recv_bufs[i].size   = DWC_REQ_SIZE;
        recv_off += DWC_REQ_SIZE;
        if (m_acm_recv_bufs[i].buffer == NULL) {
            LOGE(__func__, "alloc m_acm_recv_bufs[%d] error!", i);
            goto error_out;
        }
        list_add_tail(&m_acm_recv_bufs[i].list, &m_acm_recv_free_list);
    }

    m_dwc_usb3_acm_inited = true;

    return 0;

error_out:
    usb_acm_buffer_list_release();
    return -1;
}


/* Add the UVC packet header to the top of the specified DMA buffer. */
static void uvc_add_header(uint8_t *buf, int is_frame_end, int is_bad_frame)
{
    /*LOGI(__func__, "is_frame_end %d", is_frame_end);*/
    aiva_memcpy(buf, g_uvc_header, UVC_MAX_HEADER);
    if (is_bad_frame) {
        buf[1] |=  UVC_HEADER_BAD_PAYLOAD;
    }
    else {
        buf[1] &= ~UVC_HEADER_BAD_PAYLOAD;
    }
    if (is_frame_end) {
        buf[1]          |= UVC_HEADER_EOF;
        g_uvc_header[1] ^= UVC_HEADER_FRAME_ID;
    }
    else {
        buf[1] &= ~UVC_HEADER_EOF;
    }
}

static int set_interface(struct usb_gadget *gadget, f_uvc_camera_t *uvc, int alt)
{
    int  ret = 0;
    usb_request_t *req = NULL;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);

    LOGI(__func__, "in...");

    if (alt == -1) {
        LOGE(__func__, "alt is -1, goto cleanup");
        goto cleanup;
    }

    /* Already set? */
    if (uvc->video_in_ep) {
        return 0;
	}

    ret = uvc_camera_enable_eps(gadget, uvc);
    if (ret < 0) {
        goto cleanup;
    }

    ret = -ENOMEM;

    // allocate requests and queue requests for VIDEO IN
    req = uvc->video_in_ep->ops->alloc_request(uvc->video_in_ep, GFP_ATOMIC);
    if (req == NULL) {
        goto cleanup;
    }
    req->buf    = NULL;
    req->dma    = (uintptr_t)req->buf;
    req->length = 0;
    req->zero   = 0;
    req->complete = complete;
    req->context  = (void *)&m_uvc_stream_state;

    uvc->video_in_req = req;

    // NOTE: No frame_buf to send now, set state to STREAM_STATE_UNINITED
    usb_func_set_stream_state(req, STREAM_STATE_UNINITED);
    m_remain_size = 0;
    LOGI(__func__, "video_in_ep 0x%x, video_in_req is 0x%x", 
            uvc->video_in_ep, uvc->video_in_req);

    // allocate requests and queue requests for CMD OUT
    req = uvc->ext_cmd_out_ep->ops->alloc_request(uvc->ext_cmd_out_ep, GFP_ATOMIC);
    if (req == NULL) {
        goto cleanup;
    }
    configASSERT(uvc->ext_cmd_out_buf != NULL);
    req->buf    = uvc->ext_cmd_out_buf;
    req->dma    = (uintptr_t)req->buf;
    req->actual = 0;
    req->length = DWC_REQ_SIZE;
    req->zero   = 0;
    req->complete = complete;
    req->context  = (void *)&m_acm_recv_state;

    uvc->ext_cmd_out_req = req;

    LOGI(__func__, "ext_cmd_out_ep 0x%x, ext_cmd_out_req is 0x%x", 
            uvc->ext_cmd_out_ep, uvc->ext_cmd_out_req);

    usb_func_set_stream_state(req, STREAM_STATE_INITED);
    ret = uvc->ext_cmd_out_ep->ops->queue(uvc->ext_cmd_out_ep, req, GFP_ATOMIC);
    if (ret < 0) {
        goto cleanup;
    }
    usb_func_set_stream_state(req, STREAM_STATE_BUSY);

    // allocate requests and queue requests for RESP IN
    req = uvc->ext_resp_in_ep->ops->alloc_request(uvc->ext_resp_in_ep, GFP_ATOMIC);
    if (req == NULL) {
        goto cleanup;
    }
    configASSERT(uvc->ext_resp_in_buf != NULL);
    req->buf    = uvc->ext_resp_in_buf;
    req->dma    = (uintptr_t)req->buf;
    req->length = 0; //DWC_REQ_SIZE;
    req->zero   = 0;
    req->complete = complete;
    req->context  = (void *)&m_acm_send_state;

    uvc->ext_resp_in_req = req;
    
    LOGI(__func__, "ext_resp_in_ep 0x%x, ext_resp_in_req is 0x%x", 
            uvc->ext_resp_in_ep, uvc->ext_resp_in_req);

    usb_func_set_stream_state(req, STREAM_STATE_INITED);
    ret = uvc->ext_resp_in_ep->ops->queue(uvc->ext_resp_in_ep, req, GFP_ATOMIC);
    if (ret < 0) {
        goto cleanup;
    }
    usb_func_set_stream_state(req, STREAM_STATE_BUSY);

    return 0;

cleanup:
    LOGE (__func__, "set_interface error, cleanup...");
    if (uvc->video_in_req != NULL) {
        LOGI(__func__, "video_in_ep: free request 0x%x", uvc->video_in_req);
        uvc->video_in_ep->ops->free_request(uvc->video_in_ep, uvc->video_in_req);
        uvc->video_in_req = NULL;
    }

    if (uvc->ext_cmd_out_req != NULL) {
        LOGI(__func__, "ext_cmd_out_ep: free request 0x%x", uvc->ext_cmd_out_req);
        uvc->ext_cmd_out_ep->ops->free_request(uvc->ext_cmd_out_ep, uvc->ext_cmd_out_req);
        uvc->ext_cmd_out_req = NULL;
    }

    if (uvc->ext_resp_in_req != NULL) {
        LOGI(__func__, "ext_resp_in_ep: free request 0x%x", uvc->ext_resp_in_req);
        uvc->ext_resp_in_ep->ops->free_request(uvc->ext_resp_in_ep, uvc->ext_resp_in_req);
        uvc->ext_resp_in_req = NULL;
    }

    uvc_camera_disable_eps(gadget, uvc);

    return ret;
}

static int get_interface(struct usb_gadget *gadget, f_uvc_camera_t *uvc)
{
    int ret = 0;
    uint8_t ifc = (uint8_t)uvc->ifc;

    ret = dwc_usb_ep0_send_data(gadget, &ifc, 1);

    return ret;
}

static int get_config(struct usb_gadget *gadget, f_uvc_camera_t *uvc)
{
    int ret = 0;
    uint8_t cfg = (uint8_t)uvc->cfg;

    ret = dwc_usb_ep0_send_data(gadget, &cfg, 1);

    return ret;
}

static int update_uvc_request(usb_request_t *req, int remain_size, int total_size)
{
    int is_frame_end = 0;
    int is_bad_frame = 0;

    configASSERT(req != NULL);

    if (remain_size > VIDEO_MAX_DATA_SIZE) {
        /*UVC_DEBUG(__func__, "remain_size is %d, max size %d, split it", remain_size, VIDEO_MAX_DATA_SIZE);*/
        // Need to split the frame buffer
        req->length  = VIDEO_MAX_DATA_SIZE + UVC_MAX_HEADER;
        // adjust the frame_size
        remain_size -= VIDEO_MAX_DATA_SIZE;
        // set zero flag.
        req->zero = 0;
    }
    else {
        /*UVC_DEBUG(__func__, "Frame End !!!, frame_size is %d", remain_size);*/
        // Frame end !!!
        req->length = remain_size + UVC_MAX_HEADER;
        // set frame end flag
        is_frame_end = 1;
        // set zero flag.
        req->zero = 1;
        // check bad frame end
        if (remain_size != VIDEO_MAX_DATA_SIZE) {
            if (remain_size != (total_size % VIDEO_MAX_DATA_SIZE)) {
                LOGE(__func__, "Bad frame!!! frame_size %d, expected %d",
                        remain_size, total_size % VIDEO_MAX_DATA_SIZE);
                is_bad_frame = 1;
            }
        }
        remain_size = 0;
    }

    uvc_add_header(req->buf, is_frame_end, is_bad_frame);
    dcache_flush(req->buf, UVC_MAX_HEADER);

    return remain_size;
}

static int video_in_ep_complete(usb_ep_t *ep, usb_request_t *req)
{
    int ret = 0;
    usb_stream_state_t state = STREAM_STATE_UNINITED;

    configASSERT(ep  == m_uvc_camera.video_in_ep);
    configASSERT(req == m_uvc_camera.video_in_req);
    
    // if error existed in last req, send it again
    if (req->length != req->actual) {
        LOGE(__func__, "req->length(%d) != req->actual (%d)", req->length, req->actual);
        ret = ep->ops->queue(ep, req, GFP_ATOMIC);
        if (ret < 0) {
            LOGE(__func__, "Queue req %x error!, %d", req, ret);
        }
        return ret;
    }

    state = usb_func_get_stream_state(req);
    UVC_DEBUG(__func__, "video in ep ..., m_cur_frame 0x%x", m_cur_frame);

    switch (state) {
        case STREAM_STATE_BUSY:
            UVC_DEBUG(__func__, "video_in_ep: STREAM_STATE_BUSY");
            // The frame frame transfer in process, adjust the req->buf
            configASSERT(req->buf != NULL);
            req->buf = (void *)((uintptr_t)req->buf + VIDEO_MAX_DATA_SIZE);
            req->dma = (dwc_dma_t)req->buf;
            configASSERT((req->dma & 3UL) == 0);
            
            m_remain_size = update_uvc_request(req, m_remain_size, m_cur_frame->used_bytes);
            if (m_remain_size != 0) {
                state = STREAM_STATE_BUSY;
            }
            else {
                UVC_DEBUG(__func__, "video_in_ep: switch to STREAM_STATE_LAST_DATA!");
                state = STREAM_STATE_LAST_DATA;
            }
            usb_func_set_stream_state(req, state);

            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
            if (ret < 0) {
                LOGE(__func__, "video_in_ep: Queue req 0x%08x failed! %d", req, ret);
            }
            break;
        case STREAM_STATE_LAST_DATA:
            UVC_DEBUG(__func__, "video_in_ep: STREAM_STATE_LAST_DATA");
            // The last data transfer completed
#ifndef SINGLE_BUFFER
            if (m_uvc_strm_started && m_ready_frame_updated) {
                // m_ready_frame has been updated, send it and switch to busy status
                configASSERT(m_ready_frame != NULL);
                configASSERT(m_cur_frame != NULL);
                configASSERT(m_cur_frame->fmt = m_ready_frame->fmt);
                /*configASSERT(m_cur_frame->used_bytes == m_ready_frame->used_bytes);*/
                
                SWAP_FRAME_BUFS(m_cur_frame, m_ready_frame);

                // Frame start!!! Start stransfer
                req->buf  = (uint8_t *)m_cur_frame->data[0] - UVC_MAX_HEADER;
                req->dma  = (dwc_dma_t)req->buf;
                req->zero = 0;
                
                m_remain_size = update_uvc_request(req, m_cur_frame->used_bytes, m_cur_frame->used_bytes);

                if (m_remain_size != 0) {
                    UVC_DEBUG(__func__, "State is STREAM_STATE_LAST_DATA, switch to STREAM_STATE_BUSY");
                    state = STREAM_STATE_BUSY;
                }
                else {
                    UVC_DEBUG(__func__, "State is STREAM_STATE_LAST_DATA, switch to STREAM_STATE_LAST_DATA");
                    state = STREAM_STATE_LAST_DATA;
                }

                ret = ep->ops->queue(ep, req, GFP_ATOMIC);
                if (ret < 0) {
                    LOGE(__func__, "Queue req %x error!, %d", req, ret);
                }
                m_ready_frame_updated = false;
            } 
            else {
                // No data to send, switch to IDLE
                state = STREAM_STATE_IDLE;
            }
#else
                state = STREAM_STATE_IDLE;
#endif // SINGLE_BUFFER
            usb_func_set_stream_state(req, state);
            break;
        case STREAM_STATE_UNINITED:
            LOGW(__func__, "video_in_ep: STREAM_STATE_UNINITED");
            break;
        case STREAM_STATE_INITED:
            LOGW(__func__, "video_in_ep: STREAM_STATE_INITED");
            break;
        case STREAM_STATE_IDLE:
            LOGW(__func__, "video_in_ep: STREAM_STATE_IDLE");
            break;
        default:
            LOGE(__func__, "video_in_ep: Unknown state %d", state);
            configASSERT(!"video_in_ep: Unknown state!");
            break;
    }
    return ret;
}

static int ext_cmd_out_ep_complete(usb_ep_t *ep, usb_request_t *req)
{
    int ret = 0;
    usb_stream_state_t state;

    configASSERT(ep  == m_uvc_camera.ext_cmd_out_ep);
    configASSERT(req == m_uvc_camera.ext_cmd_out_req);

    // recv something
    state = usb_func_get_stream_state(req);
    /*UVC_DEBUG(__func__, "ext_cmd_out_ep: state %d", state);*/
    switch (state) {
        case STREAM_STATE_BUSY:
            UVC_DEBUG(__func__, "STREAM_STATE_BUSY");
            if (req->actual) {
                if (!list_empty(&m_acm_recv_free_list)) {
                    usb_func_acm_buf_t *acm_buf = NULL;
                    acm_buf = usb_acm_get_buffer(&m_acm_recv_free_list);
                    configASSERT(acm_buf != NULL);
                    configASSERT(acm_buf->buffer != NULL);
                    
                    SWAP_PTRS(acm_buf->buffer, req->buf);

                    req->dma    = (dwc_dma_t)req->buf;
                    req->length = acm_buf->size;
                    
                    acm_buf->count  = req->actual;
                
                    UVC_DEBUG(__func__, "ext_cmd_out_ep: Recv 0x%08x, %d bytes", acm_buf->buffer, acm_buf->count);

                    usb_acm_add_buffer_tail(&m_acm_recv_list, acm_buf);
                    
                    // Check whether this is the last element
                    if (list_empty(&m_acm_recv_free_list)) {
                        UVC_DEBUG(__func__, "ext_cmd_out_ep: switch to STREAM_STATE_LAST_DATA!");
                        state = STREAM_STATE_LAST_DATA;
                    }
                    else {
                        state = STREAM_STATE_BUSY;
                    }
                    
                    ret = ep->ops->queue(ep, req, GFP_ATOMIC);
                    if (ret < 0) {
                        LOGE(__func__, "Queue req 0x%x to ext_cmd_out_ep error!", req);
                    }
                }
                else {
                    UVC_DEBUG(__func__, "ext_cmd_out_ep: m_acm_recv_free_list is empty, switch to STREAM_STATE_IDLE!");
                    state = STREAM_STATE_IDLE;
                }
            }
            else {
                // No bytes received, switch to IDLE state to start a new request!
                LOGW(__func__, "req->actual is 0! switch to STREAM_STATE_IDLE");
                state = STREAM_STATE_IDLE;
            }
            usb_func_set_stream_state(req, state);
            break;
        case STREAM_STATE_LAST_DATA:
            // The free buffer filled, switch to ready state to start a new request!
            UVC_DEBUG(__func__, "ext_cmd_out_ep: STREAM_STATE_LAST_DATA");
            state = STREAM_STATE_IDLE;
            usb_func_set_stream_state(req, state);
            break;
        case STREAM_STATE_UNINITED:
            LOGW(__func__, "ext_cmd_out_ep: STREAM_STATE_UNINITED");
            break;
        case STREAM_STATE_INITED:
            LOGW(__func__, "ext_cmd_out_ep: STREAM_STATE_INITED");
            break;
        case STREAM_STATE_IDLE:
            LOGW(__func__, "ext_cmd_out_ep: STREAM_STATE_IDLE");
            break;
        default:
            LOGE(__func__, "ext_cmd_out_ep: Unknown state %d", state);
            configASSERT(!"Unknown state");
    }

    return ret;
}

static void ext_resp_in_ep_complete(usb_ep_t *ep, usb_request_t *req)
{
    int ret = 0;
    usb_stream_state_t state;

    configASSERT(ep  == m_uvc_camera.ext_resp_in_ep);
    configASSERT(req == m_uvc_camera.ext_resp_in_req);

    // send something
    state = usb_func_get_stream_state(req);
    /*UVC_DEBUG(__func__, "exp_resp_in_ep: state %d", state);*/
    switch (state) {
        case STREAM_STATE_BUSY:
            UVC_DEBUG(__func__, "exp_resp_in_ep: STREAM_STATE_BUSY");
            if (!list_empty(&m_acm_send_list)) {
                usb_func_acm_buf_t *acm_buf = usb_acm_get_buffer(&m_acm_send_list);
                configASSERT(acm_buf != NULL);
                configASSERT(acm_buf->buffer != NULL);

                // Swap acm_buf->buffer with req->buf
                SWAP_PTRS(acm_buf->buffer, req->buf);
                
                // Update dma addr and req->length
                req->dma    = (dwc_dma_t)req->buf;
                req->length = acm_buf->count;
               
                // Clear count and offset to put the buffer into free list
                acm_buf->count  = 0;
                acm_buf->offset = 0;
                acm_buf->size   = DWC_REQ_SIZE;
                usb_acm_add_buffer_tail(&m_acm_send_free_list, acm_buf);
                
                // Check whether it is the the last element 
                if (list_empty(&m_acm_send_list)) {
                    UVC_DEBUG(__func__, "exp_resp_in_ep: send the last data, switch to STREAM_STATE_LAST_DATA");
                    state = STREAM_STATE_LAST_DATA;
                }
                else {
                    state = STREAM_STATE_BUSY;
                }
                
                ret = ep->ops->queue(ep, req, GFP_ATOMIC);
                if (ret < 0) {
                    // Should never happen
                    LOGE(__func__, "exp_resp_in_ep: Queue req 0x%x to ext_resp_in_ep error!", req);
                }
            }
            else {
                UVC_DEBUG(__func__, "exp_resp_in_ep: m_acm_send_list is empty, switch to STREAM_STATE_IDLE");
                state = STREAM_STATE_IDLE;
            }
            usb_func_set_stream_state(req, state);
            break;
        case STREAM_STATE_LAST_DATA:
            UVC_DEBUG(__func__, "ext_resp_in_ep: STREAM_STATE_LAST_DATA");
            // All data are sent, switch to ready state to start a new request
            state = STREAM_STATE_IDLE;
            usb_func_set_stream_state(req, state);
            break;
        case STREAM_STATE_UNINITED:
            LOGW(__func__, "ext_resp_in_ep: STREAM_STATE_UNINITED!");
            break;
        case STREAM_STATE_INITED:
            LOGW(__func__, "ext_resp_in_ep: STREAM_STATE_INITED");
        case STREAM_STATE_IDLE:
            LOGW(__func__, "ext_resp_in_ep: STREAM_STATE_IDLE");
            break;
        default:
            LOGE(__func__, "ext_resp_in_ep: Unknown state %d", state);
            configASSERT(!"Unknown state");
    }
    /*UVC_DEBUG(__func__, "ext_resp_in_ep: Set state to %d", state);*/
    /*usb_func_set_stream_state(req, state);*/
}

static void complete(usb_ep_t *ep, usb_request_t *req)
{
    int ret = 0;
    //usb_stream_state_t state;

    UVC_DEBUG(__func__, "in...");
    configASSERT((ep != NULL) && (req != NULL));

	switch (req->status) {
		case 0:
			if (ep == m_uvc_camera.video_in_ep) {
                video_in_ep_complete(ep, req);
			}
			else if (ep == m_uvc_camera.ext_cmd_out_ep) {
                ext_cmd_out_ep_complete(ep, req);
			}
			else if (ep == m_uvc_camera.ext_resp_in_ep) {
                ext_resp_in_ep_complete(ep, req);
			}
			else {
				LOGE(__func__, "Unknown endpoint 0x%x", ep);
			}
			break;
		default:
			/* Requeue for a future OUT EP transfer */
            LOGW(__func__, "ep 0x%x, req 0x%x, status %d", ep, req, req->status);
			req->length = DWC_REQ_SIZE;
            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
			if (ret == 0) {
				return;
            }
			/* FALL-THRU */
            LOGE(__func__, "Failed ot queue OUT req!");
		case -ESHUTDOWN:
			/*dwc_debug0(&g_usb3_dev, "Shutdown status\n");*/
            LOGE(__func__, "Shutdown status!");
            if (ep == m_uvc_camera.video_in_ep) {
                if (m_uvc_camera.video_in_req != NULL) {
                    LOGI(__func__, "video_in_ep, free request 0x%x ...", req);
                    configASSERT(req == m_uvc_camera.video_in_req);
                    ep->ops->free_request(ep, req);
                    m_uvc_camera.video_in_req = NULL;
                }
            }
            else if (ep == m_uvc_camera.ext_cmd_out_ep) {
                if (m_uvc_camera.ext_cmd_out_req != NULL) {
                    LOGI(__func__, "ext_cmd_out_ep, free request 0x%x ...", req);
                    configASSERT(req == m_uvc_camera.ext_cmd_out_req);
                    ep->ops->free_request(ep, req);
                    m_uvc_camera.ext_cmd_out_req = NULL;
                }
            }
            else if (ep == m_uvc_camera.ext_resp_in_ep) {
                if (m_uvc_camera.ext_resp_in_req != NULL) {
                    LOGI(__func__, "ext_resp_in_ep, free request 0x%x ...", req);
                    configASSERT(req == m_uvc_camera.ext_resp_in_req);
                    ep->ops->free_request(ep, req);
                    m_uvc_camera.ext_resp_in_req = NULL;
                }
            }
            else {
                LOGE(__func__, "Unknown ep 0x%x", ep);
            }
			break;
	}
}

static void ep0_complete(usb_ep_t *ep, usb_request_t *req)
{
    (void)(ep);

    if (req->status || req->actual != req->length) {
        LOGW(__func__, "setup complete --> %d, %d/%d", req->status, req->actual, req->length);
    }

    /*LOGI(__func__, "req->status is %d, req->actual is %d", req->status, req->actual);*/
}

static int bind(struct usb_gadget *gadget, struct usb_gadget_driver *driver)
{
    (void)(driver);

    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);

    if (m_usb_function_binded) {
        LOGW(__func__, "Gadget has been bound, unbind it firstly!", gadget);
        unbind(gadget);
    }

    usb_function_get_lock();
    
    gadget->max_payload_size = VIDEO_REQ_SIZE;
    gadget->priv = (void *)&EP0_DESC_SEL;

    ret = usb_acm_buffer_lists_init();
    if (ret < 0) {
        LOGE(__func__, "usb_acm_buffer_lists_init failed!");
        goto cleanup;
    }

    gadget->ep0_req = ep0->ops->alloc_request(ep0, GFP_ATOMIC);
    configASSERT(gadget->ep0_req != NULL);

    gadget->ep0_req->buf = &g_ep0_status_buf[0];
    aiva_memset(gadget->ep0_req->buf, 0, USB_EP0_BUF_SIZE);
    gadget->ep0_req->complete = ep0_complete;

    usb_func_acm_buf_t *free_buf;

    free_buf = usb_acm_get_buffer(&m_acm_recv_free_list);
    m_uvc_camera.ext_cmd_out_buf = free_buf->buffer;
    if (m_uvc_camera.ext_cmd_out_buf == NULL) {
        goto cleanup;
    }
    aiva_memset(m_uvc_camera.ext_cmd_out_buf, 0, DWC_REQ_SIZE);
    dcache_flush(m_uvc_camera.ext_cmd_out_buf, DWC_REQ_SIZE);
    
    free_buf = usb_acm_get_buffer(&m_acm_send_free_list);
    m_uvc_camera.ext_resp_in_buf = free_buf->buffer;
    if (m_uvc_camera.ext_resp_in_buf == NULL) {
        goto cleanup;
    }
    aiva_memset(m_uvc_camera.ext_resp_in_buf, 0, DWC_REQ_SIZE);
    dcache_flush(m_uvc_camera.ext_resp_in_buf, DWC_REQ_SIZE);

    m_usb_function_binded = true;

    usb_function_release_lock();

    return 0;

cleanup:
    if (gadget->ep0_req != NULL) {
        ep0->ops->free_request(ep0, gadget->ep0_req);
        gadget->ep0_req = NULL;
    }

    usb_acm_buffer_list_release();

    m_uvc_camera.ext_cmd_out_buf = NULL;
    m_uvc_camera.ext_resp_in_buf = NULL;

    usb_function_release_lock();

    return ret;
}

static void unbind(struct usb_gadget *gadget)
{
    struct usb_ep *ep0 = gadget->ep0;
    
    configASSERT(ep0 != NULL);
    configASSERT(gadget->ep0_req != NULL);
    LOGI(__func__, "in...");

    usb_function_get_lock();

    m_usb_function_binded = false;
    
    xvic_irq_disable(USB_IRQn);

    if (gadget->ep0_req != NULL) {
        ep0->ops->free_request(ep0, gadget->ep0_req);
        gadget->ep0_req = NULL;
    }

    if (m_cur_frame != NULL) {
        configASSERT(m_cur_frame->_refcnt == 1);
        frame_mgr_decr_ref(m_cur_frame);
        LOGI(__func__, "free m_cur_frame 0x%08x", m_cur_frame);
        m_cur_frame = NULL;
    }
    if (m_ready_frame != NULL) {
        configASSERT(m_ready_frame->_refcnt == 1);
        frame_mgr_decr_ref(m_ready_frame);
        LOGI(__func__, "free m_ready_frame 0x%08x", m_ready_frame);
        m_ready_frame = NULL;
    }
    
    /* Set m_uvc_stream_state to uninitialized 
     * since m_cur_frame and m_ready_frame have been freed.*/
    m_uvc_stream_state = STREAM_STATE_UNINITED;

    usb_acm_buffer_list_release();

    m_uvc_camera.ext_cmd_out_buf = NULL;
    m_uvc_camera.ext_resp_in_buf = NULL;

    usb_function_release_lock();
}

static int setup(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    u16    windex  = UGETW(ctrl->wIndex);
    u16    wvalue  = UGETW(ctrl->wValue);
    //u16    wlength = UGETW(ctrl->wLength);
    int    ret  = 0;
    int    alt  = 0;
    int    intf = 0;
    
    ret = 0;
    if (UT_GET_TYPE(ctrl->bmRequestType) == UT_STANDARD) {
        switch (ctrl->bRequest) {
            case UR_GET_DESCRIPTOR:
                uvc_camera_get_desc(gadget, ctrl);
                break;
            case UR_SET_FEATURE:
                UVC_DEBUG(__func__, "USB_REQ_SET_FEATURE");
                // do nothing
                dwc_usb_ep0_wait_nrdy(gadget);
                break;
            case UR_SET_INTERFACE:
                // bRequest      | wValue | wIndex      | wLength | Data
                // --------------+--------+-------------+---------+------
                // SET_INTERFACE | alt    | interface   | 0       | None
 
                alt  = wvalue;
                intf = windex;

                UVC_DEBUG(__func__, "USB_REQ_SET_INTERFACE, intf %d, alt %d", intf, alt);
                if (!m_uvc_camera.cfg){
                    LOGE(__func__, "Device hasn't been configured, cfg = %x, intf %d, alt %d", 
                            m_uvc_camera.cfg, intf, alt);
                    return -ENOTSUPP;
                }

                if ((intf == UVC_STREAM_INTERFACE) && (alt == 0)) {
                    uvc_stop_stream(gadget);
                }

                /* If interface alt has changed, disable the old EPs and enable the new ones */
                if (m_uvc_camera.ifc != alt) {
                    set_interface(gadget, &m_uvc_camera, -1);
                    ret = set_interface(gadget, &m_uvc_camera, wvalue);
                    if (ret < 0) {
                        return ret;
                    }
                }
                m_uvc_camera.ifc = alt;
                dwc_usb_ep0_wait_nrdy(gadget);
                break;
            case UR_GET_INTERFACE:
                // bRequest      | wValue | wIndex      | wLength | Data
                // --------------+--------+-------------+---------+----------------
                // GET_INTERFACE | 0      | Interface   | 1       | Alter interface
                
                intf = windex;
                UVC_DEBUG(__func__, "USB_REQ_GET_INTERFACE, intf %d", intf);
                if (!m_uvc_camera.cfg) {
                    LOGE(__func__, "cfg = %x", m_uvc_camera.cfg);
                    return -ENOTSUPP;
                }
                // send the current m_uvc_camera.ifc back
                ret = get_interface(gadget, &m_uvc_camera);
                break;
            case UR_SET_CONFIG:
                // bRequest   | wValue | wIndex | wLength | Data
                // -----------+--------+--------+---------+------
                // SET_CONFIG | Config | 0      | 0       | None 
                UVC_DEBUG(__func__, "UR_SET_CONFIG, cfg = %d", wvalue);
                uvc_stop_stream(gadget);
                if (wvalue != 0 && wvalue != 1) {
                    LOGE(__func__, "wvalue = %x", wvalue);
                    return -ENOTSUPP;
                }

                /* If config already set, clear it and disable the EPs */
                if (m_uvc_camera.cfg) {
                    m_uvc_camera.cfg = 0;
                    m_uvc_camera.ifc = 0;
                    // set interface -1
                    set_interface(gadget, &m_uvc_camera, -1);
                }
            
                /* If new config is 1, enable the EPs for interface 0 */
                if (wvalue) {
                    m_uvc_camera.cfg = wvalue;
                    // set interface 0
                    ret = set_interface(gadget, &m_uvc_camera, 0);
                    if (ret) {
                        m_uvc_camera.cfg = 0;
                    }
                }
                dwc_usb_ep0_wait_nrdy(gadget);
                break;
            case UR_GET_CONFIG:
                UVC_DEBUG(__func__, "USB_REQ_GET_CONFIG");
                // send m_uvc_camera.cfg back
                ret = get_config(gadget, &m_uvc_camera);
                break;
            case UR_SYNCH_FRAME:
                UVC_DEBUG(__func__, "USB_REQ_SYNCH_FRAME\n");
                // do nothing
                dwc_usb_ep0_wait_nrdy(gadget);
                break;
            default:
                LOGW(__func__, "(%d) Unsupported bmRequestType 0x%x, bRequest 0x%x", 
                    __LINE__, ctrl->bmRequestType, ctrl->bRequest);
                ret = -ENOTSUPP;
                break;
        }
    }
    else if (UT_GET_TYPE(ctrl->bmRequestType) == UT_CLASS) {
        if ((ctrl->bmRequestType == USB_UVC_GET_REQ_TYPE) || 
                (ctrl->bmRequestType == USB_UVC_SET_REQ_TYPE)) {
            UVC_DEBUG(__func__, "USB_UVC_GET_REQ_TYPE/USB_UVC_SET_REQ_TYPE");
            switch (ctrl->wIndex[0]) {
                case UVC_CONTROL_INTERFACE:
                    UVC_DEBUG(__func__, "UVC_CONTROL_INTERFACE");
                    ret = uvc_handle_vc_reqs(gadget, ctrl);
                    break;
                case UVC_STREAM_INTERFACE:
                    UVC_DEBUG(__func__, "UVC_STREAM_INTERFACE");
                    ret = uvc_handle_vs_reqs(gadget, ctrl);
                    break;
                case CDC_ACM_INTERFACE:
                    UVC_DEBUG(__func__, "CDC_ACM_INTERFACE");
                    ret = cdc_acm_handle_reqs(gadget, ctrl);
                    break;
                default:
                    UVC_DEBUG(__func__, "unkown index(%d), do nothing...", windex);
                    dwc_usb_ep0_wait_nrdy(gadget);
                    ret = 0;
            }
        }
        else if (ctrl->bmRequestType == USB_SET_INTF_REQ_TYPE) {
            UVC_DEBUG(__func__, "USB_SET_INTF_REQ_TYPE");
            if (ctrl->bRequest == USB_SET_INTF_REQ_TYPE) {
                if ((windex == UVC_STREAM_INTERFACE) && (wvalue == 0)) {
                    UVC_DEBUG(__func__, "Alternate setting 0 ...");
                    dwc_usb_ep0_wait_nrdy(gadget);
                    ret = 0;
                }
                else {
                    LOGW(__func__, "unkown index(%d), do nothing...", windex);
                    dwc_usb_ep0_wait_nrdy(gadget);
                    ret = 0;
                }
            }
            else {
                LOGW(__func__, "unkown bRequest(%d), do nothing...", ctrl->bRequest);
                dwc_usb_ep0_wait_nrdy(gadget);
                ret = 0;
            }
        }
        else {
            LOGW(__func__, "(%d) Unsupported bmRequestType 0x%x, bRequest 0x%x", 
                    __LINE__, ctrl->bmRequestType, ctrl->bRequest);
            ret = -ENOTSUPP;
        }
    }
    else if (UT_GET_TYPE(ctrl->bmRequestType) == UT_VENDOR) {
        LOGW(__func__, "(%d) UT_VENDOR: bmRequestType 0x%x, bRequest 0x%x", 
                    __LINE__, ctrl->bmRequestType, ctrl->bRequest);
        ret = -ENOTSUPP;
    }
    else {
        LOGE(__func__, "Unknown request 0x%x", ctrl->bRequest);
        ret = -ENOTSUPP;
    }

    return ret;
}

static void disconnect(struct usb_gadget *gadget)
{
    (void)(gadget);

    f_uvc_camera_t *uvc = &m_uvc_camera;
    // TODO
    LOGI(__func__, "in...");
    m_remain_size = 0;
    if (uvc->video_in_req != NULL) {
        configASSERT(uvc->video_in_ep != NULL);
        LOGI(__func__, "video_in_ep: free_request 0x%x", uvc->video_in_req);
        uvc->video_in_ep->ops->free_request(uvc->video_in_ep, uvc->video_in_req);
        uvc->video_in_req = NULL;
    }

    if (uvc->ext_cmd_out_req != NULL) {
        configASSERT(uvc->ext_cmd_out_ep != NULL);
        LOGI(__func__, "ext_cmd_out_ep: free_request 0x%x", uvc->ext_cmd_out_req);
        uvc->ext_cmd_out_ep->ops->free_request(uvc->ext_cmd_out_ep, uvc->ext_cmd_out_req);
        uvc->ext_cmd_out_req = NULL;
    }

    if (uvc->ext_resp_in_req != NULL) {
        configASSERT(uvc->ext_resp_in_ep != NULL);
        LOGI(__func__, "ext_resp_in_ep: free_request 0x%x", uvc->ext_resp_in_req);
        uvc->ext_resp_in_ep->ops->free_request(uvc->ext_resp_in_ep, uvc->ext_resp_in_req);
        uvc->ext_resp_in_req = NULL;
    }

    /*uvc_camera_disable_eps(gadget, uvc);*/
    LOGI(__func__, "out...");
}

static void suspend(struct usb_gadget *gadget)
{
    (void)(gadget);

    // TODO
    /*LOGI(__func__, "in...");*/
    /*LOGI(__func__, "out...");*/
}

static void resume(struct usb_gadget *gadget)
{
    (void)(gadget);

    // TODO
    /*LOGI(__func__, "in...");*/
    /*LOGI(__func__, "out...");*/
}

static void reset(struct usb_gadget *gadget)
{
    (void)(gadget);

    // TODO
    LOGI(__func__, "in...");
    LOGI(__func__, "out...");
}

struct usb_gadget_driver *usb_func_uvc_camera(void)
{
    return &m_uvc_bulk_gadget;
}

static bool usb_bulk_acm_inited(void)
{
    return m_dwc_usb3_acm_inited;
}

#define MAX_COMMIT_CNT                  (2000)

static int usb_bulk_commit_frame(frame_buf_t *cur)
{
    int ret                     = 0;
    usb_ep_t      *ep  = NULL;
    usb_request_t *req = NULL;
    static int pre_remain_size  = 0;;
    static int timeout          = MAX_COMMIT_CNT;

    usb_function_get_lock();

    if (!m_uvc_strm_started) {
        frame_mgr_decr_ref(cur);
        ret = -1;
        goto out;
    }
    
    ep  = (usb_ep_t *)m_uvc_camera.video_in_ep;
    req = (usb_request_t *)m_uvc_camera.video_in_req;
    if (ep == NULL) {
        /*LOGW(__func__, "video_in_ep is NULL!");*/
        frame_mgr_decr_ref(cur);
        ret = -1;
        goto out;
    }
 
    if (req == NULL) {
        /*LOGW(__func__, "video_in_req is NULL!");*/
        frame_mgr_decr_ref(cur);
        ret = -1;
        goto out;
    }

    // UVC will swap data buffer!!!
    configASSERT(cur->_refcnt == 1);
    
    usb_stream_state_t state = usb_func_get_stream_state(req);
    /*UVC_DEBUG(__func__, "Commit frame %x, state is %d", cur, state);*/
    switch (state) {
        case STREAM_STATE_UNINITED:
            // init m_cur_frame pointer
            if (m_cur_frame != NULL) {
                configASSERT(m_cur_frame->_refcnt == 1);
                frame_mgr_decr_ref(m_cur_frame);
            }
            m_cur_frame   = cur;
#ifndef SINGLE_BUFFER
            m_remain_size = 0; //cur->used_bytes;
            UVC_DEBUG(__func__, "State is STREAM_STATE_UNINITED, switch to STREAM_STATE_INITED");
            state = STREAM_STATE_INITED;
            xvic_irq_disable(USB_IRQn);
            usb_func_set_stream_state(req, state);
            xvic_irq_enable(USB_IRQn);
            break;
        case STREAM_STATE_INITED:
            if (m_ready_frame != NULL) {
                configASSERT(m_ready_frame->_refcnt == 1);
                frame_mgr_decr_ref(m_ready_frame);
            }
            m_ready_frame  = cur;
            m_remain_size  = 0; //cur->used_bytes;
#endif
            UVC_DEBUG(__func__, "State is STREAM_STATE_INITED, switch to STREAM_STATE_IDLE");
            state = STREAM_STATE_IDLE;
            xvic_irq_disable(USB_IRQn);
            usb_func_set_stream_state(req, state);
            xvic_irq_enable(USB_IRQn);
            break;
        case STREAM_STATE_IDLE:
            xvic_irq_disable(USB_IRQn);
            // swap the cur frame pointer
#ifndef SINGLE_BUFFER
            if (!m_ready_frame_updated) {
                // ready frame has been sent by DMA, send the current frame directly
                SWAP_FRAME_BUFS(m_cur_frame, cur);
            }
            else {
                // ready frame hasn't been sent by DMA
                SWAP_FRAME_BUFS(m_cur_frame, m_ready_frame);
                SWAP_FRAME_BUFS(m_ready_frame, cur);
            }
#else
            SWAP_FRAME_BUFS(m_cur_frame, cur);
#endif
            // Frame start!!! Start stransfer
            req->buf  = (uint8_t *)m_cur_frame->data[0] - UVC_MAX_HEADER;
            req->dma  = (dwc_dma_t)req->buf;

            m_remain_size = update_uvc_request(req, m_cur_frame->used_bytes, m_cur_frame->used_bytes);

            if (m_remain_size != 0) {
                UVC_DEBUG(__func__, "State is STREAM_STATE_IDLE, switch to STREAM_STATE_BUSY");
                state = STREAM_STATE_BUSY;
            }
            else {
                UVC_DEBUG(__func__, "State is STREAM_STATE_IDLE, switch to STREAM_STATE_LAST_DATA");
                state = STREAM_STATE_LAST_DATA;
            }
            usb_func_set_stream_state(req, state);

            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
            if (ret < 0) {
                LOGE(__func__, "Queue req %x error!, %d", req, ret);
            }
            xvic_irq_enable(USB_IRQn);
            frame_mgr_decr_ref(cur);
            break;
        case STREAM_STATE_BUSY:
        case STREAM_STATE_LAST_DATA:
            if (pre_remain_size != m_remain_size) {
                timeout = MAX_COMMIT_CNT;
            }
            else {
                /*LOGW(__func__, "The pre_remain_size is equal to m_remain_size %d", m_remain_size);*/
                timeout--;
            }
            pre_remain_size = m_remain_size;
            if (timeout <= 0) {
                // Check timeout, though it is unlikely.
                LOGW(__func__, "Timeout! m_remain_size is always %d!", m_remain_size);
                ret = ep->ops->dequeue(ep, req);
                // Dequeue the last req to force reset connection
                if (ret < 0) {
                    LOGE(__func__, "Dequeue req %x error! %d", req, ret);
                }
                /*usb_func_set_stream_state(req, STREAM_STATE_UNINITED);*/
                timeout = MAX_COMMIT_CNT;
            }
            // wait for complete interrupt to switch state
#ifndef SINGLE_BUFFER
            if (!m_ready_frame_updated) {
                configASSERT(m_ready_frame != NULL);
                // Update ready frame if it has been fetched by DMA
                xvic_irq_disable(USB_IRQn);
                SWAP_FRAME_BUFS(m_ready_frame, cur);
                m_ready_frame_updated = true;
                xvic_irq_enable(USB_IRQn);
            }
#endif
            frame_mgr_decr_ref(cur);
            break;
        default:
            LOGE(__func__, "Unknown uvc stream state %d", state); 
            configASSERT(!"State error!");
            break;
    }

out:
    usb_function_release_lock();

    return ret;
}

static usb_func_acm_buf_t *usb_acm_get_buffer(struct list_head *list_hdr)
{
    usb_func_acm_buf_t *first;
    if (list_empty(list_hdr)) {
        return NULL;
    }

    first = list_first_entry(list_hdr, usb_func_acm_buf_t, list);
    list_del(&first->list);

    return first;
}

static int usb_acm_add_buffer_tail(struct list_head *list_hdr, usb_func_acm_buf_t *buf)
{
    list_add_tail(&buf->list, list_hdr);
    return 0;
}

static int usb_acm_add_buffer_head(struct list_head *list_hdr, usb_func_acm_buf_t *buf)
{
    list_add(&buf->list, list_hdr);
    return 0;
}

static int usb_bulk_acm_send_buffer(const void *buffer, int bytes, int wait_option)
{
    int sent_bytes              = 0;
    usb_func_acm_buf_t *acm_buf = NULL;
    const uint8_t *data         = NULL;
    int count                   = 0;
    int remain                  = 0;
    usb_ep_t *ep                = NULL;
    usb_request_t *req          = NULL;
    int ret                     = 0;
    usb_stream_state_t state    = STREAM_STATE_UNINITED;

    if (!usb_bulk_acm_inited()) {
        LOGE(__func__, "usb_function_acm hasn't been inited!");
        return 0;
    }

    // Wait for the initialization of ep and req
    while (1) {
        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        ep  = m_uvc_camera.ext_resp_in_ep;
        req = m_uvc_camera.ext_resp_in_req;
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();

        if ((ep != NULL) && (req != NULL)) {
            break;
        }
        if (wait_option == USB_FUNC_NO_WAIT) {
            break;
        }
        else if (wait_option == USB_FUNC_WAIT_FOREVER) {
            aiva_usleep(1);
            continue;
        }
        else {
            wait_option--;
            aiva_msleep(1);
            continue;
        }
    }

    if (ep == NULL || req == NULL) {
        // Wait timeout, No byte was sent
        return 0;
    }

    // ep and req are ready!
    data   = (const uint8_t *)buffer;
    remain = bytes;
    sent_bytes = 0;

    while (remain > 0) {
        count = (remain > DWC_REQ_SIZE) ? DWC_REQ_SIZE : remain;

        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        acm_buf = usb_acm_get_buffer(&m_acm_send_free_list);
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();
       
        if (acm_buf == NULL) {
            // No more free buffer, check DMA state
            state = usb_func_get_stream_state(req);
            UVC_DEBUG(__func__, "acm_buf is NULL, state is %d", state);
            if (state == STREAM_STATE_IDLE) {
                // DMA is idle, get one acm_buf from send list and start transfer
                usb_function_get_lock();
                xvic_irq_disable(USB_IRQn);
                acm_buf = usb_acm_get_buffer(&m_acm_send_list);
                // All buffers are in send list, it must not be NULL!!!
                configASSERT(acm_buf != NULL);
                // Swap acm_buf->buffer with req->buf
                SWAP_PTRS(acm_buf->buffer, req->buf);
                // Update dma addr and length
                req->dma = (dwc_dma_t)req->buf;
                req->length = acm_buf->count;
                // Queue the request
                ret = ep->ops->queue(ep, req, GFP_ATOMIC);
                if (ret == 0) {
                    // Clear the acm_buf offset and count
                    acm_buf->offset = 0;
                    acm_buf->count  = 0;
                    // Put it to free list
                    usb_acm_add_buffer_tail(&m_acm_send_free_list, acm_buf);
                    // Switch stream state
                    usb_func_set_stream_state(req, STREAM_STATE_BUSY);
                }
                else {
                    LOGE(__func__, "Queue req %x error! %d", req, ret);
                    SWAP_PTRS(acm_buf->buffer, req->buf);
                    usb_acm_add_buffer_head(&m_acm_send_list, acm_buf);
                }
                xvic_irq_enable(USB_IRQn);
                usb_function_release_lock();
            }
            if (wait_option == USB_FUNC_NO_WAIT) {
                break;
            }
            else if (wait_option == USB_FUNC_WAIT_FOREVER) {
                aiva_usleep(1);
                continue;
            }
            else {
                wait_option--;
                aiva_msleep(1);
                continue;
            }
        }

        configASSERT(acm_buf->buffer != NULL);
        configASSERT(acm_buf->count == 0);
        configASSERT(acm_buf->offset == 0);
        configASSERT(acm_buf->size == DWC_REQ_SIZE);

        acm_buf->offset = 0;
        acm_buf->count  = count;
        memcpy(acm_buf->buffer, data, count);

        dcache_flush(acm_buf->buffer, count);
        
        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        usb_acm_add_buffer_tail(&m_acm_send_list, acm_buf);
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();

        data   += count;
        remain -= count;
    }
    
    sent_bytes = bytes - remain;

    // Start next transfer if DMA is idle
    state = usb_func_get_stream_state(req);
    if (state == STREAM_STATE_IDLE) {
        // DMA is idle, get one acm_buf from send list and start transfer
        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        // check whether there are buffers to sent
        if (!list_empty(&m_acm_send_list)) {
            acm_buf = usb_acm_get_buffer(&m_acm_send_list);
            SWAP_PTRS(acm_buf->buffer, req->buf);
            req->dma = (dwc_dma_t)req->buf;
            req->length = acm_buf->count;
            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
            if (ret == 0) {
                acm_buf->offset = 0;
                acm_buf->count  = 0;
                usb_acm_add_buffer_tail(&m_acm_send_free_list, acm_buf);
                usb_func_set_stream_state(req, STREAM_STATE_BUSY);
            }
            else {
                LOGE(__func__, "Queue req %x error! %d", req, ret);
                SWAP_PTRS(acm_buf->buffer, req->buf);
                usb_acm_add_buffer_head(&m_acm_send_list, acm_buf);
            }
        }
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();
    }
    /*UVC_DEBUG(__func__, "sent_bytes is %d, state is %d", sent_bytes, state);*/

    return sent_bytes;
}

static int usb_bulk_acm_recv_buffer(void *buffer, int *bytes, int wait_option)
{
    int buf_len                 = 0;
    int count                   = 0;
    int remain                  = 0;
    int recv_bytes              = 0;
    usb_func_acm_buf_t *acm_buf = NULL;
    uint8_t *data               = NULL;
    usb_ep_t *ep                = NULL;
    usb_request_t *req          = NULL;
    int ret                     = 0;
    usb_stream_state_t state    = STREAM_STATE_UNINITED;

    if (!usb_bulk_acm_inited()) {
        LOGE(__func__, "usb_function_acm hasn't been inited!");
        *bytes = 0;
        return 0;
    }

    // Wait for ep and req are ready
    while (1) {
        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        ep  = m_uvc_camera.ext_cmd_out_ep;
        req = m_uvc_camera.ext_cmd_out_req;
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();

        if ((ep != NULL) && (req != NULL)) {
            break;
        }
        if (wait_option == USB_FUNC_NO_WAIT) {
            break;
        }
        else if (wait_option == USB_FUNC_WAIT_FOREVER) {
            aiva_usleep(1);
            continue;
        }
        else {
            wait_option--;
            aiva_msleep(1);
            continue;
        }
    }
    
    if (ep == NULL || req == NULL) {
        // No byte was sent
        return 0;
    }

    // ep and req are ready!
    buf_len    = *bytes;
    remain     = buf_len;
    recv_bytes = 0;
    data = buffer;
    
    while (remain > 0) {
        count = (remain > DWC_REQ_SIZE) ? DWC_REQ_SIZE : remain;

        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        acm_buf = usb_acm_get_buffer(&m_acm_recv_list);
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();

        if (acm_buf == NULL) {
            // No buf received, check stream state
            state = usb_func_get_stream_state(req);
            if (state == STREAM_STATE_IDLE) {
                // DMA is idle, start transfer
                usb_function_get_lock();
                xvic_irq_disable(USB_IRQn);
                // Get one free buffer to start a new request
                acm_buf = usb_acm_get_buffer(&m_acm_recv_free_list);
                configASSERT(acm_buf != NULL);
                // Swap the acm_buf->buffer and req->buf
                SWAP_PTRS(acm_buf->buffer, req->buf);
                // Clear offset and assign req->actual to count
                acm_buf->offset = 0;
                acm_buf->count  = req->actual;

                UVC_DEBUG(__func__, "req->actual is %d", req->actual);
                req->actual = 0;
                req->length = acm_buf->size;

                ret = ep->ops->queue(ep, req, GFP_ATOMIC);
                if (ret == 0) {
                    usb_acm_add_buffer_tail(&m_acm_recv_list, acm_buf);
                    usb_func_set_stream_state(req, STREAM_STATE_BUSY);
                }
                else {
                    LOGE(__func__, "Queue req %x error! %d", req, ret);
                    SWAP_PTRS(acm_buf->buffer, req->buf);
                    acm_buf->offset = 0;
                    acm_buf->count  = 0;
                    usb_acm_add_buffer_head(&m_acm_recv_free_list, acm_buf);
                }
                xvic_irq_enable(USB_IRQn);
                usb_function_release_lock();
            }
            // no buffer in recv list
            if (wait_option == USB_FUNC_NO_WAIT) {
                break;
            }
            else if (wait_option == USB_FUNC_WAIT_FOREVER) {
                aiva_usleep(1);
                continue;
            }
            else {
                wait_option--;
                aiva_msleep(1);
                continue;
            }
        }

        configASSERT(acm_buf->buffer != NULL);
        /*LOGW(__func__, " %d, %d, %d", acm_buf->offset, acm_buf->count, acm_buf->size);*/
        configASSERT((acm_buf->offset + acm_buf->count) <= acm_buf->size);
        dcache_inval(acm_buf->buffer, acm_buf->size);

        if (count >= acm_buf->count) {
            // all bytes in acm_buf will be copied
            count = acm_buf->count;
            memcpy(data, &acm_buf->buffer[acm_buf->offset], count);

            // reset count and offset of acm_buf
            acm_buf->count  = 0;
            acm_buf->offset = 0;

            // return acm_buf to free list
            usb_function_get_lock();
            xvic_irq_disable(USB_IRQn);
            usb_acm_add_buffer_tail(&m_acm_recv_free_list, acm_buf);
            xvic_irq_enable(USB_IRQn);
            usb_function_release_lock();
        }
        else {
            // count is smaller than acm_buf->count, part of acm_buf will be copied
            memcpy(data, &acm_buf->buffer[acm_buf->offset], count);
            
            // adjust the acm_buf->offset and add the acm_buf into recv list for next call
            acm_buf->count  -= count;
            acm_buf->offset += count;

            usb_function_get_lock();
            xvic_irq_disable(USB_IRQn);
            usb_acm_add_buffer_head(&m_acm_recv_list, acm_buf);
            xvic_irq_enable(USB_IRQn);
            usb_function_release_lock();
        }
        data   += count;
        remain -= count;
    }
    
    recv_bytes = buf_len - remain;
    *bytes = recv_bytes;

    // Check state again
    state = usb_func_get_stream_state(req);
    if (state == STREAM_STATE_IDLE) {
        // DMA is idle, start next transfer
        usb_function_get_lock();
        xvic_irq_disable(USB_IRQn);
        if (!list_empty(&m_acm_recv_free_list)) {
            acm_buf = usb_acm_get_buffer(&m_acm_recv_free_list);
            // Swap the acm_buf->buffer and req->buf
            SWAP_PTRS(acm_buf->buffer, req->buf);
            // Clear offset and assign req->actual to count
            acm_buf->offset = 0;
            acm_buf->count  = req->actual;

            UVC_DEBUG(__func__, "Check IDLE: req->actual is %d", req->actual);
            req->actual = 0;
            req->length = acm_buf->size;

            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
            if (ret == 0) {
                usb_acm_add_buffer_tail(&m_acm_recv_list, acm_buf);
                usb_func_set_stream_state(req, STREAM_STATE_BUSY);
            }
            else {
                LOGE(__func__, "Queue req %x error! %d", req, ret);
                SWAP_PTRS(acm_buf->buffer, req->buf);
                acm_buf->offset = 0;
                acm_buf->count  = 0;
                usb_acm_add_buffer_head(&m_acm_recv_free_list, acm_buf);
            }
        } 
        xvic_irq_enable(USB_IRQn);
        usb_function_release_lock();
    }

    /*UVC_DEBUG(__func__, "recv_bytes is %d, state is %d", recv_bytes, state);*/
    
    return recv_bytes;
}

static void uvc_bulk_register_user_start_cb(uvc_user_start_func_t func)
{
    _user_start_stream_cb = func;
}

static void uvc_bulk_register_user_stop_cb(uvc_user_stop_func_t func)
{
    _user_stop_stream_cb = func;
}

static int  uvc_bulk_init(uvc_param_t *param)
{
    configASSERT(param != NULL);

    LOGD(__func__, "set max_payload_size as %d", param->max_payload_size);
    VIDEO_REQ_SIZE = param->max_payload_size;
    EP0_DESC_SEL   = param->ep0_desc_sel;

    return 0;
}


/* Export Dev: _id,                 _name,              _flag,  _driver,        	   _priv */
UDEVICE_EXPORT(UCLASS_USB_GARGET,   uvc_bulk_gadget,    0,      &m_uvc_bulk_gadget,    NULL);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
