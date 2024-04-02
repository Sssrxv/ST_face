#ifndef _ISPHW_CLK_H_
#define _ISPHW_CLK_H_


#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


int isphw_disable_full_isp_core_clk(void);

int isphw_enable_isp_core_top_level_clk(void);

int isphw_enable_preproc_clk(void);
int isphw_disable_preproc_clk(void);

int isphw_enable_proc_clk(void);
int isphw_disable_proc_clk(void);

int isphw_enable_preview_path_clk(void);
int isphw_disable_preview_path_clk(void);

int isphw_enable_video_path_clk(void);
int isphw_disable_video_path_clk(void);

int isphw_enable_main_path_clk(void);
int isphw_disable_main_path_clk(void);

int isphw_enable_rdma_clk(void);
int isphw_disable_rdma_clk(void);

#ifdef __cplusplus
}
#endif



#endif
