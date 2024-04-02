#ifndef __RECTIFY_CTL_H__
#define __RECTIFY_CTL_H__

#include <stdint.h>
#include "mi_wr.h"
#include "mi_rd.h"
//#include "rectify.h"

#ifdef __cplusplus
extern "C" {
#endif

// FIXME: comment out below line after debug
//#define RECTIFY_DEBUG

uint32_t fpd2fp(double fpd, int expo_width, int mant_width);

#ifndef RECTIFY_DEBUG
//typedef double rec_type_t;
//typedef float rec_type_t;
#define FPD2FP(f, e, m) fpd2fp(f, e, m)
#else
//typedef uint32_t rec_type_t;
#define FPD2FP(f, e, m) (f)
#endif // RECTIFY_DEBUG

#define REC_INPUT_REVERSE               (1)
#define REC_INPUT_NO_REVERSE            (0)

#define VDE_MI_WR0                      (0)
#define VDE_MI_WR1                      (1)
#define VDE_MI_WR2                      (2)

#define VDE_REC0                        (0)
#define VDE_REC1                        (1)

#define VDE_IMG_GRAY                    (0)

/* c_lcm0/1_ctrl */
#define VDE_LCM_HLENGTH(x)              ((x) & 0x0FC0)
#define VDE_LCM_HSTRIDE(x)              (((x) & 0x0FC0) << 16)

/* c_lcm0/1_addr */
#define VDE_LCM_ADDR(x)                 ((x) & (~0x3F))

/* c_mge0/1_start */
#define VDE_MGE_HSTART(x)               ((x) & 0x7FF)
#define VDE_MGE_VSTART(x)               ((x) << 16)

/* c_mge0/1_stop */
#define VDE_MGE_HSTOP(x)                ((x) & 0x7FF)
#define VDE_MGE_VSTOP(x)                ((x) << 16)

/* c_mge0/1_max */
#define VDE_MGE_HMAX(x)                 ((x) & 0x7FF)
#define VDE_MGE_VMAX(x)                 ((x) << 16)

#define VDE_NO_CROP                     (0)
#define VDE_CROP                        (1)


//typedef struct _vde_cam_param_t {
    //uint32_t img_w;
    //uint32_t img_h;
    //rec_type_t  u0;
    //rec_type_t  v0;
    //rec_type_t  fx;
    //rec_type_t  fy;
    //rec_type_t  k1;
    //rec_type_t  k2;
    //rec_type_t  p1;
    //rec_type_t  p2;
    //rec_type_t  r0;
    //rec_type_t  r1;
    //rec_type_t  r2;
    //rec_type_t  r3;
    //rec_type_t  r4;
    //rec_type_t  r5;
    //rec_type_t  r6;
    //rec_type_t  r7;
    //rec_type_t  r8;
//} __attribute__((packed, aligned(4))) vde_cam_param_t;

//typedef struct _vde_q_param_t {
    //rec_type_t cx;
    //rec_type_t cy;
    //rec_type_t f;
    //rec_type_t invTx;
    //rec_type_t scale;  // depth scale factor
//} vde_q_param_t;

typedef struct _vde_rec_param_t {
    uint32_t        id;              // id, 0/1
    uint32_t        img_fmt;         // image format, currently just support gray image
    uint32_t        input_reverse;   // reverse read input image
    uint32_t        output_mux;      // whether to output rectified image and depth image
    vde_cam_param_t cam;             // camera parameter
    uint32_t        src_addr;        // image source addr
    uint32_t        src_x_stride;    // lcm h_stride
    uint32_t        dst_addr;        // rectified image dest addr
    uint32_t        dst_x_stride;    // mi_wr x_stride
    uint32_t        roi_ox;          // roi x offset
    uint32_t        roi_oy;          // roi y offset
    uint32_t        roi_w;           // roi width
    uint32_t        roi_h;           // roi height
} __attribute__((packed, aligned(4))) vde_rec_param_t;


/* Mulptile Generation Engine */
typedef struct _vde_mge_t {
    volatile uint32_t   start;     // 0x000
    volatile uint32_t   stop;      // 0x004
    volatile uint32_t   max;       // 0x008
    volatile uint32_t   u0;        // 0x00C
    volatile uint32_t   v0;        // 0x010
    volatile uint32_t   fx;        // 0x014
    volatile uint32_t   fy;        // 0x018
    volatile uint32_t   k1;        // 0x01C
    volatile uint32_t   k2;        // 0x020
    volatile uint32_t   p1;        // 0x024
    volatile uint32_t   p2;        // 0x028
    volatile uint32_t   r0;        // 0x02C
    volatile uint32_t   r1;        // 0x030
    volatile uint32_t   r2;        // 0x034
    volatile uint32_t   r3;        // 0x038
    volatile uint32_t   r4;        // 0x03C
    volatile uint32_t   r5;        // 0x040
    volatile uint32_t   r6;        // 0x044
    volatile uint32_t   r7;        // 0x048
    volatile uint32_t   r8;        // 0x04C
} __attribute__((packed, aligned(4))) vde_mge_t;

/* Local Cache Memory */
typedef struct _vde_lcm_t {
    volatile uint32_t   ctrl;      // 0x000
    volatile uint32_t   addr;      // 0x004
    volatile uint32_t   id;        // 0x008
    volatile uint32_t   start;     // 0x00C
    volatile uint32_t   res0[8];
} __attribute__((packed, aligned(4))) vde_lcm_t;

#if 1
typedef struct _vde_rectify_t {
    vde_mge_t mge;
    vde_lcm_t lcm;
} __attribute__((packed, aligned(4))) vde_rectify_t;
#else
typedef struct _vde_rectify_t {
    volatile uint32_t c_mge0_start;     // 0x000
    volatile uint32_t c_mge0_stop;      // 0x004
    volatile uint32_t c_mge0_max;       // 0x008
    volatile uint32_t c_mge0_p_u0;      // 0x00C
    volatile uint32_t c_mge0_p_v0;      // 0x010
    volatile uint32_t c_mge0_p_fx;      // 0x014
    volatile uint32_t c_mge0_p_fy;      // 0x018
    volatile uint32_t c_mge0_p_k1;      // 0x01C
    volatile uint32_t c_mge0_p_k2;      // 0x020
    volatile uint32_t c_mge0_p_p1;      // 0x024
    volatile uint32_t c_mge0_p_p2;      // 0x028
    volatile uint32_t c_mge0_p_r0;      // 0x02C
    volatile uint32_t c_mge0_p_r1;      // 0x030
    volatile uint32_t c_mge0_p_r2;      // 0x034
    volatile uint32_t c_mge0_p_r3;      // 0x038
    volatile uint32_t c_mge0_p_r4;      // 0x03C
    volatile uint32_t c_mge0_p_r5;      // 0x040
    volatile uint32_t c_mge0_p_r6;      // 0x044
    volatile uint32_t c_mge0_p_r7;      // 0x048
    volatile uint32_t c_mge0_p_r8;      // 0x04C
    volatile uint32_t c_lcm0_ctrl;      // 0x050
    volatile uint32_t c_lcm0_addr;      // 0x054
    volatile uint32_t c_lcm0_id;        // 0x058
    volatile uint32_t c_lcm0_start;     // 0x05C
    volatile uint32_t res0[8];          // 0x060 ~ 0x07C
    volatile uint32_t c_mge1_start;     // 0x080
    volatile uint32_t c_mge1_stop;      // 0x084
    volatile uint32_t c_mge1_max;       // 0x088
    volatile uint32_t c_mge1_p_u0;      // 0x08C
    volatile uint32_t c_mge1_p_v0;      // 0x090
    volatile uint32_t c_mge1_p_fx;      // 0x094
    volatile uint32_t c_mge1_p_fy;      // 0x098
    volatile uint32_t c_mge1_p_k1;      // 0x09C
    volatile uint32_t c_mge1_p_k2;      // 0x0A0
    volatile uint32_t c_mge1_p_p1;      // 0x0A4
    volatile uint32_t c_mge1_p_p2;      // 0x0A8
    volatile uint32_t c_mge1_p_r0;      // 0x0AC
    volatile uint32_t c_mge1_p_r1;      // 0x0B0
    volatile uint32_t c_mge1_p_r2;      // 0x0B4
    volatile uint32_t c_mge1_p_r3;      // 0x0B8
    volatile uint32_t c_mge1_p_r4;      // 0x0BC
    volatile uint32_t c_mge1_p_r5;      // 0x0C0
    volatile uint32_t c_mge1_p_r6;      // 0x0C4
    volatile uint32_t c_mge1_p_r7;      // 0x0C8
    volatile uint32_t c_mge1_p_r8;      // 0x0CC
    volatile uint32_t c_lcm1_ctrl;      // 0x0D0
    volatile uint32_t c_lcm1_addr;      // 0x0D4
    volatile uint32_t c_lcm1_id;        // 0x0D8
    volatile uint32_t c_lcm1_start;     // 0x0DC
    volatile uint32_t res1[8];          // 0x0E0 ~ 0x0FC
} __attribute__((packed, aligned(4))) vde_rectify_t;
#endif

int vde_mi_wr_config(int num, uint32_t ctrl, int x_stride, int y_stride, uint32_t dst_addr);
int vde_mi_wr_complete(int num);

int vde_rectify_init(int num, vde_rec_param_t *param);
//int vde_rectify_start(int num);
int vde_rectify_start(int num, vde_rec_param_t *param);
int vde_rectify_complete(int num);

int vde_set_rectify_in_addr(int rec_num, uint32_t load_addr);
int vde_set_rectify_out_addr(int rec_num, uint32_t dst_addr);
int vde_rec_reverse_input(int rec_num, int w, int h, int reverse);

int vde_mux_contrl(uint32_t val);

int vde_stereo_rectifiy_init(vde_rec_param_t *param_l, vde_rec_param_t *param_r);
//int vde_stereo_rectify_start(void);
int vde_stereo_rectify_start(vde_rec_param_t *param_l, vde_rec_param_t *param_r);
int vde_stereo_rectify_complete(void);

int vde_set_rectify_in_addr(int rec_num, uint32_t load_addr);
int vde_set_rectify_out_addr(int rec_num, uint32_t dst_addr);

int vde_set_stereo_rectify_in_addr(uint32_t img_addr_l, uint32_t img_addr_r);
int vde_set_stereo_rectify_out_addr(uint32_t img_addr_l, uint32_t img_addr_r);
//int vde_stereo_reverse_input(int w, int h, int reverse);

#ifdef __cplusplus
}
#endif

#endif //__RECTIFY_CTL_H__
