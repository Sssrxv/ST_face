#ifndef __NOR_STD_DRV_H__
#define __NOR_STD_DRV_H__

#include <stdint.h>
#include <stddef.h>
#include "flash.h"
#include "nor_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * nor flash generic driver 
 * only support standard spi mode
 */

int nor_std_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int nor_std_read_id(uint8_t *manuf_id, uint16_t *device_id);
int nor_std_read_unique_id(flash_unique_id_t *unique_id);

int nor_std_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int nor_std_read(uint32_t addr, uint8_t *data_buf, uint32_t length);

int nor_std_sector_erase(uint32_t addr);
int nor_std_get_param(nor_flash_param_t *pparam);

int nor_std_page_program(uint32_t addr, uint8_t *tx_buff, uint32_t tx_len);

int nor_std_lock(uint32_t addr, uint32_t length);

int nor_std_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif


#endif /* __NOR_STD_DRV_H__ */
