#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <stdint.h>
#include "rectify.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


#define EEPROM_USE_DMA                   0


#define EEPROM_IIC_DMA_CH_RX            DMAC_CHANNEL0
#define EEPROM_IIC_DMA_CH_TX            DMAC_CHANNEL1
#define EEPROM_ADDR_WIDTH               (7)
#define EEPROM_DELAY_MS(n)              aiva_msleep(n)
#define EEPROM_DELAY_US(n)              aiva_usleep(n)
#define EEPROM_MALLOC(n)                aiva_malloc(n)
#define EEPROM_FREE(p)                  aiva_free(p)
#define EEPROM_PRINT(fmt, ...)          LOG_PRINTF(fmt, ## __VA_ARGS__)
#define EEPROM_LOGE(TAG, fmt, ...)      LOGE(TAG, fmt,  ## __VA_ARGS__)
#define EEPROM_LOGD(TAG, fmt, ...)      LOGD(TAG, fmt,  ## __VA_ARGS__)
#define EEPROM_LOGI(TAG, fmt, ...)      LOGI(TAG, fmt,  ## __VA_ARGS__)
#define EEPROM_LOGW(TAG, fmt, ...)      LOGW(TAG, fmt,  ## __VA_ARGS__)

                

typedef struct _eeprom_ctrl {
    uint8_t dev_addr;
    uint8_t dev_mem_addr;
    uint8_t mem_addr[2];
    uint16_t mem_addr_len;
    uint16_t page_num;
    uint16_t page_size;
    uint8_t wr_delay; // unit:ms
    uint8_t reserved[1];
    //    uint16_t size;
    char *name;
    struct {
        i2c_device_number_t  dev_num;
        // uint8_t use_dma;
        // uint8_t bus_addr_width;
        uint32_t bus_clk;
    }i2c;
} __attribute__((packed)) eeprom_ctrl;



int eeprom_is_busy(eeprom_ctrl *dev);
int eeprom_wait_ready(eeprom_ctrl *dev);
int eeprom_write_page(eeprom_ctrl *dev, uint8_t *buf, uint32_t addr, uint16_t num);
int eeprom_write(eeprom_ctrl *dev, uint8_t *buf, uint32_t addr, uint32_t len);
int eeprom_read(eeprom_ctrl *dev, uint8_t *buf, uint32_t addr, uint32_t len);




#ifdef __cplusplus
}
#endif

#endif // __EEPROM_H__
