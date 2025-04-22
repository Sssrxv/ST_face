#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

#include "flash_dev.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct flash_sfdp {
    int is_support_uniform_4kb_erase;
    uint8_t uniform_4kb_erase_opcode;
    int is_support_1_4_4_qual_io_fast_read;
    uint8_t qual_io_fast_read_opcode;
    uint32_t memory_bytes_density;
} flash_sfdp_t;

/* get flash manufacture id and device id */
int nor_flash_read_id(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, uint8_t *mid, uint16_t *did);
int nor_flash_read_sfdp(flash_dev_t *dev, flash_sfdp_t *sfdp);

#ifdef __cplusplus
}
#endif


#endif /* __NOR_FLASH_H__ */
