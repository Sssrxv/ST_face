#ifndef __MX25LXX_H__
#define __MX25LXX_H__

#include "flash.h"
#include "nor_flash.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int mx25lxx_read_id(uint8_t *manuf_id, uint16_t *device_id);
int mx25lxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
int mx25lxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length);
int mx25lxx_read_unique_id(flash_unique_id_t *unique_id);
int mx25lxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length);
int mx25lxx_sector_erase(uint32_t addr);
int mx25lxx_page_program(uint32_t addr, uint8_t *data_buf, uint32_t length);
int mx25lxx_get_param(nor_flash_param_t *pparam);

int mx25lxx_lock(uint32_t addr, uint32_t length);
int mx25lxx_unlock(uint32_t addr, uint32_t length);

int mx25lxx_self_test(int quad_rw);
int mx25lxx_stress_test(int quad_rw);


#ifdef __cplusplus
}
#endif

#endif // __MX25LXX_H__
