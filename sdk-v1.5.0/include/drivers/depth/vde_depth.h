#ifndef __VDE_DEPTH_H__
#define __VDE_DEPTH_H__

#include <stdint.h>

#include "mi_rd.h"
#include "mi_wr.h"
#include "rectify.h"
#include "rectify_ctl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _vde_param_t {
    vde_rec_param_t recl;
    vde_rec_param_t recr;
    uint32_t cvb_ctrl;              // cost volume build
    uint32_t sgm_ctrl;
    uint32_t wta_ctrl;
    uint32_t lrc_ctrl;
    uint32_t dpp_ctrl;              // post processing
    uint32_t dpp_th;
    uint32_t lrc_dep_addr;          // lrc-depth image address
    uint32_t dst_addr;              // mi_wr address
    uint32_t dst_x_stride;
    uint32_t dpp_roi_x;
    uint32_t dpp_roi_y;
    uint32_t dpp_roi_w;
    uint32_t dpp_roi_h;
    uint32_t dtd_ctrl;
    uint32_t saf_ctrl;              // for structure light
    uint32_t saf_sp;
} __attribute__((packed, aligned(4))) vde_param_t;

/* ORB Feature Extraction */

/* cvb_ctrl */
#define VDE_CVB_DESC_F(x)               ((x) & 0xF)
#define VDE_CVB_GRAY_F(x)               (((x) & 0xF) << 8)
#define VDE_CVB_H_OFFS(x)               (((x) & 0xFF) << 16)

/* sgm_ctrl */
#define VDE_SGM_P1(x)                   ((x) & 0x1FF)
#define VDE_SGM_P2(x)                   (((x) & 0x1FF) << 16)

/* wta_ctrl */
#define VDE_WTA_SIZE(x)                 ((x) & 0x7)

/* lrc_ctrl */
#define VDE_LRC_ENABLE                  (1)
#define VDE_LRC_DISABLE                 (0)
#define VDE_LRC_H_OFFS(x)               (((x) & 0xFF) << 8)
#define VDE_LRC_TH_CONF(x)              (((x) & 0xF) << 16)
#define VDE_LRC_TH_CONF_AND             (1 << 24)
#define VDE_LRC_TH_CONF_OR              (0 << 24)

/* lrc_th_d_0~15 */
#define VDE_LRC_TH_D_LO(x)              ((x) & 0x7FF)
#define VDE_LRC_TH_D_HI(x)              (((x) & 0x7FF) << 16)

/* dpp_ctrl */
#define VDE_DPP_ENABLE                  (1)
#define VDE_DPP_DISABLE                 (0)
#define VDE_DPP_MEDIUM_DIS              (0 << 4)
#define VDE_DPP_MEDIUM_3X3              (1 << 4)
#define VDE_DPP_MEDIUM_5X5              (2 << 4)

/* dpp_th */
#define VDE_DPP_TH_MIN(x)               ((x) & 0x7FF)
#define VDE_DPP_TH_MAX(x)               (((x) & 0x7FF) << 16)

/* dpp_start */
#define VDE_DPP_HSTART(x)               ((x) & 0x3FF)
#define VDE_DPP_VSTART(x)               (((x) & 0x7FF) << 16)

/* dpp_stop */
#define VDE_DPP_HSTOP(x)                ((x) & 0x3FF)
#define VDE_DPP_VSTOP(x)                (((x) & 0x7FF) << 16)

/* mux_ctrl */
// [1:0]
#define MUX_REC0_OUT_DISABLE            (0x0)           // disable all output
#define MUX_REC0_OUT_DEPTH              (0x1)           // output depth image
// [2]
#define MUX_REC0_OUT_RECTIFY            (0x2)           // output rectify image
#define MUX_REC0_OUT_ALL                (0x3)           // output all
// [5:4]
#define MUX_REC1_OUT_DISABLE            (0x0 << 4)      // disable all output
#define MUX_REC1_OUT_DEPTH              (0x1 << 4)      // output depth image
// [6]
#define MUX_REC1_OUT_RECTIFY            (0x2 << 4)      // output rectify image
#define MUX_REC1_OUT_ALL                (0x3 << 4)      // output all
// [8]
#define MUX_DEP_LR_DPP_DIS              (1 << 8)        // wtf!!! disable lr-check and post-proc

/* dtd_ctrl */
#define VDE_DTD_CTRL_EN                 (1)
#define VDE_DTD_CTRL_DIS                (0)

/* dtd_lut */
#define VDE_DTD_LUT_MAX_SIZE            (2 * 1024)
#define VDE_DTD_LUT_WR_EN               (1)
#define VDE_DTD_LUT_WR_DIS              (0)
#define VDE_DTD_LUT_WR_ADDR(a)          (((a) & 0x7ff)  << 4)
#define VDE_DTD_LUT_WR_DATA(d)          (((d) & 0xffff) << 16)

/* saf_ctrl */
#define VDE_SAF_EN                      (1)
#define VDE_SAF_DIS                     (0)
#define VDE_SAF_MODE_0                  (0 << 4)        // similar to dpp
#define VDE_SAF_MODE_1                  (1 << 4)
#define VDE_SAF_MODE_MASK               (1 << 4)
#define VDE_SAF_K_MASK(k)               (((k) & 0xff) << 8)
#define VDE_SAF_K_SIZE(s)               (((s) & 0xff) << 16)
#define VDE_SAF_LAMDA(l)                (((l) & 0xff) << 24)

/* saf_sp */
#define VDE_SAF_SP_EN                   (1)
#define VDE_SAF_SP_DIS                  (0)
#define VDE_SAF_SP_REF(r)               (((r) & 0xff) << 8)

/* ORB feature idx */
typedef struct _vde_ofe_idx_t {
    volatile uint32_t idx[64];
} __attribute__((packed, aligned(4))) ofe_idx_t;

/* lrc theshhold */
typedef struct _lrc_th_t {
    volatile uint32_t d[8];
} __attribute__((packed, aligned(4))) lrc_th_t;

typedef struct _vde_depth_t {
    ofe_idx_t ofe;                //    = 10'h100 ~ 10'h1FC
    volatile uint32_t cvb_ctrl;   //    = 10'h200   ;
    volatile uint32_t res0[3];
    volatile uint32_t sgm_ctrl;   //    = 10'h210   ;
    volatile uint32_t res1[3];
    volatile uint32_t wta_ctrl;   //    = 10'h220   ;
    volatile uint32_t res2[3];
    volatile uint32_t lrc_ctrl;   //    = 10'h230   ;
    volatile uint32_t res3[3];
    lrc_th_t lrc_th;              //    = 10'h240 ~ 10'h25C
    volatile uint32_t dpp_ctrl;   //    = 10'h260   ;
    volatile uint32_t dpp_th;     //    = 10'h264   ;
    volatile uint32_t dpp_start;  //    = 10'h268   ;
    volatile uint32_t dpp_stop;   //    = 10'h26C   ;
    volatile uint32_t res4[4];    //    = 10'h270 ~ 10'h27c ;
    volatile uint32_t dtd_ctrl;   //    = 10'h280
    volatile uint32_t dtd_lut;    //    = 10'h284
    volatile uint32_t res5[6];    //    = 10'h288 ~ 10'h29c ;
    volatile uint32_t saf_ctrl;   //    = 10'h2A0
    volatile uint32_t saf_sp;     //    = 10'h2A4
    volatile uint32_t res6[82];
    volatile uint32_t mux_ctrl;   //    = 10'h3F0   ;
    volatile uint32_t res7[3];    //    = 10'h3F4   ;
} __attribute__((packed, aligned(4))) vde_depth_t;

typedef struct _vde_dev_t {
    //vde_rectify_t   rec0;       // 0x000 ~ 0x07C
    //vde_rectify_t   rec1;       // 0x000 ~ 0x0FC
    vde_rectify_t   rec[2];
    vde_depth_t     depth;      // 0x100 ~ 0x3FC
    //mi_wr_t         miw0;       // 0x400 ~ 0x47C
    //mi_wr_t         miw1;       // 0x480 ~ 0x4FC
    //mi_wr_t         miw2;       // 0x500 ~ 0x57C
    mi_wr_t         miwr[3];    
    mi_rd_t         mird;       // 0x580 ~ 0x5FC
} __attribute__((packed, aligned(4))) vde_dev_t;

int vde_init(vde_param_t *param);
int vde_start(vde_param_t *param);
int vde_complete(void);

/* no lrc, just one round */
int vde_depth_no_lrc_calc(vde_param_t param);

/* lrc, need to run twice inside */
int vde_depth_lrc_dpp_calc(vde_param_t param);

typedef int (*vde_irq_cb_t)(void *ctx);
int  vde_irq_register(int irq, uint32_t priority, vde_irq_cb_t cb, void *ctx);
int  vde_irq_unregister(int irq);
void vde_irq_clear_status(int irq);

int  vde_dtd_calc_lut(float b, float f, float scale);
void vde_dtd_disable(void);
void vde_dtd_enable(void);

int vde_fill_color_map(uint16_t *color_map, int cnt);

int vde_dtd_fill_lut(uint16_t *buffer, int offset, int count);

int vde_set_lrc_th_d(int index, uint32_t value);
uint32_t vde_get_lrc_th_d(int index);
int vde_dpp_enable_first_round(uint32_t value);
uint32_t vde_dpp_get_first_round(void);

int vde_ofe_idx_init(uint32_t *idx, int cnt);

#ifdef __cplusplus
}
#endif

#endif //__VDE_DEPTH_H__
