#ifndef __NO_OS_DEFS_H__
#define __NO_OS_DEFS_H__

#include "xvic.h"
#include "xvic_private.h"
#include "xlist.h"
#include "aiva_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains OS-specific includes and definitions.
 *
 */

#undef linux
#undef __linux
#undef __linux__

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>	// for memcpy

#define USB_EP0_BUF_SIZE    (64 * 1024)

/** @{ */
/** Data types needed by the PCD */
typedef uint64_t	u64, u_int64_t;
typedef uint32_t	u32, u_int32_t;
typedef uint16_t	u16, u_int16_t;
typedef uint8_t		u8, u_int8_t;

typedef int64_t		s64;
typedef int32_t		s32;
typedef int16_t		s16;
typedef int8_t		s8;

typedef unsigned long	u_long;
typedef unsigned int	u_int;
typedef unsigned short	u_short;
typedef unsigned char	u_char;

typedef int gfp_t;

/** @} */

/** @{ */
/** Data type for DMA addresses */
typedef unsigned long		    dwc_dma_t;
#define USB_DMA_ADDR_INVALID	(~(dwc_dma_t)0)
/** @} */

/** Compiler 'aligned(16)' attribute */
#define UALIGNED16	__attribute__ ((aligned(16)))
#define UALIGNED32	__attribute__ ((aligned(32)))

#define interrupt_disable()	    xvic_irq_disable(USB_IRQn)
#define interrupt_enable()	    xvic_irq_enable(USB_IRQn)

// from /usr/include/asm-x86_64/errno.h
#ifndef EIO
#define EIO		    5	/* I/O error */
#endif
#ifndef EAGAIN
#define EAGAIN		11	/* Try again */
#endif
#ifndef ENOMEM
#define ENOMEM		12	/* Out of memory */
#endif
#ifndef EBUSY
#define EBUSY		16	/* Device or resource busy */
#endif
#ifndef ENODEV
#define ENODEV		19	/* No such device */
#endif
#ifndef EINVAL
#define EINVAL		22	/* Invalid argument */
#endif
#ifndef ENOSPC
#define ENOSPC		28	/* No space left on device */
#endif
#ifndef EPIPE
#define EPIPE		32	/* Broken pipe */
#endif
#ifndef EDOM
#define EDOM		33	/* Math argument out of domain of func */
#endif
#ifndef ERANGE
#define ERANGE      34
#endif
#ifndef ENODATA
#define ENODATA		61	/* No data available */
#endif
#ifndef ENOSR
#define ENOSR		63	/* Out of streams resources */
#endif
#ifndef ECOMM
#define ECOMM		70	/* Communication error on send */
#endif
#ifndef EPROTO
#define EPROTO		71	/* Protocol error */
#endif
#ifndef EOVERFLOW
#define EOVERFLOW	75	/* Value too large for defined data type */
#endif
#ifndef ERESTART
#define ERESTART	85	/* Interrupted system call should be restarted */
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP	95	/* Operation not supported on transport endpoint */
#endif
#ifndef ECONNABORTED
#define ECONNABORTED	103	/* Software caused connection abort */
#endif
#ifndef ECONNRESET
#define ECONNRESET	104	/* Connection reset by peer */
#endif
#ifndef ESHUTDOWN
#define ESHUTDOWN	108	/* Cannot send after transport endpoint shutdown */
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT	110	/* Connection timed out */
#endif
#ifndef EINPROGRESS
#define EINPROGRESS	115	/* Operation now in progress */
#endif
#ifndef ENOTSUPP
#define ENOTSUPP    524 /* Operation is not supported */
#endif

#define USB_REQ_ISO_ASAP        (0x0002)

#include "usb.h"

struct usb_ep_ops;
struct usb_gadget_ops;

/**
 * Platform-specific USB endpoint
 */
typedef struct usb_ep {
	const void	*desc;
	const void	*comp_desc;
	unsigned	maxpacket:16;
	u8		    address;
    const char  *name;
    const struct usb_ep_ops *ops;
    struct list_head ep_list;
} usb_ep_t;

/**
 * Platform-specific USB request
 */
typedef struct usb_request {
	void		  *buf;
	unsigned	  length;
	unsigned long dma;

	unsigned	  stream_id:16;
	unsigned	  zero:1;

	void		  (*complete)(usb_ep_t *ep, struct usb_request *req);

	int		      status;
	unsigned	  actual;
    void          *context;
} usb_request_t;

struct usb_gadget_iso_packet_descriptor {
    unsigned int offset;
    unsigned int length; /* expected length */
    unsigned int actual_length;
    unsigned int status;
};

struct usb_iso_request {
    void *buf0;
    void *buf1;
    dwc_dma_t dma0;
    dwc_dma_t dma1;
    uint32_t buf_proc_intrvl;

    unsigned no_interrupt:1;
    unsigned zero:1;
    unsigned short_not_ok:1;

    uint32_t sync_frame;
    uint32_t data_per_frame;
    uint32_t data_pattern_frame;
    uint32_t start_frame;
    uint32_t flags;

    void (*process_buffer)(struct usb_ep*, struct usb_iso_request *);

    void *context;
    int status;

    struct usb_gadget_iso_packet_descriptor *iso_packet_desc0;
    struct usb_gadget_iso_packet_descriptor *iso_packet_desc1;
};

/**
 *   struct usb_otg_caps - describes the otg capabilities of the device
 *   @otg_rev: The OTG revision number the device is compliant with, it's
 *       in binary-coded decimal (i.e. 2.0 is 0200H).
 *   @hnp_support: Indicates if the device supports HNP.
 *   @srp_support: Indicates if the device supports SRP.
 *   @adp_support: Indicates if the device supports ADP.
 *  
 */
struct usb_otg_caps {
    uint16_t otg_rev;
    uint8_t  hnp_support;
    uint8_t  srp_support;
    uint8_t  adp_support;
};

/*-------------------------------------------------------------------------*/

/* endpoint-specific parts of the api to the usb controller hardware.
 * unlike the urb model, (de)multiplexing layers are not required.
 * (so this api could slash overhead if used on the host side...)
 * 
 * note that device side usb controllers commonly differ in how many
 * endpoints they support, as well as their capabilities.
 */
struct usb_ep_ops {
    int (*enable)  (struct usb_ep *ep, const usb_endpoint_descriptor_t *desc);
    int (*disable) (struct usb_ep *ep);

    struct usb_request *(*alloc_request) (struct usb_ep *ep, gfp_t gfp_flags);
    void (*free_request) (struct usb_ep *ep, struct usb_request *req);

    int (*queue)     (struct usb_ep *ep, struct usb_request *req, gfp_t gfp_flags);
    int (*dequeue)   (struct usb_ep *ep, struct usb_request *req);

    int (*set_halt)  (struct usb_ep *ep, int value);
    int (*set_wedge) (struct usb_ep *ep);

    int  (*fifo_status) (struct usb_ep *ep);
    void (*fifo_flush)  (struct usb_ep *ep);
};

struct usb_isoc_ep_ops {
    struct usb_ep_ops ep_ops;
    int (*iso_ep_start)(struct usb_ep*, struct usb_iso_request*, int);
    int (*iso_ep_stop)(struct usb_ep*, struct usb_iso_request*);
    struct usb_iso_request* (*alloc_iso_request)(struct usb_ep *ep, int packets, int gfp_flags);
    void (*free_iso_request)(struct usb_ep *ep, struct usb_iso_request *req);
};

struct usb_gadget {
  //struct work_struct work;
  //struct usb_udc * udc;
  const struct usb_gadget_ops *ops;
  struct usb_ep *ep0;
  struct usb_request *ep0_req;
  struct list_head ep_list;
  int speed;
  int max_speed;
  int state;
  const char *name;
  //struct device dev;
  unsigned out_epnum;
  unsigned in_epnum;
  unsigned mA;
  struct usb_otg_caps * otg_caps;
  unsigned sg_supported:1;
  unsigned is_otg:1;
  unsigned is_a_peripheral:1;
  unsigned b_hnp_enable:1;
  unsigned a_hnp_support:1;
  unsigned a_alt_hnp_support:1;
  unsigned hnp_polling_support:1;
  unsigned host_request_flag:1;
  unsigned quirk_ep_out_aligned_size:1;
  unsigned quirk_avoids_skb_reserve:1;
  unsigned is_selfpowered:1;
  unsigned deactivated:1;
  unsigned connected:1;
  uint32_t max_payload_size;            /** max payload size */
  void    *priv;                        /** private pointer */
};  

struct usb_gadget_ops {
    int (*get_frame)(struct usb_gadget *);
    int (*wakeup)   (struct usb_gadget *);
};



struct usb_gadget_driver {
  char *function;
  int  max_speed;
  int  (*bind)     (struct usb_gadget *gadget,struct usb_gadget_driver *driver);
  void (*unbind)   (struct usb_gadget *);
  int  (*setup)    (struct usb_gadget *,const usb_device_request_t *);
  void (*connect)  (struct usb_gadget *);
  void (*disconnect) (struct usb_gadget *);
  void (*suspend)  (struct usb_gadget *);
  void (*resume)   (struct usb_gadget *);
  void (*reset)    (struct usb_gadget *);
  void *dev_ops;
  //struct device_driver driver;
  //char *udc_name;
  //struct list_head pending;
  //unsigned match_existing_only:1;
};

/*
#define container_of(ptr, type, member) ({\
    const typeof( ((type *)0)->member ) *__mptr = (ptr);\
    (type *)( (char *)__mptr - offsetof(type,member) );})
*/

#define phys_to_virt(addr)          (void *)(addr)

#define GFP_ATOMIC                  (1)

struct usb_ep *gadget_find_eq_by_name(struct usb_gadget *g, const char *name);

#ifdef __cplusplus
}
#endif

#endif
