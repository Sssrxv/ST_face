#ifndef _ISPHW_WDMA_H_
#define _ISPHW_WDMA_H_

#include "isp_types.h"
#include "isphw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _wdma_id_t
{
    WDMA_ID_1    = 0,  //=>Preview Write DMA
    WDMA_ID_2    = 1,  //=>Video Write DMA
    WDMA_ID_3    = 2,  //=>Still Image(ZSL) Write DMA
    WDMA_ID_MAX
}wdma_id_t;


typedef enum _wdma_path_t
{
    WDMA_PATH_MIPI_CH0,
    WDMA_PATH_MIPI_CH1,
    WDMA_PATH_MIPI_CH2,
    WDMA_PATH_ISP_YUV,
    WDMA_PATH_ISP_DP, //Defect pixel output
    WDMA_PATH_ISP_RAW,
    WDMA_PATH_MAX
}wdma_path_t;


typedef enum _wdma_channel_t
{
    WDMA_CHANNEL_MCH0,
    WDMA_CHANNEL_MCH1,
    WDMA_CHANNEL_MCH2,
    WDMA_CHANNEL_ISP,
    WDMA_CHANNEL_MAX
}wdma_channel_t;


int isphw_wdma_set_buffer(wdma_id_t wdma_id, const dma_buffer_t *buffer);
int isphw_wdma_get_shd_buffer(wdma_id_t wdma_id, dma_buffer_t *buffer);
int isphw_wdma_set_pic_size(wdma_id_t wdma_id, dma_data_format_t data_format, const dma_pic_size_t *pic_size);
int isphw_wdma_set_data_format(wdma_id_t wdma_id, wdma_channel_t channel, dma_data_format_t data_format);

int isphw_wdma_set_burst_length(wdma_id_t wdma_id, wdma_channel_t channel, dma_burst_length_t burst_len_a, dma_burst_length_t burst_len_bc);

int isphw_wdma_enable_path(wdma_id_t wdma_id, wdma_path_t wdma_path);
int isphw_wdma_disable_path(wdma_id_t wdma_id, wdma_path_t wdma_path);

int isphw_wdma_enable_leak(wdma_id_t wdma_id);
int isphw_wdma_disable_leak(wdma_id_t wdma_id);

#define WDMA_LEAK_MCH0_MASK   (0x1)
#define WDMA_LEAK_MCH1_MASK   (0x2)
#define WDMA_LEAK_MCH2_MASK   (0x4)
#define WDMA_LEAK_ISP_MASK    (0x8)
int isphw_wdma_get_leak_status(wdma_id_t wdma_id, wdma_channel_t channel, dma_leak_status_t *leak_status);
int isphw_wdma_clr_leak_status(wdma_id_t wdma_id, wdma_channel_t channel);

int isphw_wdma_stall(wdma_id_t wdma_id);
int isphw_wdma_get_bus_status(wdma_id_t wdma_id, dma_bus_status_t *bus_status);

int isphw_wdma_get_byte_cnt(wdma_id_t wdma_id, wdma_path_t path, uint32_t *cnt);

/*Do a global force update*/
int isphw_wdma_glb_cfg_upd(wdma_id_t wdma_id);

int isphw_wdma_wait_done(wdma_id_t wdma_id);




#ifdef __cplusplus
}
#endif





#endif
