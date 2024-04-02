#ifndef __NOR_FLASH_DEV_H__
#define __NOR_FLASH_DEV_H__

#include "flash.h"
#include "nor_flash.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _nor_flash_drv_t {
    /* manufacture id */
    int manuf_id;
    /* device id */
    int device_id;
    /* init spi and decide whether to use quad mode */
    int (*init)(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
    /* random read */
    int (*read)(uint32_t addr, uint8_t *data_buf, uint32_t length);
    /* random write, will do erase automatically  */
    int (*write)(uint32_t addr, uint8_t *data_buf, uint32_t length);
    /* program in a specific page, need to erase the block manually */
    int (*page_prog)(uint32_t addr, uint8_t *tx_buf, uint32_t tx_len);
    /* erase a sector*/
    int (*sector_erase)(uint32_t addr);
    /* get flash parameter */
    int (*get_param)(nor_flash_param_t *pparam);
    /* lock a specific region */
    int (*lock)(uint32_t addr, uint32_t length);
    /* unlock a specific region */
    int (*unlock)(uint32_t addr, uint32_t length);
    /* read unique id */
    int (*read_unique_id)(flash_unique_id_t *unique_id);
} nor_flash_drv_t;

nor_flash_drv_t* get_nor_flash_driver(int mid, int did);

#ifdef __cplusplus
}
#endif

#endif /*__NOR_FLASH_DEV_H__*/
