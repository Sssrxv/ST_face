#ifndef _DRIVER_SPI_FLASH_
#define _DRIVER_SPI_FLASH_

#include "flash_dev.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/* --------------------------------------------------------------------------*/
/**
 * @brief  get flash params
 *
 * @param[in, out] flash_param
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int flash_get_param(flash_param_t *flash_param);

#ifdef __cplusplus
}
#endif


#endif /* _DRIVER_SPI_FLASH_ */
