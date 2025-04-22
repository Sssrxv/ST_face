#ifndef _MX35LFXXE4_H
#define _MX35LFXXE4_H

#include <stdint.h>
#include "flash.h"
#include "nand_flash.h"

#ifdef __cplusplus
extern "C" {
#endif
/* winbond nand flash */

//int mx35lfxxe4_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate);
int mx35lfxxe4_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int mx35lfxxe4_release();
int mx35lfxxe4_read_id(uint8_t *manuf_id, uint16_t *device_id);
int mx35lfxxe4_read_unique_id(flash_unique_id_t *unique_id);
int mx35lfxxe4_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int mx35lfxxe4_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

int mx35lfxxe4_blk_erase(uint32_t addr, int force_erase);
int mx35lfxxe4_get_param(nand_flash_param_t *pparam);

int mx35lfxxe4_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int mx35lfxxe4_lock(uint32_t addr, uint32_t length);
int mx35lfxxe4_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
