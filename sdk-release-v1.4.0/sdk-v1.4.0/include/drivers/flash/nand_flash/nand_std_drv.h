#ifndef __NAND_STD_DRV_H__
#define __NAND_STD_DRV_H__

#include <stdint.h>
#include "flash.h"
#include "nand_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* init spi and decide whether to use quad mode */
int nand_std_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);

/* de-init driver state */
int nand_std_release();

/* get flash manufacture id and device id */
int nand_std_read_id(uint8_t *manuf_id, uint16_t *device_id);

/* get flash unique id */
int nand_std_read_unique_id(flash_unique_id_t *unique_id);

/* random write, will do erase automatically  */
int nand_std_write(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* random read */
int nand_std_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* erase a block */
int nand_std_blk_erase(uint32_t addr, int force_erase);

/* get flash parameter */
int nand_std_get_param(nand_flash_param_t *pparam);

/* program in a specific page, need to erase the block manually */
int nand_std_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int nand_std_lock(uint32_t addr, uint32_t length);

int nand_std_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
