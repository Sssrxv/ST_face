#ifndef _ISP_CONFIG_H_
#define _ISP_CONFIG_H_

#include <stdint.h>
#include "isp_context.h"
#include "isp.h"

//#define CONFIG_ISP_INTERRUPT (0)
#define CONFIG_ISP_INTERRUPT (1)


#ifdef __cplusplus
extern "C" {
#endif

int isp_config_reset_all(isp_context_t *context);
int isp_config_core(isp_context_t *context, const isp_config_param_t *config_param);
int isp_config_rdma(isp_context_t *context, const isp_frame_t *bayer_frame);
int isp_config_out_frame2(isp_context_t *context, const isp_frame_t *out_frame2);
int isp_config_out_frame1(isp_context_t *context, const isp_frame_t *out_frame1);
int isp_config_out_frame0(isp_context_t *context, const isp_frame_t *out_frame0, int hflip);
int isp_config_out_frames(isp_context_t *context, int frame_num, const isp_frame_t *out_frame, int hflip);
int isp_config_start(isp_context_t *context);
int isp_config_wait_finish(isp_context_t *context, const isp_process_param_t *process_param);
int isp_config_clear_interrupt(isp_context_t *context);
int isp_config_irq_handler(isp_context_t *context);


#ifdef __cplusplus
}
#endif


#endif
