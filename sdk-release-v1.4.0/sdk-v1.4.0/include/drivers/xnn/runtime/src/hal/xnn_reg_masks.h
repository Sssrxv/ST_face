#ifndef _XNN_REG_MASKS_H_
#define _XNN_REG_MASKS_H_ 

//----------------------------------------------------------------------
//  XNN CORE REGS
//----------------------------------------------------------------------

// mng_ctrl
#define REG_MNG_MUX_SHIFT               (0)
#define REG_MNG_MUX_MASK                (0x7 << REG_MNG_MUX_SHIFT)
#define REG_MNG_MUX(val)                (((val) << REG_MNG_MUX_SHIFT) & REG_MNG_MUX_MASK)

#define REG_MNG_MODE_SHIFT              (4)
#define REG_MNG_MODE_MASK               (0x7 << REG_MNG_MODE_SHIFT)
#define REG_MNG_MODE(val)               (((val) << REG_MNG_MODE_SHIFT) & REG_MNG_MODE_MASK)

// cnv_upd
#define REG_CNV_UPD_SHIFT               (0)
#define REG_CNV_UPD_MASK                (0x1 << REG_CNV_UPD_SHIFT)
#define REG_CNV_UPD(val)                (((val) << REG_CNV_UPD_SHIFT) & REG_CNV_UPD_MASK)

// cnv_ctrl
#define REG_CNV_EN_SHIFT                (0)
#define REG_CNV_EN_MASK                 (0x1 << REG_CNV_EN_SHIFT)
#define REG_CNV_EN(val)                 (((val) << REG_CNV_EN_SHIFT) & REG_CNV_EN_MASK)

#define REG_CNV_MODE_SHIFT              (4)
#define REG_CNV_MODE_MASK               (0x7 << REG_CNV_MODE_SHIFT)
#define REG_CNV_MODE(val)               (((val) << REG_CNV_MODE_SHIFT) & REG_CNV_MODE_MASK)

#define REG_CNV_SH_SHIFT                (8)
#define REG_CNV_SH_MASK                 (0xf << REG_CNV_SH_SHIFT)
#define REG_CNV_SH(val)                 (((val) << REG_CNV_SH_SHIFT) & REG_CNV_SH_MASK)

#define REG_CNV_SW_SWIFT                (12)
#define REG_CNV_SW_MASK                 (0xf << REG_CNV_SW_SWIFT)
#define REG_CNV_SW(val)                 (((val) << REG_CNV_SW_SWIFT) & REG_CNV_SW_MASK)

#define REG_CNV_LH_SHIFT                (16)
#define REG_CNV_LH_MASK                 (0xf << REG_CNV_LH_SHIFT)
#define REG_CNV_LH(val)                 (((val) << REG_CNV_LH_SHIFT) & REG_CNV_LH_MASK)

// cnv_h
#define REG_CNV_OH_SHIFT                (0)
#define REG_CNV_OH_MASK                 (0x1ff << REG_CNV_OH_SHIFT)
#define REG_CNV_OH(val)                 (((val) << REG_CNV_OH_SHIFT) & REG_CNV_OH_MASK)

#define REG_CNV_IH_SHIFT                (16)
#define REG_CNV_IH_MASK                 (0x1ff << REG_CNV_IH_SHIFT)
#define REG_CNV_IH(val)                 (((val) << REG_CNV_IH_SHIFT) & REG_CNV_IH_MASK)

// cnv_w
#define REG_CNV_OW_SHIFT                (0)
#define REG_CNV_OW_MASK                 (0x1ff << REG_CNV_OW_SHIFT)
#define REG_CNV_OW(val)                 (((val) << REG_CNV_OW_SHIFT) & REG_CNV_OW_MASK)

#define REG_CNV_IW_SHIFT                (16)
#define REG_CNV_IW_MASK                 (0x1ff << REG_CNV_IW_SHIFT)
#define REG_CNV_IW(val)                 (((val) << REG_CNV_IW_SHIFT) & REG_CNV_IW_MASK)

// cnv_c
#define REG_CNV_OC_SHIFT                (0)
#define REG_CNV_OC_MASK                 (0x1ff << REG_CNV_OC_SHIFT)
#define REG_CNV_OC(val)                 (((val) << REG_CNV_OC_SHIFT) & REG_CNV_OC_MASK)

#define REG_CNV_IC_SHIFT                (16)
#define REG_CNV_IC_MASK                 (0x1ff << REG_CNV_IC_SHIFT)
#define REG_CNV_IC(val)                 (((val) << REG_CNV_IC_SHIFT) & REG_CNV_IC_MASK)

// cnv_bh
#define REG_CNV_BU_SHIFT                (0)
#define REG_CNV_BU_MASK                 (0x3ff << REG_CNV_BU_SHIFT)
#define REG_CNV_BU(val)                 (((val) << REG_CNV_BU_SHIFT) & REG_CNV_BU_MASK)

#define REG_CNV_BD_SHIFT                (16)
#define REG_CNV_BD_MASK                 (0x3ff << REG_CNV_BD_SHIFT)
#define REG_CNV_BD(val)                 (((val) << REG_CNV_BD_SHIFT) & REG_CNV_BD_MASK)

// cnv_bw
#define REG_CNV_BL_SHIFT                (0)
#define REG_CNV_BL_MASK                 (0x3ff << REG_CNV_BL_SHIFT)
#define REG_CNV_BL(val)                 (((val) << REG_CNV_BL_SHIFT) & REG_CNV_BL_MASK)

#define REG_CNV_BR_SHIFT                (16)
#define REG_CNV_BR_MASK                 (0x3ff << REG_CNV_BR_SHIFT)
#define REG_CNV_BR(val)                 (((val) << REG_CNV_BR_SHIFT) & REG_CNV_BR_MASK)

// cnv_p0
#define REG_CNV_P0_SHIFT                (0)
#define REG_CNV_P0_MASK                 (0x7ff << REG_CNV_P0_SHIFT)
#define REG_CNV_P0(val)                 (((val) << REG_CNV_P0_SHIFT) & REG_CNV_P0_MASK)

#define REG_CNV_P1_SHIFT                (16)
#define REG_CNV_P1_MASK                 (0x7ff << REG_CNV_P1_SHIFT)
#define REG_CNV_P1(val)                 (((val) << REG_CNV_P1_SHIFT) & REG_CNV_P1_MASK)

// cnv_p1
#define REG_CNV_P2_SHIFT                (0)
#define REG_CNV_P2_MASK                 (0x7ff << REG_CNV_P2_SHIFT)
#define REG_CNV_P2(val)                 (((val) << REG_CNV_P2_SHIFT) & REG_CNV_P2_MASK)

#define REG_CNV_P3_SHIFT                (16)
#define REG_CNV_P3_MASK                 (0x7ff << REG_CNV_P3_SHIFT)
#define REG_CNV_P3(val)                 (((val) << REG_CNV_P3_SHIFT) & REG_CNV_P3_MASK)

// cnv_p2
#define REG_CNV_P4_SHIFT                (0)
#define REG_CNV_P4_MASK                 (0x7ff << REG_CNV_P4_SHIFT)
#define REG_CNV_P4(val)                 (((val) << REG_CNV_P4_SHIFT) & REG_CNV_P4_MASK)

#define REG_CNV_P5_SHIFT                (16)
#define REG_CNV_P5_MASK                 (0x7ff << REG_CNV_P5_SHIFT)
#define REG_CNV_P5(val)                 (((val) << REG_CNV_P5_SHIFT) & REG_CNV_P5_MASK)

// xtf_ctrl
#define REG_XTF_MODE_SHIFT              (0)
#define REG_XTF_MODE_MASK               (0x3 << REG_XTF_MODE_SHIFT)
#define REG_XTF_MODE(val)               (((val) << REG_XTF_MODE_SHIFT) & REG_XTF_MODE_MASK)

// bnr_ctrl
#define REG_BNR_MODE_SHIFT              (0)
#define REG_BNR_MODE_MASK               (0x7 << REG_BNR_MODE_SHIFT)
#define REG_BNR_MODE(val)               (((val) << REG_BNR_MODE_SHIFT) & REG_BNR_MODE_MASK)

#define REG_BNR_OFFS_SHIFT              (4)
#define REG_BNR_OFFS_MASK               (0x3f << REG_BNR_OFFS_SHIFT)
#define REG_BNR_OFFS(val)               (((val) << REG_BNR_OFFS_SHIFT) & REG_BNR_OFFS_MASK)

// bnr_p
#define REG_BNR_P0_SHIFT                (0)
#define REG_BNR_P0_MASK                 (0xffff << REG_BNR_P0_SHIFT)
#define REG_BNR_P0(val)                 (((val) << REG_BNR_P0_SHIFT) & REG_BNR_P0_MASK)

#define REG_BNR_P1_SHIFT                (16)
#define REG_BNR_P1_MASK                 (0xffff << REG_BNR_P1_SHIFT)
#define REG_BNR_P1(val)                 (((val) << REG_BNR_P1_SHIFT) & REG_BNR_P1_MASK)

// rlu_ctrl
#define REG_RLU_MODE_SHIFT              (0)
#define REG_RLU_MODE_MASK               (0x3 << REG_RLU_MODE_SHIFT)
#define REG_RLU_MODE(val)               (((val) << REG_RLU_MODE_SHIFT) & REG_RLU_MODE_MASK)

#define REG_RLU_OFFS_SHIFT              (4)
#define REG_RLU_OFFS_MASK               (0x3f << REG_RLU_OFFS_SHIFT)
#define REG_RLU_OFFS(val)               (((val) << REG_RLU_OFFS_SHIFT) & REG_RLU_OFFS_MASK)

// rlu_p
#define REG_RLU_P0_SHIFT                (0)
#define REG_RLU_P0_MASK                 (0xffff << REG_RLU_P0_SHIFT)
#define REG_RLU_P0(val)                 (((val) << REG_RLU_P0_SHIFT) & REG_RLU_P0_MASK)

#define REG_RLU_P1_SHIFT                (16)
#define REG_RLU_P1_MASK                 (0xffff << REG_RLU_P1_SHIFT)
#define REG_RLU_P1(val)                 (((val) << REG_RLU_P1_SHIFT) & REG_RLU_P1_MASK)


// qtz_ctrl
#define REG_QTZ_MODE_SHIFT              (0)
#define REG_QTZ_MODE_MASK               (0x3 << REG_QTZ_MODE_SHIFT)
#define REG_QTZ_MODE(val)               (((val) << REG_QTZ_MODE_SHIFT) & REG_QTZ_MODE_MASK)

#define REG_QTZ_P_SHIFT                 (16)
#define REG_QTZ_P_MASK                  (0xffff << REG_QTZ_P_SHIFT)
#define REG_QTZ_P(val)                  (((val) << REG_QTZ_P_SHIFT) & REG_QTZ_P_MASK)



//------------------------------------------------------------------------------
// XNN WR REGS
//------------------------------------------------------------------------------

// wr_enable
#define REG_WR_ENABLE_SHIFT             (0)
#define REG_WR_ENABLE_MASK              (0x1 << REG_WR_ENABLE_SHIFT)
#define REG_WR_ENABLE(val)              (((val) << REG_WR_ENABLE_SHIFT) & REG_WR_ENABLE_MASK)

// wr_cfg_upd
#define REG_WR_CFG_UPD_SHIFT            (0)
#define REG_WR_CFG_UPD_MASK             (0x1 << REG_WR_CFG_UPD_SHIFT)
#define REG_WR_CFG_UPD(val)             (((val) << REG_WR_CFG_UPD_SHIFT) & REG_WR_CFG_UPD_MASK)

// wr_gen_upd
#define REG_WR_GEN_UPD_SHIFT            (0)
#define REG_WR_GEN_UPD_MASK             (0x1 << REG_WR_GEN_UPD_SHIFT)
#define REG_WR_GEN_UPD(val)             (((val) << REG_WR_GEN_UPD_SHIFT) & REG_WR_GEN_UPD_MASK)

// according to guowei:
// xnn pipeline is 128 bit long, but read and write dma can be configured to handle
// different bit on one request
// if input is nf16 and output is int8, then read dma can read 8Data=16Bytes=128bit
// on one request, and write dma will also write 128bit out with 64 bit stride,
// since output is 8Data=8Bytes=64bit, so we can configure write dma
// to DMA_DATA_FORMAT_64BIT, then write dma will only write 64bit real data to memory
#define DMA_DATA_FORMAT_16BIT_PER_REQUEST            (0)
#define DMA_DATA_FORMAT_32BIT_PER_REQUEST            (1)
#define DMA_DATA_FORMAT_64BIT_PER_REQUEST            (2)
#define DMA_DATA_FORMAT_128BIT_PER_REQUEST           (3)

// wr_ctrl
#define REG_WR_DATA_FORMAT_SHIFT        (0)
#define REG_WR_DATA_FORMAT_MASK         (0x3 << REG_WR_DATA_FORMAT_SHIFT)
#define REG_WR_DATA_FORMAT(val)         (((val) << REG_WR_DATA_FORMAT_SHIFT) & REG_WR_DATA_FORMAT_MASK)

#define REG_WR_BURST_LEN_SHIFT          (16)
#define REG_WR_BURST_LEN_MASK           (0x3 << REG_WR_BURST_LEN_SHIFT)
#define REG_WR_BURST_LEN(val)           (((val) << REG_WR_BURST_LEN_SHIFT) & REG_WR_BURST_LEN_MASK)

// wr_x_stride
#define REG_WR_X_STRIDE_SHIFT           (0)
#define REG_WR_X_STRIDE_MASK            (0xfffffff0 << REG_WR_X_STRIDE_SHIFT)
#define REG_WR_X_STRIDE(val)            (((val) << REG_WR_X_STRIDE_SHIFT) & REG_WR_X_STRIDE_MASK)

// wr_y_stride
#define REG_WR_Y_STRIDE_SHIFT           (0)
#define REG_WR_Y_STRIDE_MASK            (0xfffffff0 << REG_WR_Y_STRIDE_SHIFT)
#define REG_WR_Y_STRIDE(val)            (((val) << REG_WR_Y_STRIDE_SHIFT) & REG_WR_Y_STRIDE_MASK)

// wr_y_addr
#define REG_WR_Y_ADDR_SHIFT             (0)
#define REG_WR_Y_ADDR_MASK              (0xfffffff0 << REG_WR_Y_ADDR_SHIFT)
#define REG_WR_Y_ADDR(val)              (((val) << REG_WR_Y_ADDR_SHIFT) & REG_WR_Y_ADDR_MASK)

// wr_enable_shd
#define REG_WR_ENABLE_SHD_SHIFT         (0)
#define REG_WR_ENABLE_SHD_MASK          (0x1 << REG_WR_ENABLE_SHD_SHIFT)
#define REG_WR_ENABLE_SHD(val)          (((val) << REG_WR_ENABLE_SHD_SHIFT) & REG_WR_ENABLE_SHD_MASK)

// wr_y_addr_shd
#define REG_WR_Y_ADDR_SHD_SHIFT         (0)
#define REG_WR_Y_ADDR_SHD_MASK          (0xfffffff0 << REG_WR_Y_ADDR_SHD_SHIFT)
#define REG_WR_Y_ADDR_SHD(val)          (((val) << REG_WR_Y_ADDR_SHD_SHIFT) & REG_WR_Y_ADDR_SHD_MASK)

// wr_dbg_state
#define REG_WR_DBG_HOLD_LOCKED_SHIFT    (0)
#define REG_WR_DBG_HOLD_LOCKED_MASK     (0x1 << REG_WR_DBG_HOLD_LOCKED_SHIFT)
#define REG_WR_DBG_HOLD_LOCKED(val)     (((val) << REG_WR_DBG_HOLD_LOCKED_SHIFT) & REG_WR_DBG_HOLD_LOCKED_MASK)

#define REG_WR_DBG_UPD_LOCKED_SHIFT     (4)
#define REG_WR_DBG_UPD_LOCKED_MASK      (0x1 << REG_WR_DBG_UPD_LOCKED_SHIFT)
#define REG_WR_DBG_UPD_LOCKED(val)      (((val) << REG_WR_DBG_UPD_LOCKED_SHIFT) & REG_WR_DBG_UPD_LOCKED_MASK)

#define REG_WR_DBG_CTRL_STATE_SHIFT     (8)
#define REG_WR_DBG_CTRL_STATE_MASK      (0x3 << REG_WR_DBG_CTRL_STATE_SHIFT)
#define REG_WR_DBG_CTRL_STATE(val)      (((val) << REG_WR_DBG_CTRL_STATE_SHIFT) & REG_WR_DBG_CTRL_STATE_MASK)

#define REG_WR_DBG_BVCI_STATE_SHIFT     (12)
#define REG_WR_DBG_BVCI_STATE_MASK      (0x3 << REG_WR_DBG_BVCI_STATE_SHIFT)
#define REG_WR_DBG_BVCI_STATE(val)      (((val) << REG_WR_DBG_BVCI_STATE_SHIFT) & REG_WR_DBG_BVCI_STATE_MASK)

#define REG_WR_DBG_Y_FIFO_CNT_SHIFT     (16)
#define REG_WR_DBG_Y_FIFO_CNT_MASK      (0x3f << REG_WR_DBG_Y_FIFO_CNT_SHIFT)
#define REG_WR_DBG_Y_FIFO_CNT(val)      (((val) << REG_WR_DBG_Y_FIFO_CNT_SHIFT) & REG_WR_DBG_Y_FIFO_CNT_MASK)

// wr_dbg_y_ptr
#define REG_WR_DBG_Y_PTR_SHIFT          (0)
#define REG_WR_DBG_Y_PTR_MASK           (0xfffffff0 << REG_WR_DBG_Y_PTR_SHIFT)
#define REG_WR_DBG_Y_PTR(val)           (((val) << REG_WR_DBG_Y_PTR_SHIFT) & REG_WR_DBG_Y_PTR_MASK)

// wr_dbg_cmd_cnt
#define REG_WR_DBG_CMD_CNT_SHIFT        (0)
#define REG_WR_DBG_CMD_CNT_MASK         (0xfffffff0 << REG_WR_DBG_CMD_CNT_SHIFT)
#define REG_WR_DBG_CMD_CNT(val)         (((val) << REG_WR_DBG_CMD_CNT_SHIFT) & REG_WR_DBG_CMD_CNT_MASK)

// wr_dbg_rsp_cnt
#define REG_WR_DBG_RSP_CNT_SHIFT        (0)
#define REG_WR_DBG_RSP_CNT_MASK         (0xfffffff0 << REG_WR_DBG_RSP_CNT_SHIFT)
#define REG_WR_DBG_RSP_CNT(val)         (((val) << REG_WR_DBG_RSP_CNT_SHIFT) & REG_WR_DBG_RSP_CNT_MASK)

// wr_imsc
#define REG_WR_IMSC_SHIFT               (0)
#define REG_WR_IMSC_MASK                (0xffffffff << REG_WR_IMSC_SHIFT)
#define REG_WR_IMSC(val)                (((val) << REG_WR_IMSC_SHIFT) & REG_WR_IMSC_MASK)

// wr_ris
#define REG_WR_RIS_SHIFT                (0)
#define REG_WR_RIS_MASK                 (0xffffffff << REG_WR_RIS_SHIFT)
#define REG_WR_RIS(val)                 (((val) << REG_WR_RIS_SHIFT) & REG_WR_RIS_MASK)

// wr_icr
#define REG_WR_ICR_SHIFT                (0)
#define REG_WR_ICR_MASK                 (0xffffffff << REG_WR_ICR_SHIFT)
#define REG_WR_ICR(val)                 (((val) << REG_WR_ICR_SHIFT) & REG_WR_ICR_MASK)

// wr_mis
#define REG_WR_MIS_SHIFT                (0)
#define REG_WR_MIS_MASK                 (0xffffffff << REG_WR_MIS_SHIFT)
#define REG_WR_MIS(val)                 (((val) << REG_WR_MIS_SHIFT) & REG_WR_MIS_MASK)

// wr_lean_en
#define REG_WR_LEAK_EN_SHIFT            (0)
#define REG_WR_LEAK_EN_MASK             (0x1 << REG_WR_LEAK_EN_SHIFT)
#define REG_WR_LEAK_EN(val)             (((val) << REG_WR_LEAK_EN_SHIFT) & REG_WR_LEAK_EN_MASK)

// wr_sw_leak
#define REG_WR_SW_LEAK_SHIFT            (0)
#define REG_WR_SW_LEAK_MASK             (0x1 << REG_WR_SW_LEAK_SHIFT)
#define REG_WR_SW_LEAK(val)             (((val) << REG_WR_SW_LEAK_SHIFT) & REG_WR_SW_LEAK_MASK)


//------------------------------------------------------------------------------
// XNN RD REGS
//------------------------------------------------------------------------------

// rd_enable
#define REG_RD_ENABLE_SHIFT             (0)
#define REG_RD_ENABLE_MASK              (0x1 << REG_RD_ENABLE_SHIFT)
#define REG_RD_ENABLE(val)              (((val) << REG_RD_ENABLE_SHIFT) & REG_RD_ENABLE_MASK)

// rd_sw_start
#define REG_RD_SW_START_SHIFT           (0)
#define REG_RD_SW_START_MASK            (0x1 << REG_RD_SW_START_SHIFT)
#define REG_RD_SW_START(val)            (((val) << REG_RD_SW_START_SHIFT) & REG_RD_SW_START_MASK)

// rd_gen_upd
#define REG_RD_GEN_UPD_SHIFT            (0)
#define REG_RD_GEN_UPD_MASK             (0x1 << REG_RD_GEN_UPD_SHIFT)
#define REG_RD_GEN_UPD(val)             (((val) << REG_RD_GEN_UPD_SHIFT) & REG_RD_GEN_UPD_MASK)

// rd_ctrl
#define REG_RD_DATA_FORMAT_SHIFT        (0)
#define REG_RD_DATA_FORMAT_MASK         (0x3 << REG_RD_DATA_FORMAT_SHIFT)
#define REG_RD_DATA_FORMAT(val)         (((val) << REG_RD_DATA_FORMAT_SHIFT) & REG_RD_DATA_FORMAT_MASK)

#define REG_RD_BURST_LEN_SHIFT          (16)
#define REG_RD_BURST_LEN_MASK           (0x3 << REG_RD_BURST_LEN_SHIFT)
#define REG_RD_BURST_LEN(val)           (((val) << REG_RD_BURST_LEN_SHIFT) & REG_RD_BURST_LEN_MASK)

#define REG_RD_X_SKIP_SHIFT             (24)
#define REG_RD_X_SKIP_MASK              (0x1 << REG_RD_X_SKIP_SHIFT)
#define REG_RD_X_SKIP(val)              (((val) << REG_RD_X_SKIP_SHIFT) & REG_RD_X_SKIP_MASK)

// rd_x_length
#define REG_RD_X_LENGTH_SHIFT           (0)
#define REG_RD_X_LENGTH_MASK            (0xffffffff << REG_RD_X_LENGTH_SHIFT)
#define REG_RD_X_LENGTH(val)            (((val) << REG_RD_X_LENGTH_SHIFT) & REG_RD_X_LENGTH_MASK)

// rd_y_length
#define REG_RD_Y_LENGTH_SHIFT           (0)
#define REG_RD_Y_LENGTH_MASK            (0xffffffff << REG_RD_Y_LENGTH_SHIFT)
#define REG_RD_Y_LENGTH(val)            (((val) << REG_RD_Y_LENGTH_SHIFT) & REG_RD_Y_LENGTH_MASK)

// rd_z_length
#define REG_RD_Z_LENGTH_SHIFT           (0)
#define REG_RD_Z_LENGTH_MASK            (0xffffffff << REG_RD_Z_LENGTH_SHIFT)
#define REG_RD_Z_LENGTH(val)            (((val) << REG_RD_Z_LENGTH_SHIFT) & REG_RD_Z_LENGTH_MASK)

// rd_x_stride
#define REG_RD_X_STRIDE_SHIFT           (0)
#define REG_RD_X_STRIDE_MASK            (0xfffffff0 << REG_RD_X_STRIDE_SHIFT)
#define REG_RD_X_STRIDE(val)            (((val) << REG_RD_X_STRIDE_SHIFT) & REG_RD_X_STRIDE_MASK)

// rd_y_stride
#define REG_RD_Y_STRIDE_SHIFT           (0)
#define REG_RD_Y_STRIDE_MASK            (0xfffffff0 << REG_RD_Y_STRIDE_SHIFT)
#define REG_RD_Y_STRIDE(val)            (((val) << REG_RD_Y_STRIDE_SHIFT) & REG_RD_Y_STRIDE_MASK)

// rd_y_addr
#define REG_RD_Y_ADDR_SHIFT             (0)
#define REG_RD_Y_ADDR_MASK              (0xfffffff0 << REG_RD_Y_ADDR_SHIFT)
#define REG_RD_Y_ADDR(val)              (((val) << REG_RD_Y_ADDR_SHIFT) & REG_RD_Y_ADDR_MASK)

// rd_dbg_state
#define REG_RD_DBG_CTRL_STATE_SHIFT     (8)
#define REG_RD_DBG_CTRL_STATE_MASK      (0x7 << REG_RD_DBG_CTRL_STATE_SHIFT)
#define REG_RD_DBG_CTRL_STATE(val)      (((val) << REG_RD_DBG_CTRL_STATE_SHIFT) & REG_RD_DBG_CTRL_STATE_MASK)

// rd_dbg_y_ptr
#define REG_RD_DBG_Y_PTR_SHIFT          (0)
#define REG_RD_DBG_Y_PTR_MASK           (0xfffffff0 << REG_RD_DBG_Y_PTR_SHIFT)
#define REG_RD_DBG_Y_PTR(val)           (((val) << REG_RD_DBG_Y_PTR_SHIFT) & REG_RD_DBG_Y_PTR_MASK)

// rd_dbg_cmd_cnt
#define REG_RD_DBG_CMD_CNT_SHIFT        (0)
#define REG_RD_DBG_CMD_CNT_MASK         (0xfffffff0 << REG_RD_DBG_CMD_CNT_SHIFT)
#define REG_RD_DBG_CMD_CNT(val)         (((val) << REG_RD_DBG_CMD_CNT_SHIFT) & REG_RD_DBG_CMD_CNT_MASK)

// rd_dbg_rsp_cnt
#define REG_RD_DBG_RSP_CNT_SHIFT        (0)
#define REG_RD_DBG_RSP_CNT_MASK         (0xfffffff0 << REG_RD_DBG_RSP_CNT_SHIFT)
#define REG_RD_DBG_RSP_CNT(val)         (((val) << REG_RD_DBG_RSP_CNT_SHIFT) & REG_RD_DBG_RSP_CNT_MASK)

// rd_dbg_fifo
#define REG_RD_DBG_FIFO_SHIFT           (0)
#define REG_RD_DBG_FIFO_MASK            (0x3f << REG_RD_DBG_FIFO_SHIFT)
#define REG_RD_DBG_FIFO(val)            (((val) << REG_RD_DBG_FIFO_SHIFT) & REG_RD_DBG_FIFO_MASK)

// rd_imsc
#define REG_RD_IMSC_SHIFT               (0)
#define REG_RD_IMSC_MASK                (0xffffffff << REG_RD_IMSC_SHIFT)
#define REG_RD_IMSC(val)                (((val) << REG_RD_IMSC_SHIFT) & REG_RD_IMSC_MASK)

// rd_ris
#define REG_RD_RIS_SHIFT                (0)
#define REG_RD_RIS_MASK                 (0xffffffff << REG_RD_RIS_SHIFT)
#define REG_RD_RIS(val)                 (((val) << REG_RD_RIS_SHIFT) & REG_RD_RIS_MASK)

// rd_icr
#define REG_RD_ICR_SHIFT                (0)
#define REG_RD_ICR_MASK                 (0xffffffff << REG_RD_ICR_SHIFT)
#define REG_RD_ICR(val)                 (((val) << REG_RD_ICR_SHIFT) & REG_RD_ICR_MASK)

// rd_mis
#define REG_RD_MIS_SHIFT                (0)
#define REG_RD_MIS_MASK                 (0xffffffff << REG_RD_MIS_SHIFT)
#define REG_RD_MIS(val)                 (((val) << REG_RD_MIS_SHIFT) & REG_RD_MIS_MASK)

// rd_lean_en
#define REG_RD_LEAK_EN_SHIFT            (0)
#define REG_RD_LEAK_EN_MASK             (0x1 << REG_RD_LEAK_EN_SHIFT)
#define REG_RD_LEAK_EN(val)             (((val) << REG_RD_LEAK_EN_SHIFT) & REG_RD_LEAK_EN_MASK)

// rd_sw_leak
#define REG_RD_SW_LEAK_SHIFT            (0)
#define REG_RD_SW_LEAK_MASK             (0x1 << REG_RD_SW_LEAK_SHIFT)
#define REG_RD_SW_LEAK(val)             (((val) << REG_RD_SW_LEAK_SHIFT) & REG_RD_SW_LEAK_MASK)


//------------------------------------------------------------------------------
// XNN RBC REGS
//------------------------------------------------------------------------------

// rbc_timer_en
#define REG_RBC_TIMER_EN_SHIFT          (0)
#define REG_RBC_TIMER_EN_MASK           (0x1 << REG_RBC_TIMER_EN_SHIFT)
#define REG_RBC_TIMER_EN(val)           (((val) << REG_RBC_TIMER_EN_SHIFT) & REG_RBC_TIMER_EN_MASK)

// rbc_timer_thd
#define REG_RBC_TIMER_THD_SHIFT         (0)
#define REG_RBC_TIMER_THD_MASK          (0xffffffff << REG_RBC_TIMER_THD_SHIFT)
#define REG_RBC_TIMER_THD(val)          (((val) << REG_RBC_TIMER_THD_SHIFT) & REG_RBC_TIMER_THD_MASK)

// rbc_wait_thd
#define REG_RBC_WAIT_THD_SHIFT          (0)
#define REG_RBC_WAIT_THD_MASK           (0xffffffff << REG_RBC_WAIT_THD_SHIFT)
#define REG_RBC_WAIT_THD(val)           (((val) << REG_RBC_WAIT_THD_SHIFT) & REG_RBC_WAIT_THD_MASK)

// rbc_dbg_cmd_cnt
#define REG_RBC_DBG_CMD_CNT_SHIFT       (0)
#define REG_RBC_DBG_CMD_CNT_MASK        (0xffffffff << REG_RBC_DBG_CMD_CNT_SHIFT)
#define REG_RBC_DBG_CMD_CNT(val)        (((val) << REG_RBC_DBG_CMD_CNT_SHIFT) & REG_RBC_DBG_CMD_CNT_MASK)

// rbc_dbg_cmd_hi
#define REG_RBC_DBG_CMD_HI_SHIFT        (0)
#define REG_RBC_DBG_CMD_HI_MASK         (0xffffffff << REG_RBC_DBG_CMD_HI_SHIFT)
#define REG_RBC_DBG_CMD_HI(val)         (((val) << REG_RBC_DBG_CMD_HI_SHIFT) & REG_RBC_DBG_CMD_HI_MASK)

// rbc_dbg_cmd_lo
#define REG_RBC_DBG_CMD_LO_SHIFT        (0)
#define REG_RBC_DBG_CMD_LO_MASK         (0xffffffff << REG_RBC_DBG_CMD_LO_SHIFT)
#define REG_RBC_DBG_CMD_LO(val)         (((val) << REG_RBC_DBG_CMD_LO_SHIFT) & REG_RBC_DBG_CMD_LO_MASK)

// rbc_imsc
#define REG_RBC_IMSC_SHIFT              (0)
#define REG_RBC_IMSC_MASK               (0xffffffff << REG_RBC_IMSC_SHIFT)
#define REG_RBC_IMSC(val)               (((val) << REG_RBC_IMSC_SHIFT) & REG_RBC_IMSC_MASK)

// rbc_ris
#define REG_RBC_RIS_SHIFT               (0)
#define REG_RBC_RIS_MASK                (0xffffffff << REG_RBC_RIS_SHIFT)
#define REG_RBC_RIS(val)                (((val) << REG_RBC_RIS_SHIFT) & REG_RBC_RIS_MASK)

// rbc_icr
#define REG_RBC_ICR_SHIFT               (0)
#define REG_RBC_ICR_MASK                (0xffffffff << REG_RBC_ICR_SHIFT)
#define REG_RBC_ICR(val)                (((val) << REG_RBC_ICR_SHIFT) & REG_RBC_ICR_MASK)

// rbc_mis
#define REG_RBC_MIS_SHIFT               (0)
#define REG_RBC_MIS_MASK                (0xffffffff << REG_RBC_MIS_SHIFT)
#define REG_RBC_MIS(val)                (((val) << REG_RBC_MIS_SHIFT) & REG_RBC_MIS_MASK)

#endif

