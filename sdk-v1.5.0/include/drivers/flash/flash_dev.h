#ifndef _DRIVER_FLASH_DEV_
#define _DRIVER_FLASH_DEV_

#include "spi.h"
#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_USE_DMA

typedef struct flash_dev flash_dev_t;

typedef enum flash_type {
    FLASH_NOR = 0,
    FLASH_NAND,
} flash_type_t;

typedef struct flash_param {
    // nor/nand common params
    const char *dev_name;
    uint8_t     mid;
    uint16_t    did;
    uint32_t    chip_size;
    uint32_t    page_size;
    // flash specific params
    union
    {
        uint32_t sector_size; // nor flash param
        uint32_t block_size; // nand flash param
    };
    uint32_t    extra_size; // nand flash only param
    // reserved params
    uint32_t    feature;        /* reserved */
} flash_param_t;

#define FLASH_PARAM_INITIALIZER \
(((flash_param_t){              \
    .dev_name = '\0',           \
    .mid = 0,                   \
    .did = 0,                   \
    .chip_size = 0,             \
    .page_size = 0,             \
    .sector_size = 0,           \
    .extra_size = 0,            \
    .feature = 0,               \
}))

typedef enum flash_status {
    FLASH_OK = 0,               // 0
    FLASH_UNSPPORT,             // 1
    FLASH_WRONG_PARAM,          // 2
    FLASH_ERROR,                // 3
    FLASH_ERROR_TIMEOUT,        // 4
    FLASH_ERROR_LENGTH,         // 5
    FLASH_ERROR_STATUS,         // 6
    FLASH_WRITE_DISABLE,        // 7
    FLASH_ERROR_ERASE,          // 8
    FLASH_ERROR_WRITE,          // 9
    FLASH_ERROR_SPI_TRANS,      // 10
} flash_status_t;

typedef struct flash_unique_id {
    uint8_t     id_length;
    uint8_t     uid[16];
} flash_unique_id_t;

/**
 * @brief
 */
typedef struct flash_dev_config {
    spi_device_num_t spi_num;
    spi_chip_select_t spi_cs;
    uint32_t clk_rate;
    int enable_quad_rw;
} flash_dev_config_t;

#define FLASH_DEV_CFG_INITIALIZER \
(((flash_dev_config_t){           \
    .spi_num = SPI_DEVICE_0,      \
    .spi_cs = SPI_CHIP_SELECT_0,  \
    .clk_rate = 0,                \
    .enable_quad_rw = 0,          \
}))

typedef struct flash_dev_ops {
    int (*init)(flash_dev_t *dev, const flash_dev_config_t *cfg);
    int (*read)(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
    int (*write)(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
    int (*erase)(flash_dev_t *dev, uint32_t addr, int force_erase);
    int (*program)(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
    int (*release)(flash_dev_t *dev);
    int (*read_unique_id)(flash_dev_t *dev, flash_unique_id_t *unique_id);
    int (*get_param)(flash_dev_t *dev, flash_param_t *param);
    int (*lock)(flash_dev_t *dev, uint32_t addr, uint32_t length);
    int (*unlock)(flash_dev_t *dev, uint32_t addr, uint32_t length);
} flash_dev_ops_t;

#define FLASH_DEV_OPS_INITIALIZER \
(((flash_dev_ops_t){              \
    .init = NULL,                 \
    .read = NULL,                 \
    .write = NULL,                \
    .erase = NULL,                \
    .program = NULL,              \
    .release = NULL,              \
    .read_unique_id = NULL,       \
    .get_param = NULL,            \
    .lock = NULL,                 \
    .unlock = NULL,               \
}))

typedef struct flash_dev {
    flash_type_t type;
    uint8_t manuf_id;                                   /**manufacture id */
    uint16_t device_id;                                 /**device id */
    const char *name;                                   /**name of flash device */
    uint32_t chip_size;
    //
    flash_dev_ops_t *ops;                               /**all supported operations of flash device */
    int is_initialized;                                 /**is this flash device been initialized, must initialize duvice before use it */
    int is_support_xip;                                 /**is this flash device support xip */
    flash_dev_config_t cfg;                             /**configuration of this device */
#ifdef USE_RTOS
    pthread_mutex_t mutex;                              /**mutex of this device */
#endif
    void *priv;                                         /**private data of this device */
} flash_dev_t;

#ifdef USE_RTOS
#define FLASH_DEV_INITIALIZER \
(((flash_dev_t){              \
    .type = FLASH_NOR,        \
    .manuf_id = 0,            \
    .device_id = 0,           \
    .name = '\0',             \
    .chip_size = 0,           \
    .ops = NULL,              \
    .is_initialized = 0,      \
    .is_support_xip = 0,      \
    .mutex = {0},             \
    .priv = NULL,             \
}))

#define FLASH_DEV_INITIALIZER_WITH_PARAMS(_type, mid, did, _name, _chip_size, _ops, _is_support_xip, _priv) \
(((flash_dev_t){                                                                                            \
    .type = _type,                                                                                          \
    .manuf_id = mid,                                                                                        \
    .device_id = did,                                                                                       \
    .name = _name,                                                                                          \
    .chip_size = _chip_size,                                                                                \
    .ops = _ops,                                                                                            \
    .is_initialized = 0,                                                                                    \
    .is_support_xip = _is_support_xip,                                                                      \
    .mutex = {0},                                                                                           \
    .priv = _priv,                                                                                          \
}))

#else

#define FLASH_DEV_INITIALIZER \
(((flash_dev_t){              \
    .type = FLASH_NOR,        \
    .manuf_id = 0,            \
    .device_id = 0,           \
    .name = '\0',             \
    .chip_size = 0,           \
    .ops = NULL,              \
    .is_initialized = 0,      \
    .is_support_xip = 0,      \
    .priv = NULL,             \
}))

#define FLASH_DEV_INITIALIZER_WITH_PARAMS(_type, mid, did, _name, _chip_size, _ops, _is_support_xip, _priv) \
(((flash_dev_t){                                                                                            \
    .type = _type,                                                                                          \
    .manuf_id = mid,                                                                                        \
    .device_id = did,                                                                                       \
    .name = _name,                                                                                          \
    .chip_size = _chip_size,                                                                                \
    .ops = _ops,                                                                                            \
    .is_initialized = 0,                                                                                    \
    .is_support_xip = _is_support_xip,                                                                      \
    .priv = _priv,                                                                                          \
}))
#endif

#define FLASH_QUAD_DIS          (0)
#define FLASH_QUAD_EN           (1)

#define FIFO_W8                 (8)
#define FIFO_W32                (32)

/* 
 * flash r/w will be splited into multiple r/w
 * CPU R/W:
 * - split R/W according to SPI_FIFO_DEPTH
 * DMA R/W:
 * - for nor  flash: R/W block is 64 KB
 * - for nand flash: R/W block is 64 KB
 *
 */
#ifndef FLASH_USE_DMA
#define FIFO_NOR_LEN            SPI_FIFO_DEPTH
#define FIFO_NAND_LEN           SPI_FIFO_DEPTH
#else
//#define FIFO_NOR_LEN            (16 * 1024)
#define FIFO_NOR_LEN            (64 * 1024)
#define FIFO_NAND_LEN           (64 * 1024)
#endif // FLASH_USE_DMA

#define NAND_NO_FORCE_ERASE     (0)
#define NAND_FORCE_ERASE        (1)

/**
 * @brief  found flash device match given name, will search in udevices whose
 *         uclass_id_t is UCLASS_SPI_FLASH
 * 
 * @param[in] flash_type  used to return default flash driver if no match device found
 * @param[in] name        udevice name
 * 
 * @return return matched device or default device of given flash_type or NULL if no default
 */
flash_dev_t *flash_dev_find_device(flash_type_t flash_type, const char *name);

/**
 * @brief  flash_dev_find_device_by_mid_did, will search in udevices whose
 *         uclass_id_t is UCLASS_SPI_FLASH
 * 
 * @param[in] flash_type  used to return default flash driver if no match device found
 * @param[in] spi_num     used to help check whether in xip mode
 * @param[in] mid         manufacture id of flash
 * @param[in] did         device id of flash
 * 
 * @return return matched device or default device of given flash_type or NULL if no default
 */
flash_dev_t *flash_dev_find_device_by_mid_did(flash_type_t flash_type, spi_device_num_t spi_num, uint8_t mid, uint16_t did);

/**
 * @brief  flash_init 
 * 
 * @param[in] dev device instance to operate on
 * @param[in] cfg configuration to initialize this dev
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_init(flash_dev_t *dev, const flash_dev_config_t *cfg);

/**
 * @brief  read from addr to addr+length into data_buf
 * 
 * @param[in] dev        device instance to operate on
 * @param[in] addr       start address to read
 * @param[in] data_buf   data buf read to
 * @param[in] length     data length to read
 * 
 * @return int, 0 for success and other for fail
 */
int flash_dev_read(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);

/**
 * @brief  write from addr to addr+length with data in data_buf, auto erase if necessary
 * 
 * @param[in] dev        device instance to operate on
 * @param[in] addr       start address to write
 * @param[in] data_buf   data to write
 * @param[in] length     data length to write
 * 
 * @return int, 0 for success and other for fail
 */
int flash_dev_write(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);

/**
 * @brief  flash_erase
 * 
 * @param[in] dev           device instance to operate on
 * @param[in] addr          start addr of flash to erase
 * @param[in] force_erase   nor flash: ignored
 *                          nand flash: directly erase block even if this block
 *                                      is marked as bad block when set to 1
 * @param[in] length        length to erase
 * 
 * @return int, 0 for success and other for fail
 */
int flash_dev_erase(flash_dev_t *dev, uint32_t addr, int force_erase);

/**
 * @brief  programe from addr to addr+length use data in data_buf
 * 
 * @param[in] dev       device instance to operate on
 * @param[in] addr      start addr of flash to program
 * @param[in] data_buf  data to program
 * @param[in] length    length of data_buf
 * 
 * @return int, 0 for success and other for fail
 */
int flash_dev_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);

/**
 * @brief  flash_release 
 * 
 * @param[in] dev device instance to operate on
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_release(flash_dev_t *dev);

/**
 * @brief  read unique id of flash
 *
 * @param[in]      dev       device instance to operate on
 * @param[in, out] unique_id unique id read from dev
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_read_unique_id(flash_dev_t *dev, flash_unique_id_t *unique_id);

/**
 * @brief  get flash params
 *
 * @param[in]      dev           device instance to operate on
 * @param[in, out] flash_param   params to get from dev
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_get_param(flash_dev_t *dev, flash_param_t *flash_param);

/**
 * @brief  lock flash region from addr to addr+length
 *
 * @param[in] dev      device instance to operate on
 * @param[in] addr     start address to lock
 * @param[in] length   length of flash memory to lock from addr
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_lock(flash_dev_t *dev, uint32_t addr, uint32_t length);

/**
 * @brief  unlock flash region from addr to addr+length
 *
 * @param[in] dev      device instance to operate on
 * @param[in] addr     start address to lock
 * @param[in] length   length of flash memory to lock from addr
 *
 * @return int, 0 for success and other for fail
 */
int flash_dev_unlock(flash_dev_t *dev, uint32_t addr, uint32_t length);

#ifdef __cplusplus
}
#endif


#endif /* _DRIVER_FLASH_DEV_ */
