#ifndef _FLASH_CFG_H_
#define _FLASH_CFG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*static const uint32_t NOR_FLASH_CLK_RATE  = 50*1000*1000;*/
//static const uint32_t NAND_FLASH_CLK_RATE = 50*1000*1000;

static const uint32_t NOR_FLASH_CLK_RATE  = 80*1000*1000;
static const uint32_t NAND_FLASH_CLK_RATE = 80*1000*1000;

#ifdef __cplusplus
}
#endif 

#endif //_FLASH_CFG_H_
