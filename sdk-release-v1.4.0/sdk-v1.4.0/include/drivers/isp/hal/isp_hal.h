#ifndef _ISP_HAL_H_
#define _ISP_HAL_H_

// Registers
#define HAL_REG_ADDRESS     0x20010000


#define GET_REG(reg)                     isp_hal_read_reg(mm##reg)
#define SET_REG(reg, value)              isp_hal_write_reg(mm##reg, value)

#define VAL2FLD(reg, field, val)        (((uint32_t)(val) << reg##__##field##__SHIFT) & reg##__##field##_MASK)
#define FLD2VAL(reg, field, val)        (((val) & reg##__##field##_MASK) >> reg##__##field##__SHIFT)
#define MODIFYFLD(var, reg, field, val) (var = (var & (~reg##__##field##_MASK)) | (((uint32_t)(val) << reg##__##field##__SHIFT) & reg##__##field##_MASK))

#define GET_REG_FIELD(reg, field)       (FLD2VAL(reg, field, GET_REG(reg)))
#define SET_REG_FIELD(reg, field, val)  (SET_REG(reg, (GET_REG(reg) & ~reg##__##field##_MASK) | VAL2FLD(reg, field, val)))

#include "reg/isp_reg_offset.h"
#include "reg/isp_reg_mask.h"
#include "reg/isp_reg_shift.h"
#include "isp_base_types.h"

#ifdef __cplusplus
extern "C"
{
#endif


// In Hal.c
void   isp_hal_write_reg(uint32_t addr, uint32_t value);
uint32_t isp_hal_read_reg(uint32_t addr);

#ifdef __cplusplus
}
#endif


#endif
