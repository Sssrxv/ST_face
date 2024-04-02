#ifndef __MI_RD_H__
#define __MI_RD_H__

#include <stdint.h>
#include "io.h"

#ifdef __cplusplus
extern "C" {
#endif

/* c_mi_rd_enable */
#define MI_RD_SINGLE_MODE                                   (0UL)
#define MI_RD_CONTINOUS_MODE                                (1UL)

/* c_mi_rd_sw_start */
#define MI_RD_SW_START                                      (1UL)

/* c_mi_rd_gen_upd */
#define MI_RD_GEN_UPD                                       (1UL)

/* c_mi_rd_ctrl     */
/* data format[2:0] */
#define MI_RD_FMT_YUYV                                      (0UL)
#define MI_RD_FMT_RGBA                                      (3UL)
#define MI_RD_FMT_RAW8                                      (4UL)
#define MI_RD_FMT_RAW16                                     (5UL)
#define MI_RD_FMT_RAW32                                     (6UL)
#define MI_RD_FMT_RAW64                                     (7UL)
/* burst length[17:16] */
#define MI_RD_BURST4                                        (0UL << 16)
#define MI_RD_BURST8                                        (1UL << 16)
#define MI_RD_BURST16                                       (2UL << 16)
/* byte swap[21:20] */
#define MI_RD_NO_SWAP                                       (0UL << 20)
#define MI_RD_BYTE_SWAP                                     (1UL << 20)
#define MI_RD_HALF_WORD_SWAP                                (2UL << 20)
#define MI_RD_WORD_SWAP                                     (3UL << 20)

/* c_mi_rd_ris */
#define MI_RD_FRAME_END_INT                                 (3UL)
#define MI_RD_LEAKY_INT                                     (3UL << 2)

/* c_mi_rd_leak_en */
#define MI_RD_LEAK_EN                                       (1UL)
#define MI_RD_LEAK_DIS                                      (0UL)

/*
 * RD-DMA:  Memory Interface Read Struct 
 */
typedef struct _mi_rd {
    /* Enable: 0 - sigle mode, 1 - continous mode           (00h) */
    volatile uint32_t c_mi_rd_enable;
    /* SW start: start dma engine                           (04h) */
    volatile uint32_t c_mi_rd_sw_start;
    /* Generate Update: update Y(Luma) and C(chrome) address(08h) */
    volatile uint32_t c_mi_rd_gen_upd;
    /* Read Ctrl: data_format[2:0], burst_len[17:16], 
     *            data_swap[21:20]                          (0ch) */
    volatile uint32_t c_mi_rd_ctrl;
    /* 3D-DMA: x axis data length                           (10h) */
    volatile uint32_t c_mi_rd_x_length;
    /* 3D-DMA: y axis data length                           (14h) */
    volatile uint32_t c_mi_rd_y_length;
    /* 3D-DMA: z axis data length                           (18h) */
    volatile uint32_t c_mi_rd_z_length;
    /* 3D-DMA: x axis data stride                           (1ch) */
    volatile uint32_t c_mi_rd_x_stride;
    /* 3D-DMA: y axis data stride                           (20h) */
    volatile uint32_t c_mi_rd_y_stride;
    /* Y(Luma) address, 8-byte aligned                      (24h) */
    volatile uint32_t c_mi_rd_y_addr;
    /* C(Chrome) address, 8-byte aligned                    (28h) */
    volatile uint32_t c_mi_rd_c_addr;
    /* Reserved                                             (2ch) */
    volatile uint32_t res0;
    /* rd_enable shadow                                     (30h) */
    volatile uint32_t c_mi_rd_enable_shd;
    /* Y address shadow                                     (34h) */
    volatile uint32_t c_mi_rd_y_addr_shd;
    /* C address shadow                                     (38h) */
    volatile uint32_t c_mi_rd_c_addr_shd;
    /* Reserved                                             (3ch) */
    volatile uint32_t res1;
    /* rd_dbg_state                                         (40h) */
    volatile uint32_t c_mi_rd_dbg_state;
    /* rd_dbg_y_ptr                                         (44h) */
    volatile uint32_t c_mi_rd_dbg_y_ptr;
    /* rd_dbg_c_ptr                                         (48h) */
    volatile uint32_t c_mi_rd_dbg_c_ptr;
    /* rg_dgb_cmd_cnt                                       (4ch) */
    volatile uint32_t c_mi_rd_dbg_cmd_cnt;
    /* rd_dbg_rsp_cnt                                       (50h) */
    volatile uint32_t c_mi_rd_dbg_rsp_cnt;
    /* rd_dbg_fifo                                          (54h) */
    volatile uint32_t c_mi_rd_dbg_fifo;
    /* Reserved                                             (58h) */
    volatile uint32_t res2;
    /* Reserved                                             (5ch) */
    volatile uint32_t res3;
    /* Interrupt Mask                                       (60h) */
    volatile uint32_t c_mi_rd_imsc;
    /* Raw Interrupt Status, 
     * [1:0] mi_read frame end, 3 - frame end, 0 - busy
     * [3:2] leaky interrupt, 
     *      3 - leaky status, disable continous mode
     *      0 - normal                                      (64h) */
    volatile uint32_t c_mi_rd_ris;
    /* Masked interrput status                              (68h) */
    volatile uint32_t c_mi_rd_mis;
    /* Interrupt Clear                                      (6ch) */
    volatile uint32_t c_mi_rd_icr;
    /* Interrput Set                                        (70h) */
    volatile uint32_t c_mi_rd_isr;
    /* Reserved                                             (74h) */
    volatile uint32_t res4;
    /* Leaky Enable:
     *      0 - disable DMA handshake
     *      1 - enable DMA handshake                        (78h) */
    volatile uint32_t c_mi_rd_leak_en;
    /* rd_sw_leak                                           (7ch) */
    volatile uint32_t c_mi_rd_sw_leak;
} __attribute__((packed, aligned(4))) mi_rd_t;

void mi_rd_set_interp_mask(volatile mi_rd_t *mi_rd, uint32_t mask, writel_func_t mi_rd_writel);
void mi_rd_clear_interp_mask(volatile mi_rd_t *mi_rd, writel_func_t mi_rd_writel);
void mi_rd_clear_interp_status(volatile mi_rd_t *mi_rd, writel_func_t mi_rd_writel);
void mi_rd_set_mode(volatile mi_rd_t *mi_rd, int mode, writel_func_t writel_func);
void mi_rd_set_y_addr(volatile mi_rd_t *mi_rd, uint32_t y_addr, writel_func_t writel_func);
void mi_rd_set_c_addr(volatile mi_rd_t *mi_rd, uint32_t c_addr, writel_func_t writel_func);
uint32_t mi_rd_get_ris(volatile mi_rd_t *mi_rd, readl_func_t read_func);

void mi_rd_set_fmt_ctrl(volatile mi_rd_t *mi_rd, uint32_t ctrl, writel_func_t writel_func);
void mi_rd_set_x_length(volatile mi_rd_t *mi_rd, uint32_t x_length, writel_func_t writel_func);
void mi_rd_set_y_length(volatile mi_rd_t *mi_rd, uint32_t y_length, writel_func_t writel_func);
void mi_rd_set_z_length(volatile mi_rd_t *mi_rd, uint32_t z_length, writel_func_t writel_func);
void mi_rd_set_x_stride(volatile mi_rd_t *mi_rd, uint32_t x_stride, writel_func_t writel_func);
void mi_rd_set_y_stride(volatile mi_rd_t *mi_rd, uint32_t y_stride, writel_func_t writel_func);
void mi_rd_leaky_enable(volatile mi_rd_t *mi_rd, writel_func_t writel_func);
void mi_rd_sw_leak_enable(volatile mi_rd_t *mi_rd, writel_func_t writel_func);
uint32_t mi_rd_get_y_addr_shd(volatile mi_rd_t *mi_rd, readl_func_t read_func);
uint32_t mi_rd_get_c_addr_shd(volatile mi_rd_t *mi_rd, readl_func_t readl_func);
void mi_rd_sw_start(volatile mi_rd_t *mi_rd, writel_func_t writel_func);
void mi_rd_gen_update(volatile mi_rd_t *mi_rd, writel_func_t writel_func);
int mi_rd_complete(volatile mi_rd_t *mi_rd, checkl_func_t checkl_func);

#ifdef __cplusplus
}
#endif

#endif //__MI_RD_H__
