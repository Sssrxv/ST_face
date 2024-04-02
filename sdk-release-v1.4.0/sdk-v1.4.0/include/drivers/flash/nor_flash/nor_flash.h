#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

#include <stdint.h>
#include <stddef.h>
#include "flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * nor flash generic driver
 */

typedef struct _nor_flash_param_t {
    char       *dev_name;
    uint32_t    mid;
    uint32_t    did;
    uint32_t    chip_size;
    uint32_t    sector_size;
    uint32_t    page_size;
    uint32_t    feature;        /* reserved */
}__attribute__((packed, aligned(4))) nor_flash_param_t;

/* ---------------------------- NOR FLASH INTERFACES ---------------------------- */

/* init spi and decide whether to use quad mode */
int nor_flash_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);

int nor_flash_release(void);

/* get flash manufacture id and device id */
int nor_flash_read_id(uint8_t *manuf_id, uint16_t *device_id);

/* get flash unique id */
int nor_flash_read_unique_id(flash_unique_id_t *unique_id);

/* random write, will do erase automatically  */
int nor_flash_write(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* random read */
int nor_flash_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* erase a sector */
int nor_flash_sector_erase(uint32_t addr);

/* get flash parameter */
int nor_flash_get_param(nor_flash_param_t *pparam);

/* program in a specific page, need to erase the block manually */
int nor_flash_page_program(uint32_t addr, uint8_t *tx_buff, uint32_t tx_len);

/* lock a specific region */
int nor_flash_lock(uint32_t addr, uint32_t length);

/* unlock a specific region */
int nor_flash_unlock(uint32_t addr, uint32_t length);



#ifdef __cplusplus
}
#endif


#endif /* __NOR_FLASH_H__ */
