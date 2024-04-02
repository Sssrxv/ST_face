#ifndef __BOOT_FLASH_H__
#define __BOOT_FLASH_H__

#include <stdint.h>
#include <stddef.h>

#include "flash.h"

#ifdef __cplusplus
extern "C" {
#endif

void udelay(uint32_t us);

void boot_flash_init(uint8_t spi_index, uint8_t spi_ss, uint32_t baudr);

int boot_flash_read_id(flash_type_t type, uint16_t *manufacture_id, uint16_t *device_id);

int boot_flash_read(flash_type_t type, uint32_t addr, uint8_t *rx_buff, uint32_t rx_len, int dma_flag);

int boot_get_flash_type(uint8_t spi_dev, uint32_t baudr);

// this api will cache current flash type after first call
int boot_get_flash_type_with_cache(uint8_t spi_dev, uint32_t baudr);

/* FW image definitions */
#define FW_MARKER                   0x61697661
#define FW_VERSION(major, minor)    (((major) << 16) | ((minor) & 0xffff))

#define RBC_GET_ADDR(x)         (((x) << 2) & 0xFFFFFFFC)
#define RBC_GET_OP(x)           (((x) >> 30) & 0x3)
#define RBC_OP_NOP              (0U)
#define RBC_OP_WR               (2U)
#define RBC_OP_RD               (3U)

#define PLL_RBC_N               (6)
#define EXT_RBC_N               (8)

typedef struct _rbc_cmd_t {
    uint32_t op_addr;           // addr[29:0], op[31:30]
    uint32_t data_mask;         // [31:0]
} __attribute__((packed, aligned(4))) rbc_cmd_t;


typedef struct _fw_header_t {
    uint32_t fw_marker;
    uint32_t fw_version;        /* major 16-bit, minor 16-bit */
    uint32_t load_addr;
    int32_t  length;
    uint8_t  flash_type;        /* 0 - nor flash, 1 - nand flash */
    uint8_t  dma_en;            /* whether to enable dma */
    uint8_t  pll_bypass;        /* whether to bypass PLL */
    uint8_t  ext_rbc_en;        /* whether to program extra rbc */
    uint16_t spi_baudr;         /* spi baud rate */
    uint16_t spi_rx_delay;      /* spi rx sample delay */
    uint32_t hdr_chksum;        /* FW header checksum */
    uint32_t img_chksum;        /* XOR check sum of image */
} __attribute__((packed, aligned(4))) fw_header_t;

typedef struct _pll_header_t {
    rbc_cmd_t pll_rbcs[PLL_RBC_N];
    uint32_t  hdr_chksum;
} __attribute__((packed, aligned(4))) pll_header_t;

typedef struct _ext_header_t {
    rbc_cmd_t ext_rbcs[EXT_RBC_N];
    uint32_t  hdr_chksum;
} __attribute__((packed, aligned(4))) ext_header_t;

#define FW_HDR_SIZE                             (sizeof(fw_header_t))
#define PLL_HDR_SIZE                            (sizeof(pll_header_t))
#define EXT_HDR_SIZE                            (sizeof(ext_header_t))

#define FLASH_NAND_PAGE_SIZE                    (2048)
#define FLASH_NAND_BLK_SIZE                     (FLASH_NAND_PAGE_SIZE * 64)

#define SPI_DMA_EN                              (1)
#define SPI_DMA_DIS                             (0)

#define SPI_DMA_MAX_RD_SIZE                     (2048)

int is_valid_image(fw_header_t *p_hdr);
int checksum_verify(uint32_t *buffer, uint32_t length, uint32_t checksum);

#ifdef __cplusplus
}
#endif


#endif /* __BOOT_FLASH_H__ */
