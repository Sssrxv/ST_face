#ifndef __USB_DEBUG_H__
#define __USB_DEBUG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dump_setup_requests(uint8_t bReqType, uint8_t bRequest, 
        uint16_t wValue, uint16_t wIndex, uint16_t wLength);

#ifdef __cplusplus
}
#endif

#endif // __USB_DEBUG_H__
