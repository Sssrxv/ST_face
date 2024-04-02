#ifndef __SDCARD_H__
#define __SDCARD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define USE_FATFS       1
#define SD_SPI_USE_DMA  1


#define SD_SPI_DEV          (SPI_DEVICE_1)
#define SD_SPI_WORK_MODE    (SPI_WORK_MODE_0)
#define SD_SPI_FRAME_FRMT   (SPI_FF_STANDARD)
#define SD_SPI_BIT_LEN      (8)
#define SD_SPI_INI_RATE     (400000)
#define SD_SPI_NORMAL_RATE  (20000000)
#define SD_SPI_CS           (SPI_CHIP_SELECT_0)
#define SD_CS_PIN_DEV       (GPIO_PIN2)
#define SD_CS_UNSELECT()    gpio_set_pin(SD_CS_PIN_DEV, GPIO_PV_HIGH)
#define SD_CS_SELECT()      gpio_set_pin(SD_CS_PIN_DEV, GPIO_PV_LOW)
#define SD_DELAY_MS(n)      aiva_msleep(n)
#define SD_DELAY_US(n)      aiva_usleep(n)



int sd_initialize(void);
uint8_t sd_get_status(void);
uint32_t sd_get_block_number(void);
int sd_wait_ready(void);
int sd_read_sector(uint8_t *buff, uint32_t sector, uint32_t count);
int sd_write_sector(uint8_t *buff, uint32_t sector, uint32_t count);


#if USE_FATFS
#include "ff.h"
#include "diskio.h"
int sd_disk_ioctl(uint8_t ctrl, void *buff);
#endif


#ifdef __cplusplus
}
#endif

#endif // __SDCARD_H__
