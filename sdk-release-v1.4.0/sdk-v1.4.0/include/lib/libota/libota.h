#ifndef __LIB_OTA_H__
#define __LIB_OTA_H__

#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "flash_stream.h"
#include "xyzModem.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOAD_TO_MEM = 0,
    LOAD_TO_FLASH,
    LOAD_MAX,
};

#define OTA_MARKER                   0x61766961
#define PART_NAME_LEN                (32)
#define FILE_NAME_LEN                (64)

#define DATA_ATTR_GZIP               (1UL << 0)
#define DATA_ATTR_FW_IMG             (1UL << 1)
#define DATA_ATTR_RAW_BIN            (1UL << 2)
#define DATA_ATTR_NORMAL_FILE        (1UL << 3)

typedef struct _ota_hdr_t {
    uint32_t magic;
    uint32_t data_size;
    uint32_t data_attr;
    uint8_t  part_name[PART_NAME_LEN];
    uint8_t  file_name[FILE_NAME_LEN];
    uint8_t  fw_md5sum[16];
    uint32_t uncompressed_size;
    uint8_t  reserved[124];
    uint32_t checksum;
}__attribute__((packed, aligned(4))) ota_hdr_t;

bool is_compressed_file(const char *filename);

void ota_save_local_irqs(void);

void ota_restore_local_irqs(void);

uint32_t ota_load_ymodem(modem_chan_t chan, 
        uart_device_number_t uart_num, int uart_baudr, 
        uint32_t offset, int mode, uint32_t max_size, char *recv_file_name);

int ota_write_flash_fw(flash_stream_info_t *info, void *wr_buf, uint32_t wr_bytes);

int ota_write_flash_raw_data(flash_stream_info_t *info, void *wr_buf, uint32_t wr_bytes);

/**
 * @brief  compare flash data with memory data
 * @param [in] info    flash info to specify start address
 * @param [in] buffer  memeory buffer start address
 * @param [in] bytes   memroy buffer size
 *
 * @return  0: OK, other: Failed
 */
int ota_verify_flash_data(flash_stream_info_t *info, void *buffer, uint32_t bytes);

/**
 * @brief  compare flash data with memory data
 * @param [in] info    flash info to specify start address
 * @param [in] buffer  buffer start address, read flash data into this buffer
 * @param [in] bytes   buffer size
 * @param [in] golden_crc32   golden crc32
 *
 * @return  0: OK, other: Failed
 */
int ota_verify_flash_data_crc32(flash_stream_info_t *info, void *buffer, uint32_t bytes, uint32_t golden_crc32);

#ifdef __cplusplus
}
#endif

#endif //__LIB_OTA_H__
