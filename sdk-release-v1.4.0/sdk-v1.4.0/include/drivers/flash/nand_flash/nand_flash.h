#ifndef _nand_flash_H
#define _nand_flash_H

#include <stdint.h>
#include "flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * nand flash generic driver 
 */
//#define POTECT_SUPPORT

typedef struct _nand_flash_param_t {
    char       *dev_name;
    uint32_t    mid;
    uint32_t    did;
    uint32_t    chip_size;
    uint32_t    block_size;
    uint32_t    page_size;
    uint32_t    extra_size;
    uint32_t    feature;        /* reserved */
} __attribute__((packed, aligned(4))) nand_flash_param_t;

#define NAND_ECC_STAT_SUP            (1U << 0)  // ECC status in the status register
#define NAND_MULT_DIE_SUP            (1U << 1)  // Multiple dies
#define NAND_BUF_STAT_SUP            (1U << 2)  // BUF status in the status register
#define NAND_CACHE_RD_SUP            (1U << 3)  // Cache read support

/* external interfaces */
/* ---------------------------- NAND FLASH INTERFACES ---------------------------- */

/* init spi and decide whether to use quad mode */
int nand_flash_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);

/* de-init driver state */
int nand_flash_release();

/* get flash manufacture id and device id */
int nand_flash_read_id(uint8_t *manuf_id, uint16_t *device_id);

/* get flash unique id */
int nand_flash_read_unique_id(flash_unique_id_t *unique_id);

/* random write, will do erase automatically  */
int nand_flash_write(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* random read */
int nand_flash_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* erase a block */
int nand_flash_blk_erase(uint32_t addr, int force_erase);

/* get flash parameter */
int nand_flash_get_param(nand_flash_param_t *pparam);

/* program in a specific page, need to erase the block manually */
int nand_flash_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

/* lock a specific region */
int nand_flash_lock(uint32_t addr, uint32_t length);

/* unlock a specific region */
int nand_flash_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
