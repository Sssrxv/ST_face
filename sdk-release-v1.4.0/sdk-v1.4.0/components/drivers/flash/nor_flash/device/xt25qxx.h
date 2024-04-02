#ifndef _XT25QXX_H
#define _XT25QXX_H

#include "flash.h"
#include "nor_flash.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int xt25qxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int xt25qxx_read_id(uint8_t *manuf_id, uint16_t *device_id);
int xt25qxx_read_unique_id(flash_unique_id_t *unique_id);
int xt25qxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int xt25qxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length);
int xt25qxx_page_program(uint32_t addr, uint8_t *data_buf, uint32_t length);
int xt25qxx_sector_erase(uint32_t addr);
int xt25qxx_get_param(nor_flash_param_t *pparam);

int xt25qxx_lock(uint32_t addr, uint32_t length);
int xt25qxx_unlock(uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif

