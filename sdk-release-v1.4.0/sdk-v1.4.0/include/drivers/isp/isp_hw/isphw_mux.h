#ifndef _ISPHW_MUX_H_
#define _ISPHW_MUX_H_

#include "isp_types.h"
#include "isp_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum _isp_prepproc_input_t
{
    ISP_PRE_PROC_INPUT_INVALID       = -1,
    ISP_PRE_PROC_INPUT_FROM_PARALLEL = 0,
    ISP_PRE_PROC_INPUT_FROM_MIPI0    = 1,
    ISP_PRE_PROC_INPUT_FROM_MIPI1    = 2,
    ISP_PRE_PROC_INPUT_FROM_RDMA     = 3,
    ISP_PRE_PROC_INPUT_MAX
}isp_prepproc_input_t;


typedef enum _isp_kernel_input_t
{
    ISP_KERNEL_INPUT_INVALID        = -1,
    ISP_KERNEL_INPUT_FROM_PARALLEL  = 0,
    ISP_KERNEL_INPUT_FROM_MIPI0     = 1,
    ISP_KERNEL_INPUT_FROM_MIPI1     = 2,
    ISP_KERNEL_INPUT_FROM_PRE_ISP   = 3,
    ISP_KERNEL_INPUT_FROM_RDMA      = 4,
    ISP_KERNEL_INPUT_MAX
}isp_kernel_input_t;

typedef enum _ifsel_input_t
{
    IF_SEL_INPUT_INVALID            = -1,
    IF_SEL_INPUT_FROM_DVP           = 0,
    IF_SEL_INPUT_FROM_MIPI0         = 1,
    IF_SEL_INPUT_FROM_MIPI1         = 2,
    IF_SEL_INPUT_FROM_RDMA          = 3,
    IF_SEL_INPUT_MAX
}ifsel_input_t;

typedef enum _ycsplit_mux_input_t
{
    YC_SPLIT_MUX_INPUT_INVALID      = -1,
    YC_SPLIT_MUX_INPUT_FROM_CPROC   = 0,
    YC_SPLIT_MUX_INPUT_FROM_RDMA    = 1,
    YC_SPLIT_MUX_INPUT_MAX
}ycsplit_mux_input_t;


typedef enum _mp_mux_input_t
{
    MP_MUX_INPUT_INVALID            = -1,
    MP_MUX_INPUT_FROM_PRE_ISP       = 0,
    MP_MUX_INPUT_FROM_ISP_KERNEL    = 1,
    MP_MUX_INPUT_FROM_RDMA          = 2,
    MP_MUX_INPUT_FROM_RSZM          = 3,
    MP_MUX_INPUT_MAX
}mp_mux_input_t;


typedef enum _yuv_dma_sel_t
{
    YUV_DMA_SEL_INVALID             = -1,
    YUV_DMA_SEL_422ALIGN_422CONV    = 0,
    YUV_DMA_SEL_DMA_YUV             = 1,
    YUV_DMA_SEL_MAX
} yuv_dma_sel_t;


typedef enum _rgb_dma_sel_t
{
    RGB_DMA_SEL_INVALID             = -1,
    RGB_DMA_SEL_INFORM              = 0,
    RGB_DMA_SEL_DMA_RGB             = 1,
    RGB_DMA_SEL_MAX
} rgb_dma_sel_t;



int isphw_mux_enable_preview(void);
int isphw_mux_disable_preview(void);
int isphw_mux_enable_video(void);
int isphw_mux_disable_video(void);
int isphw_mux_enable_zsl(void);
int isphw_mux_disable_zsl(void);
int isphw_mux_cfg_upd(void);
int isphw_mux_set_preproc_input(isp_prepproc_input_t preproc_input); // select for isp preproc input
int isphw_mux_set_isp_kernel_input(isp_kernel_input_t isp_kernel_input); // select for isp kerkel input
int isphw_mux_set_if_sel(ifsel_input_t ifsel_input); // select for external interface input
int isphw_mux_set_ycsplit_mux_input(ycsplit_mux_input_t ycsplit_mux_input); // select for yc_split input
int isphw_mux_set_mp_mux_input(mp_mux_input_t mp_mux_input); // select for mp wdma input
int isphw_mux_set_yuv_dma_sel(yuv_dma_sel_t yuv_dma_sel); // select for isp_is input
int isphw_mux_set_rgb_dma_sel(rgb_dma_sel_t rgb_dma_sel); // select for latency fifo input







#ifdef __cplusplus
}
#endif





#endif
