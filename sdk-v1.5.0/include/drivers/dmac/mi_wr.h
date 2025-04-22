#ifndef __MI_WR_H__
#define __MI_WR_H__

#include <stdint.h>
#include "io.h"

#ifdef __cplusplus
extern "C" {
#endif

/* c_mi_wr_enable */
// enable DMA, 1 - shadow enable, 0 - shadow disable, controlled by cfg_upd/gen_upd
#define MI_WR_ENABLE_EN                                     (1)
#define MI_WR_ENABLE_DIS                                    (0)

/* c_mi_wr_cfg_upd */
// force update for shadow registers, take effect immediately, 1 - effective, 0 - no effect
#define MI_WR_CFG_UPD_EN                                    (1)

/* c_mi_wr_gen_upd */
// update at frame end, 1 - effective, 0 - no effect
#define MI_WR_GEN_UPD_EN                                    (1)

/* c_mi_wr_ctrl */
// data_format[2:0]
#define MI_WR_FMT_YUYV                                      (0UL)
#define MI_WR_FMT_RGBA                                      (3UL)
#define MI_WR_FMT_RAW8                                      (4UL)
#define MI_WR_FMT_RAW16                                     (5UL)
#define MI_WR_FMT_RAW32                                     (6UL)
#define MI_WR_FMT_RAW64                                     (7UL)
// y_full[4]
// c_full[8]
// noncosited[12]
// burst_len[17:16]
#define MI_WR_BURST4                                        (0UL << 16)
#define MI_WR_BURST8                                        (1UL << 16)
#define MI_WR_BURST16                                       (2UL << 16)
// data_swap[21:20]
#define MI_WR_NO_SWAP                                       (0UL << 20)
#define MI_WR_BYTE_SWAP                                     (1UL << 20)
#define MI_WR_HALF_WORD_SWAP                                (2UL << 20)
#define MI_WR_WORD_SWAP                                     (3UL << 20)
// x_skip[24]


/* c_mi_wr_ris */
#define MI_WR_INT_IN_FRAME_END                              (1UL << 0)
#define MI_WR_INT_OUT_FRAME_END                             (1UL << 1)
#define MI_WR_INT_LEAK_START                                (1UL << 2)
#define MI_WR_INT_LEAK_END                                  (1UL << 3)

/* c_mi_wr_leak_en */
#define MI_WR_LEAK_EN                                       (1UL)
#define MI_WR_LEAK_DIS                                      (0UL)

/*
 *  WR-DMA: Memory Interface Write Struct
 */

typedef struct _mi_wr {
    /* Write Enable                             (00h) */
    volatile uint32_t c_mi_wr_enable;
    /* Config update                            (04h) */
    volatile uint32_t c_mi_wr_cfg_upd;
    /* Generate update                          (08h) */
    volatile uint32_t c_mi_wr_gen_upd;
    /* Write ctrl                               (0ch) */
    volatile uint32_t c_mi_wr_ctrl;
    /* Reserved                                 (10h) */
    volatile uint32_t res0[3];
    /* DMA: x stride                            (1ch) */
    volatile uint32_t c_mi_wr_x_stride;         // a stride of data item defined by wr_ctrl
    /* DMA: y stride                            (20h) */
    volatile uint32_t c_mi_wr_y_stride;         // a stride of data item defined by wr_ctrl
    /* DMA: y address                           (24h) */
    volatile uint32_t c_mi_wr_y_addr;           // shadow register controlled by cfg_upd/gen_upd registers
    /* DMA: c address                           (28h) */
    volatile uint32_t c_mi_wr_c_addr;           // shadow register controlled by cfg_upd/gen_upd registers
    /* Reserved:                                (2ch) */
    volatile uint32_t res1[1];
    /* Shadow register: wr_enable               (30h) */
    volatile uint32_t c_mi_wr_enable_shd;
    /* Shadow register: y_addr_shd              (34h) */
    volatile uint32_t c_mi_wr_y_addr_shd;
    /* Shadow register: c_addr_shd              (38h) */
    volatile uint32_t c_mi_wr_c_addr_shd;
    /* Reserved:                                (3ch) */
    volatile uint32_t res2[1];
    /* Debug state                              (40h) */
    volatile uint32_t c_mi_wr_dbg_state;
    /* Debug y_ptr                              (44h) */
    volatile uint32_t c_mi_wr_dbg_y_ptr;
    /* Debug c_ptr                              (48h) */
    volatile uint32_t c_mi_wr_dbg_c_ptr;
    /* Debug cmd_cnt                            (4ch) */
    volatile uint32_t c_mi_wr_dbg_cmd_cnt;
    /* Debug rsp_cnt                            (50h) */
    volatile uint32_t c_mi_wr_dbg_rsp_cnt;
    /* Reserved:                                (54h) */
    volatile uint32_t res3;
	/* Z stride 								(58h) */
	volatile uint32_t c_mi_wr_z_stride;
	/* Reserved: 								(5ch) */
	volatile uint32_t res4;
    /* Interrupt mask                           (60h) */
    volatile uint32_t c_mi_wr_imsc;
    /* Interrupt status                         (64h) */
    volatile uint32_t c_mi_wr_ris;
    /* Mask interrupt status                    (68h) */
    volatile uint32_t c_mi_wr_mis;
    /* Interrupt clear                          (6ch) */
    volatile uint32_t c_mi_wr_icr;
    /* Interrupt set                            (70h) */
    volatile uint32_t c_mi_wr_isr;
    /* Reserved                                 (74h) */
    volatile uint32_t res5[1];
    /* Leaky Enable                             (78h) */
    volatile uint32_t c_mi_wr_leak_en;
    /* SW leak                                  (7ch) */
    volatile uint32_t c_mi_wr_sw_leak;
} __attribute__((packed, aligned(4))) mi_wr_t;

/* set wr dma interrupt status */
void mi_wr_set_interp_mask(volatile mi_wr_t *mi_wr, uint32_t mask, writel_func_t writel_func);
/* clear wr dma interrupt mask */
void mi_wr_clear_interp_mask(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* clear wr dma interrupt status */
void mi_wr_clear_interp_status(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* enable wr dma, this is a shadow register controlled by cfg_upd / gen_upd */
void mi_wr_shadow_enable(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* disable wr dma, this is a shadow register controlled by cfg_upd / gen_upd */
void mi_wr_shadow_disable(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* set lumia address, shadow register */
void mi_wr_set_y_addr(volatile mi_wr_t *mi_wr, uint32_t y_addr, writel_func_t writel_func);
/* set chrome address, shadow register */
void mi_wr_set_c_addr(volatile mi_wr_t *mi_wr, uint32_t c_addr, writel_func_t writel_func);
/* get raw interrupt status */
uint32_t mi_wr_get_ris(volatile mi_wr_t *mi_wr, readl_func_t readl_func);
/* set input format control, shadow register */
void mi_wr_set_fmt_ctrl(volatile mi_wr_t *mi_wr, uint32_t ctrl, writel_func_t writel_func);
/* set dma x stride, shadow register */
void mi_wr_set_x_stride(volatile mi_wr_t *mi_wr, uint32_t x_stride, writel_func_t writel_func);
/* set dma y stride, shadow register */
void mi_wr_set_y_stride(volatile mi_wr_t *mi_wr, uint32_t y_stride, writel_func_t writel_func);
/* set dma z stride, shadow register */
void mi_wr_set_z_stride(volatile mi_wr_t *mi_wr, uint32_t z_stride, writel_func_t writel_func);
/* leaky enable, shadow register */
void mi_wr_leaky_enable(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
void mi_wr_sw_leak_enable(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* force update all mi_wr shadow registers, take effect immediately */
void mi_wr_force_update(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* generate update all mi_wr shadow registers, take effect at frame end */
void mi_wr_gen_update(volatile mi_wr_t *mi_wr, writel_func_t writel_func);
/* get y_addr shadow */
uint32_t mi_wr_get_y_addr_shd(volatile mi_wr_t *mi_wr, readl_func_t readl_func);
/* get c_addr shadow */
uint32_t mi_wr_get_c_addr_shd(volatile mi_wr_t *mi_wr, readl_func_t readl_func);
int mi_wr_complete(volatile mi_wr_t *mi_wr, checkl_func_t checkl_func);

#ifdef __cplusplus
}
#endif

#endif // __MI_WR_H__
