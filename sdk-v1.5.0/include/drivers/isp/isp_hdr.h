#ifndef _ISP_HDR_H_
#define _ISP_HDR_H_

#include <stdint.h>
#include "isp.h"
#include "isp_api.h"
#include "libsync_obj.h"

#define CONFIG_HDR_INTERRUPT (0)

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _isp_hdr_process_param_t
{
    isp_frame_t *ldr_frames[2];
    isp_frame_t *hdr_frame;
} isp_hdr_process_param_t;


typedef struct _isp_hdr_context_t
{
    isp_hdr_config_param_t  config_param;
    sync_obj_t              *hdr_intr_sync;
    uint32_t                wait_interrupt_events;
    volatile uint32_t       finished_interrupt_events;
} isp_hdr_context_t;


int isp_hdr_init(isp_hdr_context_t *context, const isp_hdr_config_param_t *config_param);
int isp_hdr_config_strength(isp_hdr_context_t *context, int strength);
int isp_hdr_config_lut(isp_hdr_context_t *context, const isp_hdr_lut_t *lut);
int isp_hdr_process_frame(isp_hdr_context_t *context, const isp_hdr_process_param_t *process_param);
int isp_hdr_release(isp_hdr_context_t *context);


#ifdef __cplusplus
}
#endif


#endif
