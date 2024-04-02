#ifndef _DRIVER_SPI_FLASH_
#define _DRIVER_SPI_FLASH_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_USE_DMA

typedef enum _flash_type_t
{
    FLASH_NOR = 0,
    FLASH_NAND,
}flash_type_t;


typedef enum _flash_status {
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

typedef struct _flash_unique_id {
    uint8_t     id_length;
    uint8_t     uid[16];
}flash_unique_id_t;

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

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_init 
 * 
 * @param flash_type
 * @param spi_index
 * @param spi_ss
 * @param rate
 * @param quad_rw
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int flash_init(flash_type_t flash_type, uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_read 
 * 
 * @param addr
 * @param data_buf
 * @param length
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int flash_read (uint32_t addr, uint8_t *data_buf, uint32_t length);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_write 
 * 
 * @param addr
 * @param data_buf
 * @param length
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int flash_write(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_erase
 * 
 * @param addr
 * @param force_erase
 * @param length
 * 
 * @return
 */
/* --------------------------------------------------------------------------*/
int flash_erase(uint32_t addr, int force_erase);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_erase
 * 
 * @param addr
 * @param force_erase
 * @param length
 * 
 * @return
 */
/* --------------------------------------------------------------------------*/
int flash_program(uint32_t addr, uint8_t *data_buf, uint32_t length);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  flash_release 
 * 
 * @return   
 */
/* --------------------------------------------------------------------------*/
int flash_release(void);

/* --------------------------------------------------------------------------*/
/**
 * @brief  read unique id of flash
 *
 * @param[in, out] unique_id
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int flash_read_unique_id(flash_unique_id_t *unique_id);

#ifdef __cplusplus
}
#endif


#endif /* _DRIVER_SPI_FLASH_ */
