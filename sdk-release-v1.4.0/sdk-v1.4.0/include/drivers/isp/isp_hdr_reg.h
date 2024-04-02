#ifndef _ISP_HDR_REG_H_
#define _ISP_HDR_REG_H_

//----------------------------------------------------------------------
//  HDR CORE REGS
//----------------------------------------------------------------------
// mux_ctrl
#define REG_MUX_SHARPEN_EN_SHIFT        (0)
#define REG_MUX_SHARPEN_EN_MASK         (0x1 << REG_MUX_SHARPEN_EN_SHIFT)
#define REG_MUX_SHARPEN_EN(val)         (((val) << REG_MUX_SHARPEN_EN_SHIFT) & REG_MUX_SHARPEN_EN_MASK)
#define REG_MUX_U2D0_EN_SHIFT           (8)
#define REG_MUX_U2D0_EN_MASK            (0x1 << REG_MUX_U2D0_EN_SHIFT)
#define REG_MUX_U2D0_EN(val)            (((val) << REG_MUX_U2D0_EN_SHIFT) & REG_MUX_U2D0_EN_MASK)
#define REG_MUX_U2D1_EN_SHIFT           (9)
#define REG_MUX_U2D1_EN_MASK            (0x1 << REG_MUX_U2D1_EN_SHIFT)
#define REG_MUX_U2D1_EN(val)            (((val) << REG_MUX_U2D1_EN_SHIFT) & REG_MUX_U2D1_EN_MASK)
#define REG_MUX_U2D2_EN_SHIFT           (10)
#define REG_MUX_U2D2_EN_MASK            (0x1 << REG_MUX_U2D2_EN_SHIFT)
#define REG_MUX_U2D2_EN(val)            (((val) << REG_MUX_U2D2_EN_SHIFT) & REG_MUX_U2D2_EN_MASK)
#define REG_MUX_L2R0_EN_SHIFT           (11)
#define REG_MUX_L2R0_EN_MASK            (0x1 << REG_MUX_L2R0_EN_SHIFT)
#define REG_MUX_L2R0_EN(val)            (((val) << REG_MUX_L2R0_EN_SHIFT) & REG_MUX_L2R0_EN_MASK)
#define REG_MUX_L2R1_EN_SHIFT           (12)
#define REG_MUX_L2R1_EN_MASK            (0x1 << REG_MUX_L2R1_EN_SHIFT)
#define REG_MUX_L2R1_EN(val)            (((val) << REG_MUX_L2R1_EN_SHIFT) & REG_MUX_L2R1_EN_MASK)
#define REG_MUX_R2L0_EN_SHIFT           (13)
#define REG_MUX_R2L0_EN_MASK            (0x1 << REG_MUX_R2L0_EN_SHIFT)
#define REG_MUX_R2L0_EN(val)            (((val) << REG_MUX_R2L0_EN_SHIFT) & REG_MUX_R2L0_EN_MASK)
#define REG_MUX_BLEND_EN_SHIFT          (16)
#define REG_MUX_BLEND_EN_MASK           (0x1 << REG_MUX_BLEND_EN_SHIFT)
#define REG_MUX_BLEND_EN(val)           (((val) << REG_MUX_BLEND_EN_SHIFT) & REG_MUX_BLEND_EN_MASK)
#define REG_MUX_BLEND_S01_EN_SHIFT      (17)
#define REG_MUX_BLEND_S01_EN_MASK       (0x1 << REG_MUX_BLEND_S01_EN_SHIFT)
#define REG_MUX_BLEND_S01_EN(val)       (((val) << REG_MUX_BLEND_S01_EN_SHIFT) & REG_MUX_BLEND_S01_EN_MASK)

// sharpen_strength
#define REG_SHARPEN_STRENGTH_SHIFT      (0)
#define REG_SHARPEN_STRENGTH_MASK       (0x7 << REG_SHARPEN_STRENGTH_SHIFT)
#define REG_SHARPEN_STRENGTH(val)       (((val) << REG_SHARPEN_STRENGTH_SHIFT) & REG_SHARPEN_STRENGTH_MASK)

// sigm_lut0_dat
#define REG_SIGM_LUT0_DAT_SHIFT         (0)
#define REG_SIGM_LUT0_DAT_MASK          (0xFF << REG_SIGM_LUT0_DAT_SHIFT)
#define REG_SIGM_LUT0_DAT(val)          (((val) << REG_SIGM_LUT0_DAT_SHIFT) & REG_SIGM_LUT0_DAT_MASK)

// sigm_lut0_idx
#define REG_SIGM_LUT0_IDX_SHIFT         (0)
#define REG_SIGM_LUT0_IDX_MASK          (0xFF << REG_SIGM_LUT0_IDX_SHIFT)
#define REG_SIGM_LUT0_IDX(val)          (((val) << REG_SIGM_LUT0_IDX_SHIFT) & REG_SIGM_LUT0_IDX_MASK)

// sigm_lut1_dat
#define REG_SIGM_LUT1_DAT_SHIFT         (0)
#define REG_SIGM_LUT1_DAT_MASK          (0xFF << REG_SIGM_LUT1_DAT_SHIFT)
#define REG_SIGM_LUT1_DAT(val)          (((val) << REG_SIGM_LUT1_DAT_SHIFT) & REG_SIGM_LUT1_DAT_MASK)

// sigm_lut1_idx
#define REG_SIGM_LUT1_IDX_SHIFT         (0)
#define REG_SIGM_LUT1_IDX_MASK          (0xFF << REG_SIGM_LUT1_IDX_SHIFT)
#define REG_SIGM_LUT1_IDX(val)          (((val) << REG_SIGM_LUT1_IDX_SHIFT) & REG_SIGM_LUT1_IDX_MASK)


// wgt_lut0_dat
#define REG_WGT_LUT0_DAT_SHIFT          (0)
#define REG_WGT_LUT0_DAT_MASK           (0x7F << REG_WGT_LUT0_DAT_SHIFT)
#define REG_WGT_LUT0_DAT(val)           (((val) << REG_WGT_LUT0_DAT_SHIFT) & REG_WGT_LUT0_DAT_MASK)

// wgt_lut0_idx
#define REG_WGT_LUT0_IDX_SHIFT          (0)
#define REG_WGT_LUT0_IDX_MASK           (0xFF << REG_WGT_LUT0_IDX_SHIFT)
#define REG_WGT_LUT0_IDX(val)           (((val) << REG_WGT_LUT0_IDX_SHIFT) & REG_WGT_LUT0_IDX_MASK)

// wgt_lut1_dat
#define REG_WGT_LUT1_DAT_SHIFT          (0)
#define REG_WGT_LUT1_DAT_MASK           (0x7F << REG_WGT_LUT1_DAT_SHIFT)
#define REG_WGT_LUT1_DAT(val)           (((val) << REG_WGT_LUT1_DAT_SHIFT) & REG_WGT_LUT1_DAT_MASK)

// wgt_lut1_idx
#define REG_WGT_LUT1_IDX_SHIFT          (0)
#define REG_WGT_LUT1_IDX_MASK           (0xFF << REG_WGT_LUT1_IDX_SHIFT)
#define REG_WGT_LUT1_IDX(val)           (((val) << REG_WGT_LUT1_IDX_SHIFT) & REG_WGT_LUT1_IDX_MASK)

// hdr_hsize
#define REG_HDR_HSIZE_SHIFT             (0)
#define REG_HDR_HSIZE_MASK              (0xFFF << REG_HDR_HSIZE_SHIFT)
#define REG_HDR_HSIZE(val)              (((val) << REG_HDR_HSIZE_SHIFT) & REG_HDR_HSIZE_MASK)

//----------------------------------------------------------------------
//  HDR MIR0 REGS
//----------------------------------------------------------------------

// mir0_enable
#define REG_MIR0_CONT_ENABLE_SHIFT      (0)
#define REG_MIR0_CONT_ENABLE_MASK       (0x1 << REG_MIR0_CONT_ENABLE_SHIFT)
#define REG_MIR0_CONT_ENABLE(val)       (((val) << REG_MIR0_CONT_ENABLE_SHIFT) & REG_MIR0_CONT_ENABLE_MASK)

// mir0_sw_start
#define REG_MIR0_SW_START_SHIFT         (0)
#define REG_MIR0_SW_START_MASK          (0x1 << REG_MIR0_SW_START_SHIFT)
#define REG_MIR0_SW_START(val)          (((val) << REG_MIR0_SW_START_SHIFT) & REG_MIR0_SW_START_MASK)

// mir0_gen_upd
#define REG_MIR0_GEN_UPD_SHIFT          (0)
#define REG_MIR0_GEN_UPD_MASK           (0x1 << REG_MIR0_GEN_UPD_SHIFT)
#define REG_MIR0_GEN_UPD(val)           (((val) << REG_MIR0_GEN_UPD_SHIFT) & REG_MIR0_GEN_UPD_MASK)

// mir0_ctrl
#define REG_MIR0_DATA_FORMAT_SHIFT      (0)
#define REG_MIR0_DATA_FORMAT_MASK       (0x7 << REG_MIR0_DATA_FORMAT_SHIFT)
#define REG_MIR0_DATA_FORMAT(val)       (((val) << REG_MIR0_DATA_FORMAT_SHIFT) & REG_MIR0_DATA_FORMAT_MASK)

#define REG_MIR0_BURST_LEN_SHIFT        (16)
#define REG_MIR0_BURST_LEN_MASK         (0x3 << REG_MIR0_BURST_LEN_SHIFT)
#define REG_MIR0_BURST_LEN(val)         (((val) << REG_MIR0_BURST_LEN_SHIFT) & REG_MIR0_BURST_LEN_MASK)

#define REG_MIR0_DATA_SWAP_SHIFT        (20)
#define REG_MIR0_DATA_SWAP_MASK         (0x3 << REG_MIR0_DATA_SWAP_SHIFT)
#define REG_MIR0_DATA_SWAP(val)         (((val) << REG_MIR0_DATA_SWAP_SHIFT) & REG_MIR0_DATA_SWAP_MASK)

// mir0_x_length
#define REG_MIR0_X_LENGTH_SHIFT         (0)
#define REG_MIR0_X_LENGTH_MASK          (0xFFFFFFFF << REG_MIR0_X_LENGTH_SHIFT)
#define REG_MIR0_X_LENGTH(val)          (((val) << REG_MIR0_X_LENGTH_SHIFT) & REG_MIR0_X_LENGTH_MASK)

// mir0_y_length
#define REG_MIR0_Y_LENGTH_SHIFT         (0)
#define REG_MIR0_Y_LENGTH_MASK          (0xFFFFFFFF << REG_MIR0_Y_LENGTH_SHIFT)
#define REG_MIR0_Y_LENGTH(val)          (((val) << REG_MIR0_Y_LENGTH_SHIFT) & REG_MIR0_Y_LENGTH_MASK)

// mir0_z_length
#define REG_MIR0_Z_LENGTH_SHIFT         (0)
#define REG_MIR0_Z_LENGTH_MASK          (0xFFFFFFFF << REG_MIR0_Z_LENGTH_SHIFT)
#define REG_MIR0_Z_LENGTH(val)          (((val) << REG_MIR0_Z_LENGTH_SHIFT) & REG_MIR0_Z_LENGTH_MASK)

// mir0_x_stride
#define REG_MIR0_X_STRIDE_SHIFT         (0)
#define REG_MIR0_X_STRIDE_MASK          (0xFFFFFFF8 << REG_MIR0_X_STRIDE_SHIFT)
#define REG_MIR0_X_STRIDE(val)          (((val) << REG_MIR0_X_STRIDE_SHIFT) & REG_MIR0_X_STRIDE_MASK)

// mir0_y_stride
#define REG_MIR0_Y_STRIDE_SHIFT         (0)
#define REG_MIR0_Y_STRIDE_MASK          (0xFFFFFFF8 << REG_MIR0_Y_STRIDE_SHIFT)
#define REG_MIR0_Y_STRIDE(val)          (((val) << REG_MIR0_Y_STRIDE_SHIFT) & REG_MIR0_Y_STRIDE_MASK)

// mir0_y_addr
#define REG_MIR0_Y_ADDR_SHIFT           (0)
#define REG_MIR0_Y_ADDR_MASK            (0xFFFFFFFF << REG_MIR0_Y_ADDR_SHIFT)
#define REG_MIR0_Y_ADDR(val)            (((val) << REG_MIR0_Y_ADDR_SHIFT) & REG_MIR0_Y_ADDR_MASK)

// mir0_c_addr
#define REG_MIR0_C_ADDR_SHIFT           (0)
#define REG_MIR0_C_ADDR_MASK            (0xFFFFFFFF << REG_MIR0_C_ADDR_SHIFT)
#define REG_MIR0_C_ADDR(val)            (((val) << REG_MIR0_C_ADDR_SHIFT) & REG_MIR0_C_ADDR_MASK)

// mir0_imsc
#define REG_MIR0_IMSC_SHIFT             (0)
#define REG_MIR0_IMSC_MASK              (0x3 << REG_MIR0_IMSC_SHIFT)
#define REG_MIR0_IMSC(val)              (((val) << REG_MIR0_IMSC_SHIFT) & REG_MIR0_IMSC_MASK)

// mir0_ris
#define REG_MIR0_RIS_SHIFT              (0)
#define REG_MIR0_RIS_MASK               (0x3 << REG_MIR0_RIS_SHIFT)
#define REG_MIR0_RIS(val)               (((val) << REG_MIR0_RIS_SHIFT) & REG_MIR0_RIS_MASK)

// mir0_mis
#define REG_MIR0_MIS_SHIFT              (0)
#define REG_MIR0_MIS_MASK               (0x3 << REG_MIR0_MIS_SHIFT)
#define REG_MIR0_MIS(val)               (((val) << REG_MIR0_MIS_SHIFT) & REG_MIR0_MIS_MASK)

// mir0_icr
#define REG_MIR0_ICR_SHIFT              (0)
#define REG_MIR0_ICR_MASK               (0x3 << REG_MIR0_ICR_SHIFT)
#define REG_MIR0_ICR(val)               (((val) << REG_MIR0_ICR_SHIFT) & REG_MIR0_ICR_MASK)

// mir0_isr
#define REG_MIR0_ISR_SHIFT              (0)
#define REG_MIR0_ISR_MASK               (0x3 << REG_MIR0_ISR_SHIFT)
#define REG_MIR0_ISR(val)               (((val) << REG_MIR0_ISR_SHIFT) & REG_MIR0_ISR_MASK)

// mir0_leak_en
#define REG_MIR0_LEAK_EN_SHIFT          (0)
#define REG_MIR0_LEAK_EN_MASK           (0x1 << REG_MIR0_LEAK_EN_SHIFT)
#define REG_MIR0_LEAK_EN(val)           (((val) << REG_MIR0_LEAK_EN_SHIFT) & REG_MIR0_LEAK_EN_MASK)

// mir0_sw_leak
#define REG_MIR0_SW_LEAK_SHIFT          (0)
#define REG_MIR0_SW_LEAK_MASK           (0x1 << REG_MIR0_SW_LEAK_SHIFT)
#define REG_MIR0_SW_LEAK(val)           (((val) << REG_MIR0_SW_LEAK_SHIFT) & REG_MIR0_SW_LEAK_MASK)



//----------------------------------------------------------------------
//  HDR MIR1 REGS
//----------------------------------------------------------------------

// mir1_enable
#define REG_MIR1_CONT_ENABLE_SHIFT      (0)
#define REG_MIR1_CONT_ENABLE_MASK       (0x1 << REG_MIR1_CONT_ENABLE_SHIFT)
#define REG_MIR1_CONT_ENABLE(val)       (((val) << REG_MIR1_CONT_ENABLE_SHIFT) & REG_MIR1_CONT_ENABLE_MASK)

// mir1_sw_start
#define REG_MIR1_SW_START_SHIFT         (0)
#define REG_MIR1_SW_START_MASK          (0x1 << REG_MIR1_SW_START_SHIFT)
#define REG_MIR1_SW_START(val)          (((val) << REG_MIR1_SW_START_SHIFT) & REG_MIR1_SW_START_MASK)

// mir1_gen_upd
#define REG_MIR1_GEN_UPD_SHIFT          (0)
#define REG_MIR1_GEN_UPD_MASK           (0x1 << REG_MIR1_GEN_UPD_SHIFT)
#define REG_MIR1_GEN_UPD(val)           (((val) << REG_MIR1_GEN_UPD_SHIFT) & REG_MIR1_GEN_UPD_MASK)

// mir1_ctrl
#define REG_MIR1_DATA_FORMAT_SHIFT      (0)
#define REG_MIR1_DATA_FORMAT_MASK       (0x7 << REG_MIR1_DATA_FORMAT_SHIFT)
#define REG_MIR1_DATA_FORMAT(val)       (((val) << REG_MIR1_DATA_FORMAT_SHIFT) & REG_MIR1_DATA_FORMAT_MASK)

#define REG_MIR1_BURST_LEN_SHIFT        (16)
#define REG_MIR1_BURST_LEN_MASK         (0x3 << REG_MIR1_BURST_LEN_SHIFT)
#define REG_MIR1_BURST_LEN(val)         (((val) << REG_MIR1_BURST_LEN_SHIFT) & REG_MIR1_BURST_LEN_MASK)

#define REG_MIR1_DATA_SWAP_SHIFT        (20)
#define REG_MIR1_DATA_SWAP_MASK         (0x3 << REG_MIR1_DATA_SWAP_SHIFT)
#define REG_MIR1_DATA_SWAP(val)         (((val) << REG_MIR1_DATA_SWAP_SHIFT) & REG_MIR1_DATA_SWAP_MASK)

// mir1_x_length
#define REG_MIR1_X_LENGTH_SHIFT         (0)
#define REG_MIR1_X_LENGTH_MASK          (0xFFFFFFFF << REG_MIR1_X_LENGTH_SHIFT)
#define REG_MIR1_X_LENGTH(val)          (((val) << REG_MIR1_X_LENGTH_SHIFT) & REG_MIR1_X_LENGTH_MASK)

// mir1_y_length
#define REG_MIR1_Y_LENGTH_SHIFT         (0)
#define REG_MIR1_Y_LENGTH_MASK          (0xFFFFFFFF << REG_MIR1_Y_LENGTH_SHIFT)
#define REG_MIR1_Y_LENGTH(val)          (((val) << REG_MIR1_Y_LENGTH_SHIFT) & REG_MIR1_Y_LENGTH_MASK)

// mir1_z_length
#define REG_MIR1_Z_LENGTH_SHIFT         (0)
#define REG_MIR1_Z_LENGTH_MASK          (0xFFFFFFFF << REG_MIR1_Z_LENGTH_SHIFT)
#define REG_MIR1_Z_LENGTH(val)          (((val) << REG_MIR1_Z_LENGTH_SHIFT) & REG_MIR1_Z_LENGTH_MASK)

// mir1_x_stride
#define REG_MIR1_X_STRIDE_SHIFT         (0)
#define REG_MIR1_X_STRIDE_MASK          (0xFFFFFFF8 << REG_MIR1_X_STRIDE_SHIFT)
#define REG_MIR1_X_STRIDE(val)          (((val) << REG_MIR1_X_STRIDE_SHIFT) & REG_MIR1_X_STRIDE_MASK)

// mir1_y_stride
#define REG_MIR1_Y_STRIDE_SHIFT         (0)
#define REG_MIR1_Y_STRIDE_MASK          (0xFFFFFFF8 << REG_MIR1_Y_STRIDE_SHIFT)
#define REG_MIR1_Y_STRIDE(val)          (((val) << REG_MIR1_Y_STRIDE_SHIFT) & REG_MIR1_Y_STRIDE_MASK)

// mir1_y_addr
#define REG_MIR1_Y_ADDR_SHIFT           (0)
#define REG_MIR1_Y_ADDR_MASK            (0xFFFFFFFF << REG_MIR1_Y_ADDR_SHIFT)
#define REG_MIR1_Y_ADDR(val)            (((val) << REG_MIR1_Y_ADDR_SHIFT) & REG_MIR1_Y_ADDR_MASK)

// mir1_c_addr
#define REG_MIR1_C_ADDR_SHIFT           (0)
#define REG_MIR1_C_ADDR_MASK            (0xFFFFFFFF << REG_MIR1_C_ADDR_SHIFT)
#define REG_MIR1_C_ADDR(val)            (((val) << REG_MIR1_C_ADDR_SHIFT) & REG_MIR1_C_ADDR_MASK)

// mir1_imsc
#define REG_MIR1_IMSC_SHIFT             (0)
#define REG_MIR1_IMSC_MASK              (0x3 << REG_MIR1_IMSC_SHIFT)
#define REG_MIR1_IMSC(val)              (((val) << REG_MIR1_IMSC_SHIFT) & REG_MIR1_IMSC_MASK)

// mir1_ris
#define REG_MIR1_RIS_SHIFT              (0)
#define REG_MIR1_RIS_MASK               (0x3 << REG_MIR1_RIS_SHIFT)
#define REG_MIR1_RIS(val)               (((val) << REG_MIR1_RIS_SHIFT) & REG_MIR1_RIS_MASK)

// mir1_mis
#define REG_MIR1_MIS_SHIFT              (0)
#define REG_MIR1_MIS_MASK               (0x3 << REG_MIR1_MIS_SHIFT)
#define REG_MIR1_MIS(val)               (((val) << REG_MIR1_MIS_SHIFT) & REG_MIR1_MIS_MASK)

// mir1_icr
#define REG_MIR1_ICR_SHIFT              (0)
#define REG_MIR1_ICR_MASK               (0x3 << REG_MIR1_ICR_SHIFT)
#define REG_MIR1_ICR(val)               (((val) << REG_MIR1_ICR_SHIFT) & REG_MIR1_ICR_MASK)

// mir1_isr
#define REG_MIR1_ISR_SHIFT              (0)
#define REG_MIR1_ISR_MASK               (0x3 << REG_MIR1_ISR_SHIFT)
#define REG_MIR1_ISR(val)               (((val) << REG_MIR1_ISR_SHIFT) & REG_MIR1_ISR_MASK)

// mir1_leak_en
#define REG_MIR1_LEAK_EN_SHIFT          (0)
#define REG_MIR1_LEAK_EN_MASK           (0x1 << REG_MIR1_LEAK_EN_SHIFT)
#define REG_MIR1_LEAK_EN(val)           (((val) << REG_MIR1_LEAK_EN_SHIFT) & REG_MIR1_LEAK_EN_MASK)

// mir1_sw_leak
#define REG_MIR1_SW_LEAK_SHIFT          (0)
#define REG_MIR1_SW_LEAK_MASK           (0x1 << REG_MIR1_SW_LEAK_SHIFT)
#define REG_MIR1_SW_LEAK(val)           (((val) << REG_MIR1_SW_LEAK_SHIFT) & REG_MIR1_SW_LEAK_MASK)


//----------------------------------------------------------------------
//  HDR MIW0 REGS
//----------------------------------------------------------------------

// miw0_enable
#define REG_MIW0_ENABLE_SHIFT           (0)
#define REG_MIW0_ENABLE_MASK            (0x1 << REG_MIW0_ENABLE_SHIFT)
#define REG_MIW0_ENABLE(val)            (((val) << REG_MIW0_ENABLE_SHIFT) & REG_MIW0_ENABLE_MASK)

// miw0_cfg_upd
#define REG_MIW0_CFG_UPD_SHIFT          (0)
#define REG_MIW0_CFG_UPD_MASK           (0x1 << REG_MIW0_CFG_UPD_SHIFT)
#define REG_MIW0_CFG_UPD(val)           (((val) << REG_MIW0_CFG_UPD_SHIFT) & REG_MIW0_CFG_UPD_MASK)

// miw0_gen_upd
#define REG_MIW0_GEN_UPD_SHIFT          (0)
#define REG_MIW0_GEN_UPD_MASK           (0x1 << REG_MIW0_GEN_UPD_SHIFT)
#define REG_MIW0_GEN_UPD(val)           (((val) << REG_MIW0_GEN_UPD_SHIFT) & REG_MIW0_GEN_UPD_MASK)

// miw0_ctrl
#define REG_MIW0_DATA_FORMAT_SHIFT      (0)
#define REG_MIW0_DATA_FORMAT_MASK       (0x7 << REG_MIW0_DATA_FORMAT_SHIFT)
#define REG_MIW0_DATA_FORMAT(val)       (((val) << REG_MIW0_DATA_FORMAT_SHIFT) & REG_MIW0_DATA_FORMAT_MASK)

#define REG_MIW0_Y_FULL_SHIFT           (4)
#define REG_MIW0_Y_FULL_MASK            (0x1 << REG_MIW0_Y_FULL_SHIFT)
#define REG_MIW0_Y_FULL(val)            (((val) << REG_MIW0_Y_FULL_SHIFT) & REG_MIW0_Y_FULL_MASK)

#define REG_MIW0_C_FULL_SHIFT           (8)
#define REG_MIW0_C_FULL_MASK            (0x1 << REG_MIW0_C_FULL_SHIFT)
#define REG_MIW0_C_FULL(val)            (((val) << REG_MIW0_C_FULL_SHIFT) & REG_MIW0_C_FULL_MASK)

#define REG_MIW0_NONCOSITED_SHIFT       (12)
#define REG_MIW0_NONCOSITED_MASK        (0x1 << REG_MIW0_NONCOSITED_SHIFT)
#define REG_MIW0_NONCOSITED(val)        (((val) << REG_MIW0_NONCOSITED_SHIFT) & REG_MIW0_NONCOSITED_MASK)

#define REG_MIW0_BURST_LEN_SHIFT        (16)
#define REG_MIW0_BURST_LEN_MASK         (0x3 << REG_MIW0_BURST_LEN_SHIFT)
#define REG_MIW0_BURST_LEN(val)         (((val) << REG_MIW0_BURST_LEN_SHIFT) & REG_MIW0_BURST_LEN_MASK)

#define REG_MIW0_DATA_SWAP_SHIFT        (20)
#define REG_MIW0_DATA_SWAP_MASK         (0x3 << REG_MIW0_DATA_SWAP_SHIFT)
#define REG_MIW0_DATA_SWAP(val)         (((val) << REG_MIW0_DATA_SWAP_SHIFT) & REG_MIW0_DATA_SWAP_MASK)

#define REG_MIW0_X_SKIP_SHIFT           (24)
#define REG_MIW0_X_SKIP_MASK            (0x1 << REG_MIW0_X_SKIP_SHIFT)
#define REG_MIW0_X_SKIP(val)            (((val) << REG_MIW0_X_SKIP_SHIFT) & REG_MIW0_X_SKIP_MASK)

// miw0_x_stride
#define REG_MIW0_XSTRIDE_SHIFT          (0)
#define REG_MIW0_XSTRIDE_MASK           (0xFFFFFFF8 << REG_MIW0_XSTRIDE_SHIFT)
#define REG_MIW0_XSTRIDE(val)           (((val) << REG_MIW0_XSTRIDE_SHIFT) & REG_MIW0_XSTRIDE_MASK)

// miw0_y_stride
#define REG_MIW0_YSTRIDE_SHIFT          (0)
#define REG_MIW0_YSTRIDE_MASK           (0xFFFFFFF8 << REG_MIW0_YSTRIDE_SHIFT)
#define REG_MIW0_YSTRIDE(val)           (((val) << REG_MIW0_YSTRIDE_SHIFT) & REG_MIW0_YSTRIDE_MASK)

// miw0_y_addr
#define REG_MIW0_Y_ADDR_SHIFT           (0)
#define REG_MIW0_Y_ADDR_MASK            (0xFFFFFFF8 << REG_MIW0_Y_ADDR_SHIFT)
#define REG_MIW0_Y_ADDR(val)            (((val) << REG_MIW0_Y_ADDR_SHIFT) & REG_MIW0_Y_ADDR_MASK)

// miw0_c_addr
#define REG_MIW0_C_ADDR_SHIFT           (0)
#define REG_MIW0_C_ADDR_MASK            (0xFFFFFFF8 << REG_MIW0_C_ADDR_SHIFT)
#define REG_MIW0_C_ADDR(val)            (((val) << REG_MIW0_C_ADDR_SHIFT) & REG_MIW0_C_ADDR_MASK)

// miw0_imsc
#define REG_MIW0_IMSC_SHIFT             (0)
#define REG_MIW0_IMSC_MASK              (0x3 << REG_MIW0_IMSC_SHIFT)
#define REG_MIW0_IMSC(val)              (((val) << REG_MIW0_IMSC_SHIFT) & REG_MIW0_IMSC_MASK)

// miw0_ris
#define REG_MIW0_RIS_SHIFT              (0)
#define REG_MIW0_RIS_MASK               (0x3 << REG_MIW0_RIS_SHIFT)
#define REG_MIW0_RIS(val)               (((val) << REG_MIW0_RIS_SHIFT) & REG_MIW0_RIS_MASK)

// miw0_mis
#define REG_MIW0_MIS_SHIFT              (0)
#define REG_MIW0_MIS_MASK               (0x3 << REG_MIW0_MIS_SHIFT)
#define REG_MIW0_MIS(val)               (((val) << REG_MIW0_MIS_SHIFT) & REG_MIW0_MIS_MASK)

// miw0_icr
#define REG_MIW0_ICR_SHIFT              (0)
#define REG_MIW0_ICR_MASK               (0x3 << REG_MIW0_ICR_SHIFT)
#define REG_MIW0_ICR(val)               (((val) << REG_MIW0_ICR_SHIFT) & REG_MIW0_ICR_MASK)

// miw0_isr
#define REG_MIW0_ISR_SHIFT              (0)
#define REG_MIW0_ISR_MASK               (0x3 << REG_MIW0_ISR_SHIFT)
#define REG_MIW0_ISR(val)               (((val) << REG_MIW0_ISR_SHIFT) & REG_MIW0_ISR_MASK)

// miw0_leak_en
#define REG_MIW0_LEAK_EN_SHIFT          (0)
#define REG_MIW0_LEAK_EN_MASK           (0x1 << REG_MIW0_LEAK_EN_SHIFT)
#define REG_MIW0_LEAK_EN(val)           (((val) << REG_MIW0_LEAK_EN_SHIFT) & REG_MIW0_LEAK_EN_MASK)

// miw0_sw_leak
#define REG_MIW0_SW_LEAK_SHIFT          (0)
#define REG_MIW0_SW_LEAK_MASK           (0x1 << REG_MIW0_SW_LEAK_SHIFT)
#define REG_MIW0_SW_LEAK(val)           (((val) << REG_MIW0_SW_LEAK_SHIFT) & REG_MIW0_SW_LEAK_MASK)


//----------------------------------------------------------------------
//  HDR MIW1 REGS
//----------------------------------------------------------------------

// miw1_enable
#define REG_MIW1_ENABLE_SHIFT           (0)
#define REG_MIW1_ENABLE_MASK            (0x1 << REG_MIW1_ENABLE_SHIFT)
#define REG_MIW1_ENABLE(val)            (((val) << REG_MIW1_ENABLE_SHIFT) & REG_MIW1_ENABLE_MASK)

// miw1_cfg_upd
#define REG_MIW1_CFG_UPD_SHIFT          (0)
#define REG_MIW1_CFG_UPD_MASK           (0x1 << REG_MIW1_CFG_UPD_SHIFT)
#define REG_MIW1_CFG_UPD(val)           (((val) << REG_MIW1_CFG_UPD_SHIFT) & REG_MIW1_CFG_UPD_MASK)

// miw1_gen_upd
#define REG_MIW1_GEN_UPD_SHIFT          (0)
#define REG_MIW1_GEN_UPD_MASK           (0x1 << REG_MIW1_GEN_UPD_SHIFT)
#define REG_MIW1_GEN_UPD(val)           (((val) << REG_MIW1_GEN_UPD_SHIFT) & REG_MIW1_GEN_UPD_MASK)

// miw1_ctrl
#define REG_MIW1_DATA_FORMAT_SHIFT      (0)
#define REG_MIW1_DATA_FORMAT_MASK       (0x7 << REG_MIW1_DATA_FORMAT_SHIFT)
#define REG_MIW1_DATA_FORMAT(val)       (((val) << REG_MIW1_DATA_FORMAT_SHIFT) & REG_MIW1_DATA_FORMAT_MASK)

#define REG_MIW1_Y_FULL_SHIFT           (4)
#define REG_MIW1_Y_FULL_MASK            (0x1 << REG_MIW1_Y_FULL_SHIFT)
#define REG_MIW1_Y_FULL(val)            (((val) << REG_MIW1_Y_FULL_SHIFT) & REG_MIW1_Y_FULL_MASK)

#define REG_MIW1_C_FULL_SHIFT           (8)
#define REG_MIW1_C_FULL_MASK            (0x1 << REG_MIW1_C_FULL_SHIFT)
#define REG_MIW1_C_FULL(val)            (((val) << REG_MIW1_C_FULL_SHIFT) & REG_MIW1_C_FULL_MASK)

#define REG_MIW1_NONCOSITED_SHIFT       (12)
#define REG_MIW1_NONCOSITED_MASK        (0x1 << REG_MIW1_NONCOSITED_SHIFT)
#define REG_MIW1_NONCOSITED(val)        (((val) << REG_MIW1_NONCOSITED_SHIFT) & REG_MIW1_NONCOSITED_MASK)

#define REG_MIW1_BURST_LEN_SHIFT        (16)
#define REG_MIW1_BURST_LEN_MASK         (0x3 << REG_MIW1_BURST_LEN_SHIFT)
#define REG_MIW1_BURST_LEN(val)         (((val) << REG_MIW1_BURST_LEN_SHIFT) & REG_MIW1_BURST_LEN_MASK)

#define REG_MIW1_DATA_SWAP_SHIFT        (20)
#define REG_MIW1_DATA_SWAP_MASK         (0x3 << REG_MIW1_DATA_SWAP_SHIFT)
#define REG_MIW1_DATA_SWAP(val)         (((val) << REG_MIW1_DATA_SWAP_SHIFT) & REG_MIW1_DATA_SWAP_MASK)

#define REG_MIW1_X_SKIP_SHIFT           (24)
#define REG_MIW1_X_SKIP_MASK            (0x1 << REG_MIW1_X_SKIP_SHIFT)
#define REG_MIW1_X_SKIP(val)            (((val) << REG_MIW1_X_SKIP_SHIFT) & REG_MIW1_X_SKIP_MASK)

// miw1_x_stride
#define REG_MIW1_XSTRIDE_SHIFT          (0)
#define REG_MIW1_XSTRIDE_MASK           (0xFFFFFFF8 << REG_MIW1_XSTRIDE_SHIFT)
#define REG_MIW1_XSTRIDE(val)           (((val) << REG_MIW1_XSTRIDE_SHIFT) & REG_MIW1_XSTRIDE_MASK)

// miw1_y_stride
#define REG_MIW1_YSTRIDE_SHIFT          (0)
#define REG_MIW1_YSTRIDE_MASK           (0xFFFFFFF8 << REG_MIW1_YSTRIDE_SHIFT)
#define REG_MIW1_YSTRIDE(val)           (((val) << REG_MIW1_YSTRIDE_SHIFT) & REG_MIW1_YSTRIDE_MASK)

// miw1_y_addr
#define REG_MIW1_Y_ADDR_SHIFT           (0)
#define REG_MIW1_Y_ADDR_MASK            (0xFFFFFFF8 << REG_MIW1_Y_ADDR_SHIFT)
#define REG_MIW1_Y_ADDR(val)            (((val) << REG_MIW1_Y_ADDR_SHIFT) & REG_MIW1_Y_ADDR_MASK)

// miw1_c_addr
#define REG_MIW1_C_ADDR_SHIFT           (0)
#define REG_MIW1_C_ADDR_MASK            (0xFFFFFFF8 << REG_MIW1_C_ADDR_SHIFT)
#define REG_MIW1_C_ADDR(val)            (((val) << REG_MIW1_C_ADDR_SHIFT) & REG_MIW1_C_ADDR_MASK)

// miw1_imsc
#define REG_MIW1_IMSC_SHIFT             (0)
#define REG_MIW1_IMSC_MASK              (0x3 << REG_MIW1_IMSC_SHIFT)
#define REG_MIW1_IMSC(val)              (((val) << REG_MIW1_IMSC_SHIFT) & REG_MIW1_IMSC_MASK)

// miw1_ris
#define REG_MIW1_RIS_SHIFT              (0)
#define REG_MIW1_RIS_MASK               (0x3 << REG_MIW1_RIS_SHIFT)
#define REG_MIW1_RIS(val)               (((val) << REG_MIW1_RIS_SHIFT) & REG_MIW1_RIS_MASK)

// miw1_mis
#define REG_MIW1_MIS_SHIFT              (0)
#define REG_MIW1_MIS_MASK               (0x3 << REG_MIW1_MIS_SHIFT)
#define REG_MIW1_MIS(val)               (((val) << REG_MIW1_MIS_SHIFT) & REG_MIW1_MIS_MASK)

// miw1_icr
#define REG_MIW1_ICR_SHIFT              (0)
#define REG_MIW1_ICR_MASK               (0x3 << REG_MIW1_ICR_SHIFT)
#define REG_MIW1_ICR(val)               (((val) << REG_MIW1_ICR_SHIFT) & REG_MIW1_ICR_MASK)

// miw1_isr
#define REG_MIW1_ISR_SHIFT              (0)
#define REG_MIW1_ISR_MASK               (0x3 << REG_MIW1_ISR_SHIFT)
#define REG_MIW1_ISR(val)               (((val) << REG_MIW1_ISR_SHIFT) & REG_MIW1_ISR_MASK)

// miw1_leak_en
#define REG_MIW1_LEAK_EN_SHIFT          (0)
#define REG_MIW1_LEAK_EN_MASK           (0x1 << REG_MIW1_LEAK_EN_SHIFT)
#define REG_MIW1_LEAK_EN(val)           (((val) << REG_MIW1_LEAK_EN_SHIFT) & REG_MIW1_LEAK_EN_MASK)

// miw1_sw_leak
#define REG_MIW1_SW_LEAK_SHIFT          (0)
#define REG_MIW1_SW_LEAK_MASK           (0x1 << REG_MIW1_SW_LEAK_SHIFT)
#define REG_MIW1_SW_LEAK(val)           (((val) << REG_MIW1_SW_LEAK_SHIFT) & REG_MIW1_SW_LEAK_MASK)



#endif
