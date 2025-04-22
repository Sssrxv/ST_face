#ifndef _W25NXX_H
#define _W25NXX_H

#include <stdint.h>
#include "flash.h"
#include "nand_flash.h"

#ifdef __cplusplus
extern "C" {
#endif
/* winbond nand flash */

//int w25nxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate);
int w25nxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int w25nxx_release();
int w25nxx_read_id(uint8_t *manuf_id, uint16_t *device_id);
int w25nxx_read_unique_id(flash_unique_id_t *unique_id);
int w25nxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int w25nxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

int w25nxx_blk_erase(uint32_t addr, int force_erase);
int w25nxx_get_param(nand_flash_param_t *pparam);

int w25nxx_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int w25nxx_lock(uint32_t addr, uint32_t length);
int w25nxx_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
