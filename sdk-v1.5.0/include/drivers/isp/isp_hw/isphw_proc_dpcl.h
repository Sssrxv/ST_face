#ifndef _ISPHW_PROC_DPCL_H_
#define _ISPHW_PROC_DPCL_H_


#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _proc_dpcl_data_type_t
{
    PROC_DPCL_DATA_TYPE_YUV8   = 0x1,
    PROC_DPCL_DATA_TYPE_YUV10  = 0x2,
    PROC_DPCL_DATA_TYPE_RAW8   = 0x3,
    PROC_DPCL_DATA_TYPE_RAW10  = 0x4,
    PROC_DPCL_DATA_TYPE_RAW12  = 0x5,
    PROC_DPCL_DATA_TYPE_RAW14  = 0x6
}proc_dpcl_data_type_t;


typedef enum _proc_dpcl_mempkt_fmt_t
{
    PROC_DPCL_MEM_PKT_FMT_ISP1P1         = 0x0,
    PROC_DPCL_MEM_PKT_FMT_ISP2P0         = 0x1,
    PROC_DPCL_MEM_PKT_FMT_ANDROID        = 0x2,
    PROC_DPCL_MEM_PKT_FMT_RGBIR          = 0x3,
    PROC_DPCL_MEM_PKT_FMT_MAX
}proc_dpcl_mempkt_fmt_t;

int isphw_proc_dpcl_set_data_type(proc_dpcl_data_type_t data_type);
int isphw_proc_dpcl_set_mempkt_fmt(proc_dpcl_mempkt_fmt_t mempkt_fmt);
int isphw_proc_dpcl_enable_rgbir_swap(void);
int isphw_proc_dpcl_disable_rgbir_swap(void);



#ifdef __cplusplus
}
#endif



#endif
