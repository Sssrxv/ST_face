#ifndef _MIPI_TYPES_H_
#define _MIPI_TYPES_H_

#include <stdint.h>


typedef enum _mipi_id_t
{
    MIPI_ID_0   = 0,
    MIPI_ID_1   = 1,
    MIPI_ID_2   = 2,
    MIPI_ID_MAX
} mipi_id_t;


typedef enum _mipi_wdma_channel_t
{
    MIPI_WDMA_CHANNEL_MCH0,
    MIPI_WDMA_CHANNEL_MCH1,
    MIPI_WDMA_CHANNEL_MCH2,
    MIPI_WDMA_CHANNEL_MAX
} mipi_wdma_channel_t;


typedef struct _mipi_window_t
{
    uint32_t h_offset;
    uint32_t v_offset;
    uint32_t h_size;
    uint32_t v_size;
} mipi_window_t;


typedef struct _mipi_dma_buffer_t
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
} mipi_dma_buffer_t;


typedef struct _mipi_dma_picsize_t
{
    uint32_t width_a;
    uint32_t height_a;
    uint32_t llength_a;
    uint32_t picsize_a;

    uint32_t width_b;
    uint32_t height_b;
    uint32_t llength_b;
    uint32_t picsize_b;

    uint32_t width_c;
    uint32_t height_c;
    uint32_t llength_c;
    uint32_t picsize_c;
} mipi_dma_picsize_t;



typedef enum _mipi_dma_burst_length_t
{
    MIPI_DMA_BURST_LENGTH_1  = 0,
    MIPI_DMA_BURST_LENGTH_4  = 1,
    MIPI_DMA_BURST_LENGTH_8  = 2,
    MIPI_DMA_BURST_LENGTH_MAX
} mipi_dma_burst_length_t;


typedef enum _mipi_dma_dataformat_t
{
    MIPI_DMA_DATA_FORMAT_INVALID,
    MIPI_DMA_DATA_FORMAT_YUV422PLANAR,
    MIPI_DMA_DATA_FORMAT_YUV422SEMIPLANAR,
    MIPI_DMA_DATA_FORMAT_YUV422INTERLEAVED,
    MIPI_DMA_DATA_FORMAT_YUV420PLANAR,
    MIPI_DMA_DATA_FORMAT_YUV420SEMIPLANAR,
    MIPI_DMA_DATA_FORMAT_YUV444PLANAR,
    MIPI_DMA_DATA_FORMAT_YUV400,
    MIPI_DMA_DATA_FORMAT_RAW8,
    MIPI_DMA_DATA_FORMAT_RAW16,
    MIPI_DMA_DATA_FORMAT_MAX
} mipi_dma_dataformat_t;


typedef enum _mipi_dma_bus_status_t
{
    MIPI_DMA_BUS_STATUS_DIRTY = 0x0,
    MIPI_DMA_BUS_STATUS_CLEAN = 0x1
} mipi_dma_bus_status_t;


typedef enum _mipi_dma_leak_status_t
{
    MIPI_DMA_LEAK_STATUS_NO_LEAK = 0x0,
    MIPI_DMA_LEAK_STATUS_LEAKING = 0x1
} mipi_dma_leak_status_t;


typedef enum _mipi_intf_type_t
{
    MIPI_INTF_TYPE_RAW       = 0x0,
    MIPI_INTF_TYPE_656       = 0x1,
    MIPI_INTF_TYPE_601       = 0x2,
    MIPI_INTF_TYPE_BAYER_RGB = 0x3,
    MIPI_INTF_TYPE_DATA      = 0x4,
    MIPI_INTF_TYPE_RGB656    = 0x5,
    MIPI_INTF_TYPE_RAW656    = 0x6,
    MIPI_INTF_TYPE_MAX
} mipi_intf_type_t;


typedef enum _mipi_acq_input_selection_t
{
    MIPI_ACQ_INPUT_SELECTION_12BIT     = 0x0,
    MIPI_ACQ_INPUT_SELECTION_10BIT_ZZ  = 0x1,
    MIPI_ACQ_INPUT_SELECTION_10BIT_EX  = 0x2,
    MIPI_ACQ_INPUT_SELECTION_8BIT_ZZ   = 0x3,
    MIPI_ACQ_INPUT_SELECTION_8BIT_EX   = 0x4,
    MIPI_ACQ_INPUT_SELECTION_MAX
} mipi_acq_input_selection_t;


typedef enum _mipi_acq_field_selection_t
{
    MIPI_ACQ_FIELD_SELECTION_BOTH  = 0x0,
    MIPI_ACQ_FIELD_SELECTION_EVEN  = 0x1,
    MIPI_ACQ_FIELD_SELECTION_ODD   = 0x2,
    MIPI_ACQ_FIELD_SELECTION_MAX
} mipi_acq_field_selection_t;


typedef enum _mipi_acq_field_inv_t
{
    MIPI_ACQ_FIELD_INV_NOSWAP  = 0x0,
    MIPI_ACQ_FIELD_INV_SWAP    = 0x1,
    MIPI_ACQ_FIELD_INV_MAX
} mipi_acq_field_inv_t;


typedef enum _mipi_acq_ccir_seq_t
{
    MIPI_ACQ_CCIR_SEQ_YCBYCR  = 0x0,
    MIPI_ACQ_CCIR_SEQ_YCRYCB  = 0x1,
    MIPI_ACQ_CCIR_SEQ_CBYCRY  = 0x2,
    MIPI_ACQ_CCIR_SEQ_CRYCBY  = 0x3,
    MIPI_ACQ_CCIR_SEQ_MAX
} mipi_acq_ccir_seq_t;


typedef enum _mipi_acq_conv422_t
{
    MIPI_ACQ_CONV422_COSITED      = 0x0,
    MIPI_ACQ_CONV422_INTERLEAVED  = 0x1,
    MIPI_ACQ_CONV422_NONCOSITED   = 0x2,
    MIPI_ACQ_CONV422_MAX
} mipi_acq_conv422_t;

typedef enum _mipi_acq_bayer_pat_t
{
    MIPI_ACQ_BAYER_PAT_RGGB    = 0x0,
    MIPI_ACQ_BAYER_PAT_GRBG    = 0x1,
    MIPI_ACQ_BAYER_PAT_GBRG    = 0x2,
    MIPI_ACQ_BAYER_PAT_BGGR    = 0x3,
    MIPI_ACQ_BAYER_PAT_MAX
} mipi_acq_bayer_pat_t;


typedef enum _mipi_acq_vsync_pol_t
{
    MIPI_ACQ_VSYNC_POL_HIGH  = 0x0,
    MIPI_ACQ_VSYNC_POL_LOW   = 0x1,
    MIPI_ACQ_VSYNC_POL_MAX
} mipi_acq_vsync_pol_t;


typedef enum _mipi_acq_hsync_pol_t
{
    MIPI_ACQ_HSYNC_POL_HIGH  = 0x0,
    MIPI_ACQ_HSYNC_POL_LOW   = 0x1,
    MIPI_ACQ_HSYNC_POL_MAX
} mipi_acq_hsync_pol_t;


typedef enum _mipi_acq_sample_edge_t
{
    MIPI_ACQ_SAMPLE_EDGE_FALLING  = 0x0,
    MIPI_ACQ_SAMPLE_EDGE_RISING   = 0x1,
    MIPI_ACQ_SAMPLE_EDGE_MAX
} mipi_acq_sample_edge_t;


typedef enum _mipi_virtual_channel_t
{
    MIPI_VIRTUAL_CHANNEL_0 = 0x0, //!< virtual channel 0
    MIPI_VIRTUAL_CHANNEL_1 = 0x1, //!< virtual channel 1
    MIPI_VIRTUAL_CHANNEL_2 = 0x2, //!< virtual channel 2
    MIPI_VIRTUAL_CHANNEL_3 = 0x3, //!< virtual channel 3
    MIPI_VIRTUAL_CHANNEL_MAX
} mipi_virtual_channel_t;



typedef enum _mipi_data_type_t
{
    MIPI_DATA_TYPE_FSC              = 0x00, //!< frame start code
    MIPI_DATA_TYPE_FEC              = 0x01, //!< frame end code
    MIPI_DATA_TYPE_LSC              = 0x02, //!< line start code
    MIPI_DATA_TYPE_LEC              = 0x03, //!< line end code

    // 0x04 .. 0x07 reserved

    MIPI_DATA_TYPE_GSPC1            = 0x08, //!< generic short packet code 1
    MIPI_DATA_TYPE_GSPC2            = 0x09, //!< generic short packet code 2
    MIPI_DATA_TYPE_GSPC3            = 0x0A, //!< generic short packet code 3
    MIPI_DATA_TYPE_GSPC4            = 0x0B, //!< generic short packet code 4
    MIPI_DATA_TYPE_GSPC5            = 0x0C, //!< generic short packet code 5
    MIPI_DATA_TYPE_GSPC6            = 0x0D, //!< generic short packet code 6
    MIPI_DATA_TYPE_GSPC7            = 0x0E, //!< generic short packet code 7
    MIPI_DATA_TYPE_GSPC8            = 0x0F, //!< generic short packet code 8


    MIPI_DATA_TYPE_NULL             = 0x10, //!< null
    MIPI_DATA_TYPE_BLANKING         = 0x11, //!< blanking data
    MIPI_DATA_TYPE_EMBEDDED         = 0x12, //!< embedded 8-bit non image data

    // 0x13 .. 0x17 reserved
    
    MIPI_DATA_TYPE_YUV420_8         = 0x18, //!< YUV 420 8-Bit
    MIPI_DATA_TYPE_YUV420_10        = 0x19, //!< YUV 420 10-Bit
    MIPI_DATA_TYPE_LEGACY_YUV420_8  = 0x1A, //!< YUV 420 8-Bit
    //   0x1B reserved
    MIPI_DATA_TYPE_YUV420_8_CSPS    = 0x1C, //!< YUV 420 8-Bit (chroma shifted pixel sampling)
    MIPI_DATA_TYPE_YUV420_10_CSPS   = 0x1D, //!< YUV 420 10-Bit (chroma shifted pixel sampling)
    MIPI_DATA_TYPE_YUV422_8         = 0x1E, //!< YUV 422 8-Bit
    MIPI_DATA_TYPE_YUV422_10        = 0x1F, //!< YUV 422 10-Bit

    MIPI_DATA_TYPE_RGB444           = 0x20, //!< RGB444
    MIPI_DATA_TYPE_RGB555           = 0x21, //!< RGB555
    MIPI_DATA_TYPE_RGB565           = 0x22, //!< RGB565
    MIPI_DATA_TYPE_RGB666           = 0x23, //!< RGB666
    MIPI_DATA_TYPE_RGB888           = 0x24, //!< RGB888

    //   0x25 .. 0x27 reserved

    MIPI_DATA_TYPE_RAW_6            = 0x28, //!< RAW6
    MIPI_DATA_TYPE_RAW_7            = 0x29, //!< RAW7
    MIPI_DATA_TYPE_RAW_8            = 0x2A, //!< RAW8
    MIPI_DATA_TYPE_RAW_10           = 0x2B, //!< RAW10
    MIPI_DATA_TYPE_RAW_12           = 0x2C, //!< RAW12
    MIPI_DATA_TYPE_RAW_14           = 0x2D, //!< RAW14

    //   0x2E .. 0x2F reserved

    MIPI_DATA_TYPE_USER_1           = 0x30, //!< user defined 1
    MIPI_DATA_TYPE_USER_2           = 0x31, //!< user defined 2
    MIPI_DATA_TYPE_USER_3           = 0x32, //!< user defined 3
    MIPI_DATA_TYPE_USER_4           = 0x33, //!< user defined 4
    MIPI_DATA_TYPE_USER_5           = 0x34, //!< user defined 5
    MIPI_DATA_TYPE_USER_6           = 0x35, //!< user defined 6
    MIPI_DATA_TYPE_USER_7           = 0x36, //!< user defined 7
    MIPI_DATA_TYPE_USER_8           = 0x37, //!< user defined 8
    MIPI_DATA_TYPE_MAX
} mipi_data_type_t;


typedef enum _mipi_comp_scheme_t
{
    MIPI_COMP_SCHEME_NONE    = 0,   //!< NONE
    MIPI_COMP_SCHEME_12_8_12 = 1,   //!< 12_8_12
    MIPI_COMP_SCHEME_12_7_12 = 2,   //!< 12_7_12
    MIPI_COMP_SCHEME_12_6_12 = 3,   //!< 12_6_12
    MIPI_COMP_SCHEME_10_8_10 = 4,   //!< 10_8_10
    MIPI_COMP_SCHEME_10_7_10 = 5,   //!< 10_7_10
    MIPI_COMP_SCHEME_10_6_10 = 6,   //!< 10_6_10
    MIPI_COMP_SCHEME_MAX
} mipi_comp_scheme_t;



typedef enum _mipi_pred_block_t
{
    MIPI_PRED_BLOCK_INVALID = 0,   //!< invalid
    MIPI_PRED_BLOCK_1       = 1,   //!< Predictor1 (simple algorithm)
    MIPI_PRED_BLOCK_2       = 2,   //!< Predictor2 (more complex algorithm)
    MIPI_PRED_BLOCK_MAX
} mipi_pred_block_t;


#endif
