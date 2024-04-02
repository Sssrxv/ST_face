#ifndef __NAND_FLASH_DEV_H__
#define __NAND_FLASH_DEV_H__


#include "flash.h"
#include "nand_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _nand_flash_drv_t {
    /* manufacture id */
    int manuf_id;
    /* device id */
    int device_id;
    /* init spi and decide whether to use quad mode */
    int (*init)(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);
    /* de-init driver state */
    int (*release)();
    /* random read */
    int (*read)(uint32_t addr, uint8_t *data_buf, uint32_t length);
    /* random write, will do erase automatically  */
    int (*write)(uint32_t addr, uint8_t *data_buf, uint32_t length);
    /* program in a specific page, need to erase the block manually */
    int (*page_prog)(uint16_t blk, uint16_t page, uint16_t page_off, uint8_t *tx_buf, int tx_len);
    /* erase a block */
    int (*blk_erase)(uint32_t addr, int force_erase);
    /* get flash parameter */
    int (*get_param)(nand_flash_param_t *pparam);
    /* lock specific region */
    int (*lock)(uint32_t addr, uint32_t length);
    /* unlock specific region */
    int (*unlock)(uint32_t addr, uint32_t length);
    /* read unique id */
    int (*read_unique_id)(flash_unique_id_t *unique_id);
} nand_flash_drv_t;

nand_flash_drv_t* get_nand_flash_driver(int mid, int did);


#ifdef __cplusplus
}
#endif


#endif /*__NAND_FLASH_DEV_H__*/
