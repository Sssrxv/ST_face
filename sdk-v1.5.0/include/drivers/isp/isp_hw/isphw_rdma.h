#ifndef _ISPHW_RDMA_H_
#define _ISPHW_RDMA_H_


#include "isp_types.h"
#include "isphw_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


int isphw_rdma_set_buffer(const dma_buffer_t *dma_buffer);
int isphw_rdma_set_pic_size(dma_data_format_t data_format, const dma_pic_size_t *pic_size);
int isphw_rdma_set_data_format(dma_data_format_t data_format);
int isphw_rdma_set_burst_length(dma_burst_length_t burst_len);

int isphw_rdma_start(void);

int isphw_rdma_stall(void);
int isphw_rdma_get_bus_status(dma_bus_status_t *bus_status);

int isphw_rdma_cfg_upd(void);

int isphw_rdma_wait_done(void);




#ifdef __cplusplus
}
#endif





#endif
