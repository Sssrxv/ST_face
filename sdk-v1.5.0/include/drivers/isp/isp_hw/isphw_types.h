#ifndef _ISPHW_TYPES_H_
#define _ISPHW_TYPES_H_

#include "isp_types.h"

typedef struct _matrix_3x3_t
{
    uint32_t coeff[9];
}matrix_3x3_t;



typedef struct _matrix_1x3_t
{
    uint32_t coeff[3];
}matrix_1x3_t;


typedef struct _dma_buffer_t
{
    uint32_t buf_base_a_lo;
    uint32_t buf_base_a_hi;
    uint32_t buf_size_a;

    uint32_t buf_base_b_lo;
    uint32_t buf_base_b_hi;
    uint32_t buf_size_b;

    uint32_t buf_base_c_lo;
    uint32_t buf_base_c_hi;
    uint32_t buf_size_c;
}dma_buffer_t;


typedef struct _dma_pic_size_t
{
    uint32_t width_a;
    uint32_t height_a;
    uint32_t llength_a;
    uint32_t pic_size_a;

    uint32_t width_b;
    uint32_t height_b;
    uint32_t llength_b;
    uint32_t pic_size_b;

    uint32_t width_c;
    uint32_t height_c;
    uint32_t llength_c;
    uint32_t pic_size_c;
}dma_pic_size_t;



typedef enum _dma_burst_length_t
{
    DMA_BURST_LENGTH_1  = 0,
    DMA_BURST_LENGTH_4  = 1,
    DMA_BURST_LENGTH_8  = 2,
    DMA_BURST_LENGTH_MAX
}dma_burst_length_t;


typedef enum _dma_data_format_t
{
    DMA_DATA_FORMAT_INVALID,
    DMA_DATA_FORMAT_YUV422PLANAR,
    DMA_DATA_FORMAT_YUV422SEMIPLANAR,
    DMA_DATA_FORMAT_YUV422INTERLEAVED,
    DMA_DATA_FORMAT_YUV420PLANAR,
    DMA_DATA_FORMAT_YUV420SEMIPLANAR_NV12,
    DMA_DATA_FORMAT_YUV420SEMIPLANAR_NV21,
    DMA_DATA_FORMAT_YUV444PLANAR,
    DMA_DATA_FORMAT_YUV400,
    DMA_DATA_FORMAT_RAW8,
    DMA_DATA_FORMAT_RAW16,
    DMA_DATA_FORMAT_MAX
}dma_data_format_t;


typedef enum _dma_bus_status_t
{
    DMA_BUS_STATUS_DIRTY = 0x0,
    DMA_BUS_STATUS_CLEAN = 0x1
}dma_bus_status_t;


typedef enum _dma_leak_status_t
{
    DMA_LEAK_STATUS_NO_LEAK = 0x0,
    DMA_LEAK_STATUS_LEAKING = 0x1
}dma_leak_status_t;


typedef enum _intf_type_t
{
    INTF_TYPE_RAW       = 0x0,
    INTF_TYPE_656       = 0x1,
    INTF_TYPE_601       = 0x2,
    INTF_TYPE_BAYER_RGB = 0x3,
    INTF_TYPE_DATA      = 0x4,
    INTF_TYPE_RGB656    = 0x5,
    INTF_TYPE_RAW656    = 0x6,
    INTF_TYPE_MAX
}intf_type_t;

typedef enum _yc_range_type_t {
    YC_RANGE_CLIPPED = 0,
    YC_RANGE_FULL    = 1,
    YC_RANGE_MAX,
} yc_range_type_t;

typedef enum _acq_input_selection_t
{
    ACQ_INPUT_SELECTION_12BIT     = 0x0,
    ACQ_INPUT_SELECTION_10BIT_ZZ  = 0x1,
    ACQ_INPUT_SELECTION_10BIT_EX  = 0x2,
    ACQ_INPUT_SELECTION_8BIT_ZZ   = 0x3,
    ACQ_INPUT_SELECTION_8BIT_EX   = 0x4,
    ACQ_INPUT_SELECTION_MAX
}acq_input_selection_t;


typedef enum _acq_field_selection_t
{
    ACQ_FIELD_SELECTION_BOTH  = 0x0,
    ACQ_FIELD_SELECTION_EVEN  = 0x1,
    ACQ_FIELD_SELECTION_ODD   = 0x2,
    ACQ_FIELD_SELECTION_MAX
}acq_field_selection_t;


typedef enum _acq_field_inv_t
{
    ACQ_FIELD_INV_NOSWAP  = 0x0,
    ACQ_FIELD_INV_SWAP    = 0x1,
    ACQ_FIELD_INV_MAX
}acq_field_inv_t;


typedef enum _acq_ccir_seq_t
{
    ACQ_CCIR_SEQ_YCBYCR  = 0x0,
    ACQ_CCIR_SEQ_YCRYCB  = 0x1,
    ACQ_CCIR_SEQ_CBYCRY  = 0x2,
    ACQ_CCIR_SEQ_CRYCBY  = 0x3,
    ACQ_CCIR_SEQ_MAX
}acq_ccir_seq_t;


typedef enum _acq_conv422_t
{
    ACQ_CONV422_COSITED      = 0x0,
    ACQ_CONV422_INTERLEAVED  = 0x1,
    ACQ_CONV422_NONCOSITED   = 0x2,
    ACQ_CONV422_MAX
}acq_conv422_t;

typedef enum _acq_bayer_pat_t
{
    ACQ_BAYER_PAT_RGGB    = 0x0,
    ACQ_BAYER_PAT_GRBG    = 0x1,
    ACQ_BAYER_PAT_GBRG    = 0x2,
    ACQ_BAYER_PAT_BGGR    = 0x3,
    ACQ_BAYER_PAT_MAX
}acq_bayer_pat_t;


typedef enum _acq_vsync_pol_t
{
    ACQ_VSYNC_POL_HIGH  = 0x0,
    ACQ_VSYNC_POL_LOW   = 0x1,
    ACQ_VSYNC_POL_MAX
}acq_vsync_pol_t;


typedef enum _acq_hsync_pol_t
{
    ACQ_HSYNC_POL_HIGH  = 0x0,
    ACQ_HSYNC_POL_LOW   = 0x1,
    ACQ_HSYNC_POL_MAX
}acq_hsync_pol_t;


typedef enum _acq_sample_edge_t
{
    ACQ_SAMPLE_EDGE_FALLING  = 0x0,
    ACQ_SAMPLE_EDGE_RISING   = 0x1,
    ACQ_SAMPLE_EDGE_MAX
}acq_sample_edge_t;


#endif
