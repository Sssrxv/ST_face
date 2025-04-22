#ifndef _GD5FXX_H
#define _GD5FXX_H

#include <stdint.h>
#include "flash.h"
#include "nand_flash.h"

#ifdef __cplusplus
extern "C" {
#endif
/* winbond nand flash */

//int gd5fxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate);
int gd5fxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int gd5fxx_release();
int gd5fxx_read_id(uint8_t *manuf_id, uint16_t *device_id);
int gd5fxx_read_unique_id(flash_unique_id_t *unique_id);
int gd5fxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int gd5fxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

int gd5fxx_blk_erase(uint32_t addr, int force_erase);
int gd5fxx_get_param(nand_flash_param_t *pparam);

int gd5fxx_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int gd5fxx_lock(uint32_t addr, uint32_t length);
int gd5fxx_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
