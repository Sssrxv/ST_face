#ifndef __VTE_CTRL_H__
#define __VTE_CTRL_H__

#include <stdint.h>
#include "frame_mgr.h"

//#include "mi_rd.h"
//#include "mi_wr.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _vte_rsz_param_t {
    uint32_t src_addr;          // gray image src addr
    uint32_t dst_addr;          // gray image dst addr
    int32_t  input_w;           // input width, aligned with 8
    int32_t  input_h;           // input height
    int32_t  output_w;          // output width, aligned with 8
    int32_t  output_h;          // output height
} __attribute__((packed, aligned(4))) vte_rsz_param_t;


typedef struct _vte_csf_param_t {
    // K00, K01, K10, K11, K12, K22
    uint8_t K[6];
} __attribute__((packed, aligned(4))) vte_csf_param_t;

typedef struct _vte_ffe_param_t {
    uint32_t nms_en;
    uint32_t pix_thd;
    uint32_t max_level;
    uint32_t max_num;
    uint32_t dst_addr;
} __attribute__((packed, aligned(4))) vte_ffe_param_t;


typedef struct _fast_pt_hdr_t {
    uint16_t score;
    uint16_t y;
    uint16_t x;
    struct {
        uint16_t layer : 4;
        uint16_t num : 12;
    }ln;
} __attribute__((packed)) fast_pt_hdr_t;

typedef struct _vte_param_t {
    uint32_t max_level;
    float    scale;
    uint16_t img_w;
    uint16_t img_h;
    uint32_t max_pt_num;
} __attribute__((packed, aligned(4))) vte_fast_param_t;




void vte_set_mux(uint32_t mux_ctrl);
void vte_mux_rsz_rd_enable(void);
void vte_mux_rsz_rd_disable(void);
void vte_mux_csf_rd_enable(void);
void vte_mux_csf_rd_disable(void);
void vte_mux_csf_wr_enable(void);
void vte_mux_csf_wr_disable(void);
void vte_mux_ffe_wr_enable(void);
void vte_mux_ffe_wr_disable(void);
void vte_rsz_enable(void);
void vte_rsz_disable(void);
void vte_rsz_set_scale(uint32_t h_scale, uint32_t v_scale);
void vte_csf_enable(void);
void vte_csf_disable(void);
void vte_csf_set_filter_kernel(uint8_t *k, int cnt);
void vte_ffe_nms_enable(void);
void vte_ffe_nms_disable(void);
void vte_ffe_set_pix_thd(uint32_t thd);
void vte_ffe_set_cropv(uint32_t vu, uint32_t vd);
void vte_ffe_set_croph(uint32_t hl, uint32_t hr);
void vte_fpp_set_level_maxnum(uint32_t level, uint32_t maxnum);
uint32_t vte_fpp_get_act_num(void);
void vte_fpp_clear_act_num(void);
void vte_mird_cfg(uint32_t wi, uint32_t hi, uint32_t addr);
void vte_mird_start(void);
int vte_mird_complete(void);
void vte_miwr_cfg(int num, uint32_t fmt, int x_stride, int y_stride, uint32_t addr);
int vte_miwr_complete(int num);
void vte_rsz_init(vte_rsz_param_t *param);
void vte_rsz_start(void);
int vte_rsz_complete(void);
void vte_csf_init(vte_csf_param_t *param);
void vte_ffe_update_crop_vh(uint32_t wi, uint32_t hi);
void vte_ffe_init(vte_ffe_param_t *param, uint32_t wi, uint32_t hi);
int vte_ffe_complete(void);
uint32_t vte_fpp_get_act_num(void);
void vte_fpp_clr_act_num(void);
void vte_mird_clear_interp_status();
void vte_dump_fast_point(volatile fast_pt_hdr_t *ptr);

int vte_pyr_fast_pt_init(vte_fast_param_t *param);
int vte_pyr_fast_pt_calc(vte_fast_param_t *param, void *src_addr, fast_pt_hdr_t *ptr, uint32_t *act_num);
int vte_pyr_fast_pt_release(void);

typedef int (*vte_irq_cb_t)(void *ctx);
int vte_irq_register(int irq, uint32_t priority, vte_irq_cb_t cb, void *ctx);
int vte_irq_unregister(int irq);
void vte_clear_interp_status(int irq);

int vte_ctrl_get_pyr_frames(frame_buf_t **pyr_frames, int max_level);

#ifdef __cplusplus
}
#endif

#endif //__VTE_CTRL_H__
