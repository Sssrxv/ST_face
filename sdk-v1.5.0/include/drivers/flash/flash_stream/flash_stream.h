#ifndef __FLASH_STREAM_H__
#define __FLASH_STREAM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_STREAM_RD       (0)
#define FLASH_STREAM_WR       (1)

typedef struct _flash_stream_info_t {
    int spi_num;
    int spi_chip_sel;
    int clk_rate;
    int flash_type;
    int stream_attr;
    int start_rw_addr;
    int quad_en;
} flash_stream_info_t;

int flash_stream_open(flash_stream_info_t *info);

int flash_stream_read(uint8_t *rx_buff, int length);

int flash_stream_write(uint8_t *tx_buff, int length);

int flash_stream_set_addr(int addr);

int flash_stream_get_addr(void);

int flash_stream_close(void);

#ifdef __cplusplus
}
#endif

#endif // __FLASH_STREAM_H__
