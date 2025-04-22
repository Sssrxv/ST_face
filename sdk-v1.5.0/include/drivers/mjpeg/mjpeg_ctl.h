#ifndef __MJPEG_CTL_H__
#define __MJPEG_CTL_H__

#include <stdint.h>
#include "mi_rd.h"
#include "mi_wr.h"
#include "mjpeg.h"


#ifdef __cplusplus
extern "C" {
#endif

//gen_header
#define JPEG_GEN_HEADER_START        (1)
//encode
#define JPEG_ENCODE_START            (1)
//init
#define JPEG_INIT_START              (1)
//y_scale_en
#define JPEG_Y_SCALE_EN              (1)
//cbcr_scale_en
#define JPEG_CBCR_SCALE_EN           (1)
//table_flush
#define JPEG_TABLE_FLUSH             (1)

//enc_hsize
#define JPEG_ENC_HSIZE(x)            ((x) & ((1UL << 15) - 1))
//enc_vsize
#define JPEG_ENC_VSIZE(x)            ((x) & ((1UL << 14) - 1))

//pic_format
#define JPEG_FMT_YUV422              (1)
#define JPEG_FMT_YUV400              (4)

//restart_interval
#define JPEG_RES_INTERVAL(x)         ((x) & ((1UL << 16) - 1))

//tq_y_select, tq_u_select, tq_v_select
#define JPEG_QTABLE_0                (0)
#define JPEG_QTABLE_1                (1)
#define JPEG_QTABLE_2                (2)
#define JPEG_QTABLE_3                (3)

//dc_table_select
#define JPEG_DC_TABLE_Y0             (0)
#define JPEG_DC_TABLE_Y1             (1)
#define JPEG_DC_TABLE_U0             (0)
#define JPEG_DC_TABLE_U1             (1 << 1)
#define JPEG_DC_TABLE_V0             (0)
#define JPEG_DC_TABLE_V1             (1 << 2)

//ac_table_select
#define JPEG_AC_TABLE_Y2             (0)
#define JPEG_AC_TABLE_Y1             (1)
#define JPEG_AC_TABLE_U0             (0)
#define JPEG_AC_TABLE_U1             (1 << 1)
#define JPEG_AC_TABLE_V0             (0)
#define JPEG_AC_TABLE_V1             (1 << 2)

//table_data
#define JPEG_TABLE_DATA(h, l)       ((((h) & 0xFF) << 8) | ((l) & 0xFF))

//table_id
#define JPEG_Q_TABLE_0               (0)
#define JPEG_Q_TABLE_1               (1)
#define JPEG_Q_TABLE_2               (2)
#define JPEG_Q_TABLE_3               (3)
#define JPEG_VLC_DC_TABLE_0          (4)
#define JPEG_VLC_AC_TABLE_0          (5)
#define JPEG_VLC_DC_TABLE_1          (6)
#define JPEG_VLC_AC_TABLE_1          (7)

//tac0_len
#define JPEG_TAC0_LEN(x)             ((x) & 0xFF)
//tdc0_len
#define JPEG_TDC0_LEN(x)             ((x) & 0xFF)
//tac1_len
#define JPEG_TAC1_LEN(x)             ((x) & 0xFF)
//tdc1_len
#define JPEG_TDC1_LEN(x)             ((x) & 0xFF)

//codec_busy
#define JPEG_CODEC_BUSY              (1)

//header_mode
#define JPEG_NO_HEADER               (0)
#define JPEG_JFIF_102                (2)

//error_imr, error_ris, error_mis, error_icr, error_isr
#define JPEG_VLC_SYM_ERR             (1UL << 4)
#define JPEG_DCT_ERR                 (1UL << 7)
#define JPEG_R2B_IMG_SZ_ERR          (1UL << 9)
#define JPEG_VLC_TABLE_ERR           (1UL << 10)

// status_imr, status_ris, status_mis, status_icr, status_isr
#define JPEG_ENCODE_DONE             (1UL << 4)
#define JPEG_GEN_HDR_DONE            (1UL << 5)

//config
#define JPEG_STOP_AT_FRAME_END       (0)
#define JPEG_START_AUTO              (1)
#define JPEG_EXT_START_AUTO          (3)

/* MJPEG encoder struct */
typedef struct _mjpeg_enc_t {
    volatile uint32_t gen_header;       // (00h), WO
    volatile uint32_t encode;           // (04h), WO
    volatile uint32_t init;             // (08h), WO
    volatile uint32_t y_scale_en;       // (0Ch), RW
    volatile uint32_t cbcr_scale_en;    // (10h), RW
    volatile uint32_t table_flush;      // (14h), RW
    volatile uint32_t enc_hsize;        // (18h), RW
    volatile uint32_t enc_vsize;        // (1ch), RW
    volatile uint32_t pic_format;       // (20h), RW
    volatile uint32_t restart_interval; // (24h), RW
    volatile uint32_t tq_y_select;      // (28h), RW
    volatile uint32_t tq_u_select;      // (2ch), RW
    volatile uint32_t tq_v_select;      // (30h), RW
    volatile uint32_t dc_table_select;  // (34h), RW
    volatile uint32_t ac_table_select;  // (38h), RW
    volatile uint32_t table_data;       // (3ch), WO
    volatile uint32_t table_id;         // (40h), RW
    volatile uint32_t tac0_len;         // (44h), RW
    volatile uint32_t tdc0_len;         // (48h), RW
    volatile uint32_t tac1_len;         // (4ch), RW
    volatile uint32_t tdc1_len;         // (50h), RW
    volatile uint32_t res0;             // (54h), RW
    volatile uint32_t codec_busy;       // (58h), RO
    volatile uint32_t header_mode;      // (5ch), RW
    volatile uint32_t encode_mode;      // (60h), RO
    volatile uint32_t debug;            // (64h), RO
    volatile uint32_t error_imr;        // (68h), RW
    volatile uint32_t error_ris;        // (6ch), RO
    volatile uint32_t error_mis;        // (70h), RO
    volatile uint32_t error_icr;        // (74h), WO
    volatile uint32_t error_isr;        // (78h), WO
    volatile uint32_t status_imr;       // (7ch), RW
    volatile uint32_t status_ris;       // (80h), RO
    volatile uint32_t status_mis;       // (84h), RO
    volatile uint32_t status_icr;       // (88h), WO
    volatile uint32_t status_isr;       // (8ch), WO
    volatile uint32_t config;           // (90h), RW
    volatile uint32_t res1[27];          // (94h, 98h, 9ch)
    // 180h - 1fch mi_wr, 200h - 27ch mi_rd
    volatile uint32_t ret_bytes;        // (100h), RO
    volatile uint32_t res2[31];
    volatile mi_wr_t  mi_wr;            // 180h - 1fch
    volatile mi_rd_t  mi_rd;            // 200h - 27ch
} __attribute__((packed, aligned(4))) mjpeg_enc_t;

//int mjpeg_ctl_init(int w, int h, mjpeg_comp_level_t level);
int mjpeg_ctl_init(int w, int h, int x_stride, mjpeg_comp_level_t level);

int mjpeg_set_compress_level(mjpeg_comp_level_t level);

//int mjpeg_ctl_start(void *y_addr, void *c_addr, void *out_buf);
int mjpeg_ctl_start(void *y_addr, void *c_addr, void *out_buf, uint32_t mird_fmt);

int mjpeg_ctl_complete(int ms);

int mjpeg_ctl_release(void);

int mjpeg_miwr_irq_config(int enable);

int mjpeg_miwr_clear_interp_status(void);

#ifdef __cplusplus
}
#endif

#endif // __MJPEG_CTL_H__
