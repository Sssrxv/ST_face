#include "no_os_defs.h"
#include "usb_sink_lpbk.h"
#include "usb_sink_lpbk_ep0.h"
#include "syslog.h"
#include "aiva_utils.h"
#include "aiva_malloc.h"
#include "udevice.h"

#ifdef TEST_ISOC
#define USB_REQ_SIZ    (512)
#else
#define USB_REQ_SIZ    (512 * 1024 - 32)
#endif

/** Data packet buffer used to return data for GET_STATUS and
 *  GET_DESCRIPTOR requests, up to USB_EP0_BUF_SIZE bytes in length
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

static struct usb_gadget_driver m_lpbk_gadget = {
    .function  = "usb_sink_lpbk",
    .max_speed = USB_SPEED_HIGH,
    .bind      = bind,
    .unbind    = unbind,
    .setup     = setup,
    .connect   = NULL,
    .disconnect= disconnect,
    .suspend   = suspend,
    .resume    = resume,
    .reset     = reset,
};

static f_loopback_t m_loopback = {
    .driver = &m_lpbk_gadget,
    .ifc    = 0,
    .cfg    = 0,
    .sink   = 1,
    .ep_in      = NULL,
    .ep_in_buf  = NULL,
    .ep_in_req  = NULL,
    .ep_out     = NULL,
    .ep_out_buf = NULL,
    .ep_out_req = NULL,
};

static int set_interface(struct usb_gadget *gadget, f_loopback_t *lpbk, int alt)
{
    int ret = 0;
    usb_request_t *req;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);

    if (alt == -1) {
        goto cleanup;
    }

    /* Already set? */
    if (lpbk->ep_out) {
        LOGW (__func__, "lpbk->ep_out is not NULL, already set!");
        return 0;
    }

    ret = usb_sink_lpbk_enable_eps(gadget, lpbk);
    if (ret < 0) {
        return ret;
    }

    ret = -ENOMEM;

    // TODO: allocate request and queue request
    req = lpbk->ep_in->ops->alloc_request(lpbk->ep_in, GFP_ATOMIC);
    if (req == NULL) {
        goto cleanup;
    }
    configASSERT(lpbk->ep_in_buf != NULL);
    req->buf = lpbk->ep_in_buf;
    req->dma = (dwc_dma_t)req->buf;
    req->length = USB_REQ_SIZ;
    req->zero = 0;
    req->complete = complete;
    lpbk->ep_in_req = req;

    ret = lpbk->ep_in->ops->queue(lpbk->ep_in, req, GFP_ATOMIC);
    if (ret < 0) {
        goto cleanup;
    }

    req = lpbk->ep_out->ops->alloc_request(lpbk->ep_out, GFP_ATOMIC);
    configASSERT(lpbk->ep_out_buf != NULL);
    req->buf = lpbk->ep_out_buf;
    req->dma = (dwc_dma_t)req->buf;
    req->length = USB_REQ_SIZ;
    req->zero = 0;
    req->complete = complete;
    lpbk->ep_out_req = req;

    ret = lpbk->ep_out->ops->queue(lpbk->ep_out, req, GFP_ATOMIC);
    if (ret < 0) {
        goto cleanup;
    }

    return 0;
cleanup:
    /* disable_eps() will eventually dequeue all requests queued on each EP,
     * and call the ->complete routine with -USB_E_SHUTDOWN status for each
     * one. That in turn will free the request. So all cleanup is done for
     * us by this one call.
     */
    if (lpbk->ep_in_req != NULL) {
        lpbk->ep_in->ops->free_request(lpbk->ep_in, lpbk->ep_in_req);
        lpbk->ep_in_req = NULL;
    }
    if (lpbk->ep_out_req != NULL) {
        lpbk->ep_out->ops->free_request(lpbk->ep_out, lpbk->ep_out_req);
        lpbk->ep_out_req = NULL;
    }
    usb_sink_lpbk_disable_eps(gadget, lpbk);
    return -1;
}

static int get_interface(struct usb_gadget *gadget, f_loopback_t *lpbk)
{
    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    u8 *buf = req->buf;
    configASSERT(buf != NULL);

    buf[0] = lpbk->ifc;
    req->length = 1;

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

static int get_config(struct usb_gadget *gadget, f_loopback_t *lpbk)
{
    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    u8 *buf = req->buf;
    configASSERT(buf != NULL);

    buf[0] = lpbk->cfg;
    req->length = 1;

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

static int g_send_idx = 0;

/**
 * Function Driver transfer complete callback routine
 */
static void complete(usb_ep_t *ep, usb_request_t *req)
{
    int ret;

    /*LOGI(__func__, "in..., req->status is %d", req->status);*/
    switch (req->status) {
        case 0:
            if (m_loopback.sink) {
                if (ep == m_loopback.ep_out) {
                    /*LOGD(__func__, "ep == out");*/
                    dcache_inval(req->buf, req->actual);
                    /*LOGD(__func__, "%s", req->buf);*/
                    req->length = USB_REQ_SIZ;
                    ret = m_loopback.ep_out->ops->queue(m_loopback.ep_out, req, GFP_ATOMIC);
                    if (ret < 0) {
                        LOGE(__func__, "ep_out->ops->queue error!");
                    }
                }
               else {
                    /*LOGD(__func__, "ep == in");*/
                    aiva_sprintf(req->buf, "g_send_idx is %d\r\n", g_send_idx++);
                    dcache_flush(req->buf, USB_REQ_SIZ);
                    req->length = USB_REQ_SIZ;
                    ret = m_loopback.ep_in->ops->queue(m_loopback.ep_in, req, GFP_ATOMIC);
                    if (ret < 0) {
                        LOGE(__func__, "ep_out->ops->queue error!");
                    }
                }
            }
            break;
        default:
            req->length = USB_REQ_SIZ;
            ret = ep->ops->queue(ep, req, GFP_ATOMIC);
            if (ret == 0) {
                return ;
            }
            LOGE(__func__, "Failed ot queue OUT req!");
        //case -USB_E_SHUTDOWN:
        case -ESHUTDOWN:
            LOGE(__func__, "Shutdown status!");
            ep->ops->free_request(ep, req);
            if (ep == m_loopback.ep_in) {
                m_loopback.ep_in_req = NULL;
            }
            if (ep == m_loopback.ep_out) {
                m_loopback.ep_out_req = NULL;
            }
            break;
    }
}

static int bind(struct usb_gadget *gadget, struct usb_gadget_driver *driver)
{
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);

    gadget->ep0_req = ep0->ops->alloc_request(ep0, GFP_ATOMIC);
    configASSERT(gadget->ep0_req != NULL);

    gadget->ep0_req->buf = &g_ep0_status_buf[0];

    m_loopback.ep_in_buf = aiva_malloc(USB_REQ_SIZ);
    configASSERT(m_loopback.ep_in_buf != NULL);
    memset(m_loopback.ep_in_buf, 0, USB_REQ_SIZ);
    dcache_flush(m_loopback.ep_in_buf, USB_REQ_SIZ);

    m_loopback.ep_out_buf = aiva_malloc(USB_REQ_SIZ);
    configASSERT(m_loopback.ep_out_buf != NULL);
    memset(m_loopback.ep_out_buf, 0, USB_REQ_SIZ);
    dcache_flush(m_loopback.ep_out_buf, USB_REQ_SIZ);

    return 0;
}

static void unbind(struct usb_gadget *gadget)
{
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
    configASSERT(gadget->ep0_req != NULL);

    ep0->ops->free_request(ep0, gadget->ep0_req);

    if (m_loopback.ep_in_buf != NULL) {
        aiva_free(m_loopback.ep_in_buf);
        m_loopback.ep_in_buf = NULL;
    }

    if (m_loopback.ep_out_buf != NULL) {
        aiva_free(m_loopback.ep_out_buf);
        m_loopback.ep_out_buf = NULL;
    }
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

    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}

static int dwc_usb_ep0_recv_data(struct usb_gadget *gadget, void *buf, int bytes)
{
    int ret = 0;
    struct usb_ep *ep0 = gadget->ep0;
    configASSERT(ep0 != NULL);
   
    struct usb_request *req = gadget->ep0_req;
    configASSERT(req != NULL);
    
    req->length = bytes;
    ret = ep0->ops->queue(ep0, req, GFP_ATOMIC);

    return ret;
}


static int setup(struct usb_gadget *gadget, const usb_device_request_t *ctrl)
{
    u16    windex  = UGETW(ctrl->wIndex);
    u16    wvalue  = UGETW(ctrl->wValue);
    u16    wlength = UGETW(ctrl->wLength);
    int    ret = 0;
    
    ret = 0;
    switch (ctrl->bRequest) {
        case UR_GET_DESCRIPTOR:
            usb_sink_lpbk_get_desc(gadget, ctrl);
            break;
        case UR_SET_FEATURE:
            LOGD(__func__, "USB_REQ_SET_FEATURE");
            // do nothing
            break;
        case UR_SET_INTERFACE:
            LOGD(__func__, "USB_REQ_SET_INTERFACE\n");
            if (!m_loopback.cfg || windex){
                LOGE(__func__, "cfg = %x, wIndex = %x", m_loopback.cfg, windex);
                return -ENOTSUPP;
            }
            LOGI(__func__, "Set interface %x", windex);
            if (m_loopback.ifc != wvalue) {
                set_interface(gadget, &m_loopback, -1);
                ret = set_interface(gadget, &m_loopback, wvalue);
                if (ret < 0) {
                    return ret;
                }
            }
            m_loopback.ifc = wvalue;
            break;
        case UR_GET_INTERFACE:
            LOGD(__func__, "USB_REQ_GET_INTERFACE\n");
            if (!m_loopback.cfg) {
                LOGE(__func__, "cfg = %x", m_loopback.cfg);
                return -ENOTSUPP;
            }
            if (windex) {
                LOGE(__func__, "windex = %x", windex);
                return -ENOTSUPP;
            }
            // send the current m_loopback.ifc back
            ret = get_interface(gadget, &m_loopback);
            break;
        case UR_SET_CONFIG:
            LOGI(__func__, "UR_SET_CONFIG, cfg = %d, wlength is %d", wvalue, wlength);
            if (wvalue != 0 && wvalue != 1) {
                LOGE(__func__, "wvalue = %x", wvalue);
                return -ENOTSUPP;
            }

            /* If config already set, clear it and disable the EPs */
            if (m_loopback.cfg) {
                m_loopback.cfg = 0;
                m_loopback.ifc = 0;
                // set interface -1
                LOGW(__func__, "config already set, clear it and disable EPs");
                set_interface(gadget, &m_loopback, -1);
            }
        
            /* If new config is 1, enable the EPs for interface 0 */
            if (wvalue) {
                m_loopback.cfg = wvalue;
                m_loopback.ifc = 0;
                // set interface 0
                LOGI(__func__, "cfg is %d, set ifc to 0", wvalue);
                ret = set_interface(gadget, &m_loopback, 0);
                dwc_usb_ep0_recv_data(gadget, &m_loopback.cfg, wlength);
            }
            break;
        case UR_GET_CONFIG:
            LOGD(__func__, "USB_REQ_GET_CONFIG");
            // send m_loopback.cfg back
            ret = get_config(gadget, &m_loopback);
            break;
        case UR_SYNCH_FRAME:
            LOGD(__func__, "USB_REQ_SYNCH_FRAME\n");
            // do nothing
            break;
        default:
            LOGE(__func__, "Unknown request 0x%x", ctrl->bRequest);
            break;
    }

    return ret;
}

static void disconnect(struct usb_gadget *gadget)
{
    LOGI(__func__, "in...");
    LOGI(__func__, "out...");
}

static void suspend(struct usb_gadget *gadget)
{
    LOGI(__func__, "in...");
    LOGI(__func__, "out...");
}

static void resume(struct usb_gadget *gadget)
{
    LOGI(__func__, "in...");
    LOGI(__func__, "out...");
}

static void reset(struct usb_gadget *gadget)
{
    LOGI(__func__, "in...");
    LOGI(__func__, "out...");
}

struct usb_gadget_driver *usb_func_loopback(void)
{
    return &m_lpbk_gadget;
}

/* Export Dev: _id,                 _name,          _flag,  _driver,      	   _priv */
UDEVICE_EXPORT(UCLASS_USB_GARGET,   lpbk_gadget,    0,      &m_lpbk_gadget,    NULL);
