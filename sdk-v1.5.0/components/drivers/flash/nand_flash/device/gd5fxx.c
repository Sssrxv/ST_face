/* Winbond NAND flash driver */
#include "gd5fxx.h"
#include "nand_flash.h"
#include "spi.h"
#include "sysctl.h"
#include "dmac.h"
#include <stdint.h>
#include <string.h>
#include "flash.h"
#include "aiva_sleep.h"
#include "aiva_malloc.h"
#include "dmac.h"
#include "syslog.h"
#include "nand_comm.h"

#include "udevice.h"
#include "nand_flash_dev.h"

/* clang-format off */

#define GD5FXX_PAGE_SIZE              (2048)
#define GD5FXX_EXTRA_SIZE             (64)
#define GD5FXX_BLOCK_SIZE             (128 * 1024)
#define GD5FXX_PAGE_NUM_PER_BLK       (GD5FXX_BLOCK_SIZE/GD5FXX_PAGE_SIZE)

#define GET_PAGE_ADDR(blk, page)     ((((blk) << 6) | ((page) & 0x3f)) & 0xffff)
#define GET_COL_ADDR(page_offset)    ((page_offset) & 0xfff)

/* Command codes for the flash_command routine */
#define FLASH_RESET             0xFF    /* reset flash */
#define FLASH_RDID              0x9F    /* read manufacturer and product id */
#define FLASH_GFEAT             0x0F    /* read status/get feature option */
#define FLASH_SFEAT             0x1F    /* write status/set feature option */
#   define FEATURE_PROT_ADDR    0xA0    /* protection register */
#   define FEATURE_FEAT_ADDR    0xB0    /* configuration register */
#   define FEATURE_STAT_ADDR    0xC0    /* status register */
#define FLASH_WREN              0x06    /* set write enable latch */
#define FLASH_WRDI              0x04    /* reset write enable latch */
#define FLASH_BERASE            0xD8    /* erase one block in memory array */

#define FLASH_PROG              0x02    /* program load data to cache */
#define FLASH_QPROG             0x32    /* quad program load data to cache */
#define FLASH_PEXEC             0x10    /* program cache data to memory array */
#define FLASH_PROG_RAN          0x84    /* program load data to cache at offset */
#define FLASH_QPROG_RAN         0x34    /* quad program load data to cache at offset */

#define FLASH_PREAD             0x13    /* read from memory array to cache */
#define FLASH_NEXT_PREAD        0x31    /* GD specific: next page cache read */
#define FLASH_LAST_PREAD        0x3F    /* GD specific: last page cache read */
#define FLASH_READ              0x03    /* read data from cache */
#define FLASH_READ_FAST         0x0B    /* read data from cache */
#define FLASH_QREAD_FAST        0xEB    /* quad-read data from cache */

#define FEATURE_STAT_ENH        0x30
#define FEATURE_STAT_AUX        0xF0

#define STAT_ECC_MASK           0x30
#define STAT_ECC_UNCORR         0x20    /* uncorrectable error */
#define STAT_PFAIL              0x8     /* program fail */
#define STAT_EFAIL              0x4     /* erase fail */
#define STAT_WEL                0x2     /* write enable latch */
#define STAT_OIP                0x1     /* operation in progress */

#define FEAT_ECC_EN             (1u << 4)
#define FEAT_BUF_EN             (1u << 3)
#define FEAT_QUAD_EN            (1u << 0)

#define FLASH_SELECT_DIE        0xC2
#define FLASH_BAD_BLOCK_MARKER  0x00

#define NON_CACHE_RD_MODE       (0UL << 0)
#define CACHE_RD_MODE           (1UL << 0)
#define CACHE_RD_FIRST          (1UL << 1)
#define CACHE_RD_LAST           (1UL << 2)

/* clang-format on */

typedef struct{
    uint32_t addr_protected_bound;
    uint8_t  feature_prot_cfg_value;
}feature_prot_cfg;

static feature_prot_cfg feature_prot_cfg_lut[]={
    {0x003f, (0b11001) << 1},
    {0x03ff, (0b00110) << 1},
    {0x07ff, (0b01010) << 1},
    {0x0fff, (0b01110) << 1},
    {0x1fff, (0b10010) << 1},
    {0x3fff, (0b10110) << 1},
    {0x7fff, (0b11010) << 1},
    {0xbfff, (0b10101) << 1},
    {0xdfff, (0b10001) << 1},
    {0xefff, (0b01101) << 1},
    {0xf7ff, (0b01001) << 1},
    {0xfbff, (0b00101) << 1},
    {0xffff, (0b11101) << 1},
};
static uint32_t spi_bus_no = 0;
static uint32_t spi_chip_select = 0;
static uint32_t gd5fxx_FLASH_CHIP_SIZE = (128 * 1024 * 1024);
static nand_flash_param_t m_param = {0};

static volatile int m_gd5fxx_inited    = 0;
static volatile int m_gd5fxx_cur_die   = 0;
static volatile int m_gd5fxx_die_size  = 0;
static bblk_lut_t m_bblk_lut;

int gd5fxx_mark_bad_blk(uint16_t blk, uint32_t badblockmarker);

static int (*gd5fxx_read_page_func)(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc, int cache_rd_flag);
static int (*gd5fxx_page_program_func)(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int gd5fxx_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc, int cache_rd_flag);

static int gd5fxx_wait_for_ready();

static int gd5fxx_receive_data(uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    int ret;
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_standard(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    ret = spi_receive_data_standard_dma(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}

static int gd5fxx_send_data(uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_standard(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_standard_dma(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}

static int gd5fxx_receive_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    int ret;
    
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(spi_bus_no, 8/*instrction length*/, 16/*address length*/, 2/*wait cycles*/,
                          SPI_AITM_ADDR_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_multiple(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    ret = spi_receive_data_multiple_dma(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        LOGE(__func__, "spi receive error %d.", ret);
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}

static int gd5fxx_send_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(spi_bus_no, 8/*instrction length*/, 16/*address length*/, 0/*wait cycles*/,
                          SPI_AITM_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_multiple(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_multiple_dma(spi_bus_no, spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    return FLASH_OK;
}

static int gd5fxx_select_die(uint8_t die_num)
{
    int ret;
    uint8_t cmd[2];

    cmd[0] = FLASH_SELECT_DIE;
    cmd[1] = die_num;

    ret = gd5fxx_send_data(cmd, 2, NULL, 0);
    
    if (ret == 0) {
        LOGI(__func__, "select die %d suc!", die_num);
        m_gd5fxx_cur_die = die_num;
    }

    return ret;
}


static int gd5fxx_read_status(uint8_t reg, uint8_t *reg_data)
{
    int ret;
    uint8_t cmd[2] = {FLASH_GFEAT, reg};
    
    ret = gd5fxx_receive_data(cmd, 2, reg_data, 1);
    CHECK_RET(ret);

    return ret;
}

static int gd5fxx_is_busy(void)
{
    uint8_t status = 0;
    
    gd5fxx_read_status(FEATURE_STAT_ADDR, &status);
    if (status & 0x1) {
        return 1;
    }
    return 0;
}

static int gd5fxx_wait_for_ready()
{
    uint32_t timeout;

    timeout = 20 * 1000 * 100; // 20 sec

    while(timeout--) {
        if (!gd5fxx_is_busy()) {
            return FLASH_OK;
        }
        aiva_usleep(10);
    }
    return -FLASH_ERROR_TIMEOUT;
}

static int gd5fxx_write_status(uint8_t reg, uint8_t reg_data)
{
    int ret;
    uint8_t cmd[3] = {FLASH_SFEAT, reg, reg_data};

    ret = gd5fxx_send_data(cmd, 3, 0, 0);
    CHECK_RET(ret);
    
    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);

    return ret;
}

static int gd5fxx_soft_reset(void)
{
    int ret;
    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);
    uint8_t cmd[1] = {FLASH_RESET};
    
    ret = gd5fxx_send_data(cmd, 1, 0, 0);
    CHECK_RET(ret);
   
    // The max tRST timing is 500us.
    /*aiva_usleep(500);*/
    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);
    
    return ret;
}


// gd5fxx_write_enable() must be called before any change to the 
// device such as write, erase. It also unlocks the blocks.
static int gd5fxx_write_enable(void)
{
    int ret;
    uint8_t prot;
#ifndef POTECT_SUPPORT
    /* make sure it is not locked firstly */
    ret = gd5fxx_read_status(FEATURE_PROT_ADDR, &prot);
    CHECK_RET(ret);

    if (prot != 0) {
        prot = 0;
        ret = gd5fxx_write_status(FEATURE_PROT_ADDR, prot);
        CHECK_RET(ret);
    }
#endif
    uint8_t cmd[1] = {FLASH_WREN};

    ret = gd5fxx_send_data(cmd, 1, 0, 0);
    CHECK_RET(ret);

    ret = gd5fxx_wait_for_ready();
    if (ret < 0) {
        return  ret;
    }
    
    ret = gd5fxx_read_status(FEATURE_STAT_ADDR, &prot);
    CHECK_RET(ret);

    if (!(prot & STAT_WEL)) {
        return -FLASH_ERROR_STATUS;
    }

    return ret;
}

/* Set ECC-E in configuration register */
static int gd5fxx_enable_ecc()
{
    int ret;
    uint8_t status;

    ret = gd5fxx_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status |= FEAT_ECC_EN;

    ret = gd5fxx_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
static int gd5fxx_enable_quad()
{
    int ret;
    uint8_t status;

    ret = gd5fxx_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status |= FEAT_QUAD_EN;

    ret = gd5fxx_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
/*Clear ECC-E in configuration register*/
static int gd5fxx_disable_ecc()
{
    int ret;
    uint8_t status;

    ret = gd5fxx_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status &= (~FEAT_ECC_EN);

    ret = gd5fxx_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
static int gd5fxx_disable_quad()
{
    int ret;
    uint8_t status;

    ret = gd5fxx_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status &= (~FEAT_QUAD_EN);

    ret = gd5fxx_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}

static int gd5fxx_is_ecc_good()
{
    int ret;
    uint8_t status;

    ret = gd5fxx_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if ((status & STAT_ECC_MASK) >= STAT_ECC_UNCORR) {
        return -FLASH_ERROR_STATUS;
    }
    
    return FLASH_OK;
}

static int _gd5fxx_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc, int cache_rd_flag)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    int is_cache_rd, cache_rd_first, cache_rd_last;
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }
    
    is_cache_rd    = cache_rd_flag & CACHE_RD_MODE;
    cache_rd_first = cache_rd_flag & CACHE_RD_FIRST;
    cache_rd_last  = cache_rd_flag & CACHE_RD_LAST;

    /*if (check_ecc) {*/
        /*ret = gd5fxx_enable_ecc();*/
        /*CHECK_RET(ret);*/
    /*}*/
    /*else {*/
        /*ret = gd5fxx_disable_ecc();*/
        /*CHECK_RET(ret);*/
    /*}*/

    page_addr = GET_PAGE_ADDR(blk, page);

    if (!is_cache_rd) {
        cmd[0] = FLASH_PREAD;
        cmd[1] = 0; // dummy byte
        cmd[2] = (page_addr >> 8) & 0xff;
        cmd[3] = page_addr & 0xff;

        // PAGE READ (13h), transfer data from NAND flash array to cache
        ret = gd5fxx_send_data(cmd, 4, 0, 0);
        CHECK_RET(ret);

        ret = gd5fxx_wait_for_ready();
        CHECK_RET(ret);

        if (check_ecc) {
            ret = gd5fxx_is_ecc_good();
            if (ret < 0) {
                LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
                nand_bblk_lut_set(&m_bblk_lut, blk, 1);
                gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
                return ret;
            }
        }

        col_addr = GET_COL_ADDR(page_offset);
        
        cmd[0] = FLASH_READ;
        cmd[1] = (col_addr >> 8) & 0xff;
        cmd[2] = col_addr & 0xff;
        cmd[3] = 0; // dummy byte
        
        // READ (03h), read from cache
        ret = gd5fxx_receive_data(cmd, 4, rx_buf, rx_len);
        CHECK_RET(ret);
    }
    else {
        if (cache_rd_first) {
            cmd[0] = FLASH_PREAD;
            cmd[1] = 0; // dummy byte
            cmd[2] = (page_addr >> 8) & 0xff;
            cmd[3] = page_addr & 0xff;

            // PAGE READ (13h), transfer data from NAND flash array to cache
            ret = gd5fxx_send_data(cmd, 4, 0, 0);
            CHECK_RET(ret);
        }
        
        ret = gd5fxx_wait_for_ready();
        CHECK_RET(ret);

        if (check_ecc) {
            ret = gd5fxx_is_ecc_good();
            if (ret < 0) {
                LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
                nand_bblk_lut_set(&m_bblk_lut, blk, 1);
                gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
                return ret;
            }
        }
        
        if (!cache_rd_last) {
            // Next page RD (0x31)
            cmd[0] = FLASH_NEXT_PREAD;
        }
        else {
            // Last page read (0x3F)
            cmd[0] = FLASH_LAST_PREAD;
        }
        ret = gd5fxx_send_data(cmd, 1, 0, 0);
        CHECK_RET(ret);
        
        col_addr = GET_COL_ADDR(page_offset);
        
        cmd[0] = FLASH_READ;
        cmd[1] = (col_addr >> 8) & 0xff;
        cmd[2] = col_addr & 0xff;
        cmd[3] = 0; // dummy byte
        
        // READ (03h), read from cache
        ret = gd5fxx_receive_data(cmd, 4, rx_buf, rx_len);
        CHECK_RET(ret);

    }

    return FLASH_OK;
}

static int _gd5fxx_quad_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc, int cache_rd_flag)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    uint32_t read_cmd[2];
    int is_cache_rd, cache_rd_first, cache_rd_last;
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }

    
    is_cache_rd    = cache_rd_flag & CACHE_RD_MODE;
    cache_rd_first = cache_rd_flag & CACHE_RD_FIRST;
    cache_rd_last  = cache_rd_flag & CACHE_RD_LAST;
    
    /*if (check_ecc) {*/
        /*ret = gd5fxx_enable_ecc();*/
        /*CHECK_RET(ret);*/
    /*}*/
    /*else {*/
        /*ret = gd5fxx_disable_ecc();*/
        /*CHECK_RET(ret);*/
    /*}*/

    page_addr = GET_PAGE_ADDR(blk, page);

    if (!is_cache_rd) {
        cmd[0] = FLASH_PREAD;
        cmd[1] = 0; // dummy byte
        cmd[2] = (page_addr >> 8) & 0xff;
        cmd[3] = page_addr & 0xff;

        // PAGE READ (13h), transfer data from NAND flash array to cache
        ret = gd5fxx_send_data(cmd, 4, 0, 0);
        CHECK_RET(ret);

        ret = gd5fxx_wait_for_ready();
        CHECK_RET(ret);

        if (check_ecc) {
            ret = gd5fxx_is_ecc_good();
            if (ret < 0) {
                LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
                nand_bblk_lut_set(&m_bblk_lut, blk, 1);
                gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
                return ret;
            }
        }
        
        col_addr = GET_COL_ADDR(page_offset);
        read_cmd[0] = FLASH_QREAD_FAST;
        read_cmd[1] = col_addr;
        
        // Quad READ (EBh), read from cache
        ret = gd5fxx_receive_data_enhanced(read_cmd, 2, rx_buf, rx_len);
        CHECK_RET(ret);
    }
    else {
        if (cache_rd_first) {
            cmd[0] = FLASH_PREAD;
            cmd[1] = 0; // dummy byte
            cmd[2] = (page_addr >> 8) & 0xff;
            cmd[3] = page_addr & 0xff;

            // PAGE READ (13h), transfer data from NAND flash array to cache
            ret = gd5fxx_send_data(cmd, 4, 0, 0);
            CHECK_RET(ret);
        }

        ret = gd5fxx_wait_for_ready();
        CHECK_RET(ret);

        if (check_ecc) {
            ret = gd5fxx_is_ecc_good();
            if (ret < 0) {
                LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
                nand_bblk_lut_set(&m_bblk_lut, blk, 1);
                gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
                return ret;
            }
        }
    
        if (!cache_rd_last) {
            // Next page RD (0x31)
            cmd[0] = FLASH_NEXT_PREAD;
        }
        else {
            // Last page read (0x3F)
            cmd[0] = FLASH_LAST_PREAD;
        }
        ret = gd5fxx_send_data(cmd, 1, 0, 0);
        CHECK_RET(ret);
       
        col_addr = GET_COL_ADDR(page_offset);
        read_cmd[0] = FLASH_QREAD_FAST;
        read_cmd[1] = col_addr;
        
        // Quad READ (EBh), read from cache
        ret = gd5fxx_receive_data_enhanced(read_cmd, 2, rx_buf, rx_len);
        CHECK_RET(ret);
    }


    return FLASH_OK;
}

// Read the 1st byte of the 64-byte spare area in the 1st page and 2nd page.
// Non-FFh data indicates bad block.
static int gd5fxx_is_blk_good(uint16_t blk)
{
    int ret;
    uint8_t byte0;
    uint32_t first_dword;
    int check_ecc;
    
    // 1. disable ecc to read the 64-byte extra data of the 1st page and 2nd page
    check_ecc = 0;
    ret = gd5fxx_disable_ecc();
    CHECK_RET(ret);
    ret = gd5fxx_read_page(blk, 0, GD5FXX_PAGE_SIZE, (uint8_t *)&first_dword, 4, check_ecc, NON_CACHE_RD_MODE);
    CHECK_RET(ret);

    // 2. Non-FFh checks.
    byte0 = first_dword & 0xff;
    if (byte0 != 0xff) {
        LOGW(__func__, "Blk %d, byte0 0x%x", byte0);
        return -FLASH_ERROR_STATUS;
    }
    
    return FLASH_OK;
}

static int _gd5fxx_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    uint8_t status;

    if (tx_len == 0) {
        return FLASH_OK;
    }
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }

    
    ret = gd5fxx_enable_ecc();
    CHECK_RET(ret);

    ret = gd5fxx_write_enable();
    CHECK_RET(ret);

    col_addr = GET_COL_ADDR(page_offset);
    cmd[0] = FLASH_PROG; // FLASH_PROG_RAN
    cmd[1] = (col_addr >> 8) & 0xff;
    cmd[2] = col_addr & 0xff;

    // program to cache (02h/84h)
    ret = gd5fxx_send_data(cmd, 3, tx_buf, tx_len);
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, page);
    cmd[0] = FLASH_PEXEC;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // execute program (10h)
    ret = gd5fxx_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);

    ret = gd5fxx_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_PFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}

static int _gd5fxx_quad_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    uint32_t prog_cmd[2];
    uint8_t status;

    if (tx_len == 0) {
        return FLASH_OK;
    }
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }
    
    ret = gd5fxx_enable_ecc();
    CHECK_RET(ret);

    ret = gd5fxx_write_enable();
    CHECK_RET(ret);

    col_addr    = GET_COL_ADDR(page_offset);
    prog_cmd[0] = FLASH_QPROG;
    prog_cmd[1] = col_addr;

    // program to cache (32h/34h)
    ret = gd5fxx_send_data_enhanced(prog_cmd, 2, tx_buf, tx_len);
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, page);
    cmd[0] = FLASH_PEXEC;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // execute program (10h)
    ret = gd5fxx_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);

    ret = gd5fxx_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_PFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}


static int gd5fxx_blk_program(uint16_t blk, uint8_t *tx_buf, uint32_t tx_len)
{
    uint16_t page;
    uint32_t tx_remain;
    int ret = FLASH_OK;

    uint16_t page_cnt = tx_len / GD5FXX_PAGE_SIZE;

    tx_remain = tx_len & (GD5FXX_PAGE_SIZE - 1);

    configASSERT(page_cnt <= GD5FXX_PAGE_NUM_PER_BLK);

    for (page = 0; page < page_cnt; page++) {
        ret = gd5fxx_page_program(blk, page, 0, tx_buf, GD5FXX_PAGE_SIZE);
        if (ret < 0) {
            break;
        }
        tx_buf += GD5FXX_PAGE_SIZE;
    }
    if (tx_remain) {
        ret = gd5fxx_page_program(blk, page, 0, tx_buf, tx_remain);
    }

    return ret;
}

static int gd5fxx_quad_blk_program(uint16_t blk, uint8_t *tx_buf, uint32_t tx_len)
{
    uint16_t page;
    uint32_t tx_remain;
    int ret = FLASH_OK;

    uint16_t page_cnt = tx_len / GD5FXX_PAGE_SIZE;

    tx_remain = tx_len & (GD5FXX_PAGE_SIZE - 1);

    configASSERT(page_cnt <= GD5FXX_PAGE_NUM_PER_BLK);

    for (page = 0; page < page_cnt; page++) {
        ret = _gd5fxx_quad_page_program(blk, page, 0, tx_buf, GD5FXX_PAGE_SIZE);
        if (ret < 0) {
            break;
        }
        tx_buf += GD5FXX_PAGE_SIZE;
    }
    if (tx_remain) {
        ret = _gd5fxx_quad_page_program(blk, page, 0, tx_buf, tx_remain);
    }

    return ret;
}


static void get_blk_page_and_offset(uint32_t addr, 
        uint16_t *blk_ptr,
        uint16_t *page_ptr,
        uint16_t *page_off_ptr
        )
{
    uint32_t blk, page;
    uint32_t blk_remain, page_remain;

    blk  = addr / GD5FXX_BLOCK_SIZE;
    blk_remain = addr & (GD5FXX_BLOCK_SIZE - 1);
    page = blk_remain / GD5FXX_PAGE_SIZE;
    page_remain = blk_remain & (GD5FXX_PAGE_SIZE - 1);

    *blk_ptr  = blk & 0xffff;
    *page_ptr = page & 0xffff;
    *page_off_ptr = page_remain & 0xffff;
}

static int gd5fxx_enable_quad_mode(void)
{
    gd5fxx_enable_quad();
    gd5fxx_page_program_func = _gd5fxx_quad_page_program;
    gd5fxx_read_page_func    = _gd5fxx_quad_read_page;

    return 0;
}

static int gd5fxx_disable_quad_mode(void)
{
    gd5fxx_disable_quad();
    gd5fxx_page_program_func = _gd5fxx_page_program;
    gd5fxx_read_page_func    = _gd5fxx_read_page;

    return 0;
}


int gd5fxx_read_id(uint8_t *manuf_id, uint16_t *device_id)
{
    uint8_t cmd[2] = {FLASH_RDID, 0};
    uint8_t data[3] = {0};
    int ret;

    ret = gd5fxx_receive_data(cmd, 2, data, 3);
    if (ret < 0) {
        return ret;
    }
    if (data[0] != 0xC8) {
        return -FLASH_UNSPPORT;
    }
    *manuf_id = data[0];
    //*device_id = ((uint16_t)(data[1]) << 8) | (uint16_t)(data[2]); 
    *device_id = data[1]; 
    return FLASH_OK;
}

int gd5fxx_read_unique_id(flash_unique_id_t *unique_id)
{
    LOGE(__func__, "Unsupported now!");

    return -FLASH_UNSPPORT;
}

int gd5fxx_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc, int cache_rd_flag)
{
    int ret;
    uint32_t addr;
    uint8_t  die_num;
    
    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    
    if (m_param.feature & NAND_MULT_DIE_SUP) {
        addr = (uint32_t)blk * GD5FXX_BLOCK_SIZE + (uint32_t)page * GD5FXX_PAGE_SIZE + page_offset;
        die_num = addr / m_gd5fxx_die_size;
        if (die_num != m_gd5fxx_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_gd5fxx_cur_die, die_num, addr);
            ret = gd5fxx_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    
    ret = gd5fxx_read_page_func(blk, page, page_offset, rx_buf, rx_len, check_ecc, cache_rd_flag);
   
    return ret;
}

int gd5fxx_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len)
{
    int ret;
    uint32_t addr;
    uint8_t  die_num;

    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    if (m_param.feature & NAND_MULT_DIE_SUP) {
        addr = (uint32_t)blk * GD5FXX_BLOCK_SIZE + (uint32_t)page * GD5FXX_PAGE_SIZE + page_offset;
        die_num = addr / m_gd5fxx_die_size;
        if (die_num != m_gd5fxx_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_gd5fxx_cur_die, die_num, addr);
            ret = gd5fxx_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    ret = gd5fxx_page_program_func(blk, page, page_offset, tx_buf, tx_len);
    return ret;
}

int gd5fxx_blk_erase(uint32_t addr, int force_erase)
{
    int ret;
    uint8_t cmd[4];
    uint16_t page_addr;
    uint8_t status;

    uint16_t blk;
    
    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    
    if (m_param.feature & NAND_MULT_DIE_SUP) {
        uint8_t die_num = addr / m_gd5fxx_die_size;
        if (die_num != m_gd5fxx_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_gd5fxx_cur_die, die_num, addr);
            ret = gd5fxx_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    
    blk = addr / GD5FXX_BLOCK_SIZE;

    /*ret = gd5fxx_is_blk_good(blk);*/
    /*CHECK_RET(ret);*/
 
    if (!force_erase) {
        if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
            LOGE(__func__, "bad blk %d", blk);
            return -FLASH_ERROR_STATUS;
        }
    }

    ret = gd5fxx_write_enable();
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, 0);
    cmd[0] = FLASH_BERASE;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    ret = gd5fxx_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = gd5fxx_wait_for_ready();
    CHECK_RET(ret);

    ret = gd5fxx_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_EFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        gd5fxx_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_ERASE;
    }
    nand_bblk_lut_set(&m_bblk_lut, blk, 0);
    gd5fxx_mark_bad_blk(blk, 0xFF);

    return FLASH_OK;
}

static int gd5fxx_has_cache_rd(void)
{
    return (m_param.feature & NAND_CACHE_RD_SUP) == NAND_CACHE_RD_SUP;
}

int gd5fxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t len = 0;
    uint16_t blk, page, page_offset;
    uint16_t page_remain;

    int cache_rd_flag = NON_CACHE_RD_MODE;
    int check_ecc = 1;
    int cache_rd_support;
    
    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    cache_rd_support = gd5fxx_has_cache_rd();

    // 1. Check whether the addr is legal
    if (addr >= gd5fxx_FLASH_CHIP_SIZE) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > gd5fxx_FLASH_CHIP_SIZE ? (gd5fxx_FLASH_CHIP_SIZE - addr) : length;
    
    blk  = 0;
    page = 0;
    page_offset = 0;

    ret = gd5fxx_enable_ecc();
    CHECK_RET(ret);

    if (cache_rd_support) {
        if (length > (2 * GD5FXX_PAGE_SIZE)) {
            cache_rd_flag = CACHE_RD_FIRST | CACHE_RD_MODE;
        }
    }

    while (length) {
        len = (length >= GD5FXX_PAGE_SIZE) ? GD5FXX_PAGE_SIZE: length;
        get_blk_page_and_offset(addr, &blk, &page, &page_offset);
        page_remain = GD5FXX_PAGE_SIZE - page_offset;

        if (cache_rd_support && (cache_rd_flag & CACHE_RD_MODE)) {
            if ((page == (GD5FXX_PAGE_NUM_PER_BLK - 1)) || (len == length)) {
                cache_rd_flag |= CACHE_RD_LAST;
            }
        }
        
        if (len > page_remain) {
            len = page_remain;
        }

        ret = gd5fxx_read_page(blk, page, page_offset, data_buf, len,
                check_ecc, cache_rd_flag);
        if (ret < 0) {
            return ret;
        }

        // clear the first cache read flag
        if (cache_rd_support && (cache_rd_flag & CACHE_RD_FIRST)) {
            cache_rd_flag &= ~(CACHE_RD_FIRST);
        }

        if (cache_rd_support && (cache_rd_flag & CACHE_RD_LAST)) {
            cache_rd_flag &= ~(CACHE_RD_LAST);
            cache_rd_flag |= CACHE_RD_FIRST;
            gd5fxx_soft_reset();
        }
        addr     += len;
        data_buf += len;
        length   -= len;
    }

    return ret;
}

int gd5fxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret = FLASH_OK;
    uint32_t blk_addr = 0;
    uint32_t blk_offset = 0;
    uint32_t blk_remain = 0;
    uint32_t write_len = 0;
    uint32_t index = 0;
    uint8_t *pread = NULL;
    uint8_t *pwrite = NULL;
    uint8_t *swap_buf = NULL;
    uint8_t *src_buf = data_buf;

    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    // 1. Check whether the addr is legal
    if (addr >= gd5fxx_FLASH_CHIP_SIZE) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > gd5fxx_FLASH_CHIP_SIZE ? (gd5fxx_FLASH_CHIP_SIZE - addr) : length;

    while (length)
    {
        blk_addr   = addr & (~(GD5FXX_BLOCK_SIZE - 1));
        blk_offset = addr & (GD5FXX_BLOCK_SIZE - 1);
        blk_remain = GD5FXX_BLOCK_SIZE - blk_offset;

        write_len = ((length < blk_remain) ? length : blk_remain);

        if (write_len == GD5FXX_BLOCK_SIZE && addr == blk_addr) {
            src_buf = data_buf;
        }
        else {
            if (!swap_buf) {
                swap_buf = aiva_malloc(GD5FXX_BLOCK_SIZE);
                configASSERT(swap_buf != NULL);
            }
            ret = gd5fxx_read(blk_addr, swap_buf, GD5FXX_BLOCK_SIZE);
            // if (ret < 0) {
            //     goto out;
            // }
            pread = swap_buf + blk_offset;
            pwrite = data_buf;
            for (index = 0; index < write_len; index++)
                *pread++ = *pwrite++;
            src_buf = swap_buf;
        }

        ret = gd5fxx_blk_erase(blk_addr, NAND_FORCE_ERASE);
        if (ret < 0) {
            goto out;
        }

        ret = gd5fxx_blk_program(blk_addr/GD5FXX_BLOCK_SIZE, src_buf, GD5FXX_BLOCK_SIZE);
        if (ret < 0) {
            goto out;
        }
        length   -= write_len;
        addr     += write_len;
        data_buf += write_len;
    }
out:
    if (swap_buf) {
        aiva_free(swap_buf);
    }
    return ret;
}

int gd5fxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw)
{
    int ret;
    uint8_t manuf_id;
    uint16_t device_id;

    if (m_gd5fxx_inited) {
        gd5fxx_release();
    }

    spi_bus_no = spi_index;
    spi_chip_select = spi_ss;
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
    spi_set_clk_rate(spi_bus_no, rate);

    gd5fxx_soft_reset();
    
    ret = gd5fxx_read_id(&manuf_id, &device_id);
    if (ret < 0) {
        return ret;
    }

    if (manuf_id != 0xC8) {
        return -FLASH_UNSPPORT;
    }

    m_param.mid = manuf_id;
    m_param.did = device_id;

    switch (device_id) {
        case 0x00F2: // gd5f2G 3.3V
            gd5fxx_FLASH_CHIP_SIZE = 256 * 1024 * 1024; // 256MB 
            m_param.feature |= NAND_ECC_STAT_SUP;
            m_gd5fxx_die_size = 128 * 1024 * 1024;
            break;
        case 0x00F1: // gd5f1G 3.3V
            gd5fxx_FLASH_CHIP_SIZE = 128 * 1024 * 1024; // 128MB 
            m_param.feature |= NAND_ECC_STAT_SUP;
            break;
        case 0x00c9: // gd5f1G 1.8V
            gd5fxx_FLASH_CHIP_SIZE = 128 * 1024 * 1024; // 128MB 
            m_param.feature |= NAND_ECC_STAT_SUP;
            m_param.feature |= NAND_CACHE_RD_SUP;
            break;
        case 0x00d9: // gd5f1G4UEY1H 3.3V
            gd5fxx_FLASH_CHIP_SIZE = 128 * 1024 * 1024; // 128MB
            m_param.feature |= NAND_ECC_STAT_SUP;
            m_param.feature |= NAND_CACHE_RD_SUP;
            break;
        default:
            return -FLASH_UNSPPORT;
    }
    
    m_param.chip_size   = gd5fxx_FLASH_CHIP_SIZE;
    m_param.block_size  = GD5FXX_BLOCK_SIZE;
    m_param.page_size   = GD5FXX_PAGE_SIZE;
    m_param.extra_size  = GD5FXX_EXTRA_SIZE;

    if (quad_rw) {
        ret = gd5fxx_enable_quad_mode();
    }
    else {
        ret = gd5fxx_disable_quad_mode();
    }

    int max_blk;
    max_blk = m_param.chip_size / m_param.block_size;
    m_bblk_lut.lut = aiva_malloc(max_blk);
    m_bblk_lut.max_blk = max_blk;
    memset(m_bblk_lut.lut, 0, max_blk);

    configASSERT(m_bblk_lut.lut != NULL);

    m_gd5fxx_inited = (ret == 0) ? 1 : 0;
    nand_bblk_lut_init(&m_bblk_lut, gd5fxx_is_blk_good);
    
    return ret;
}

int gd5fxx_release()
{
    m_gd5fxx_inited = 0;
    if (m_bblk_lut.lut) {
        aiva_free(m_bblk_lut.lut);
    }
    return FLASH_OK;
}

int gd5fxx_get_param(nand_flash_param_t *pparam)
{
    if (!m_gd5fxx_inited) {
        LOGE (__func__, "gd5fxx hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    *pparam = m_param;
    return 0;
}

int gd5fxx_mark_bad_blk(uint16_t blk, uint32_t badblockmarker)
{
    int ret;
    ret = gd5fxx_page_program(blk, 0, GD5FXX_PAGE_SIZE, (uint8_t *)&badblockmarker, 4);
    return ret;
}

// TODO
int gd5fxx_lock(uint32_t addr, uint32_t length)
{
    int ret;
    uint8_t feature_prot_cfg_value;
    uint8_t i;
    uint32_t addr_bound;
    addr_bound = addr + length;
    /*translate addr_bound to page address*/
    addr_bound = addr_bound / GD5FXX_PAGE_SIZE;
    for(i = 0; i < sizeof(feature_prot_cfg_lut) / sizeof(feature_prot_cfg_lut[0]); i++)
    {
        if(addr_bound <= feature_prot_cfg_lut[i].addr_protected_bound)
        {
            feature_prot_cfg_value = feature_prot_cfg_lut[i].feature_prot_cfg_value;
            break;
        }
    }

    ret = gd5fxx_write_status(FEATURE_PROT_ADDR, feature_prot_cfg_value);
    CHECK_RET(ret);

    return FLASH_OK;
}

// TODO
int gd5fxx_unlock(uint32_t addr, uint32_t length)
{
    int ret;
    uint8_t feature_prot_cfg_value;

    feature_prot_cfg_value = 0;

    ret = gd5fxx_write_status(FEATURE_PROT_ADDR, feature_prot_cfg_value);
    CHECK_RET(ret);

    return FLASH_OK;
}

#define GIGA_NAND_OPERATION \
        .init      = gd5fxx_init,\
        .release   = gd5fxx_release,\
        .read      = gd5fxx_read,\
        .write     = gd5fxx_write,\
        .page_prog = gd5fxx_page_program,\
        .blk_erase = gd5fxx_blk_erase,\
        .get_param = gd5fxx_get_param,\
        .lock      = gd5fxx_lock,\
        .unlock    = gd5fxx_unlock,\
        .read_unique_id = gd5fxx_read_unique_id


static nand_flash_drv_t gd5fxx_nand_drivers[] = {
    // --------------- GIGA NAND ----------------
    {
        .manuf_id  = 0xc8,
        .device_id = 0x00f1,
        GIGA_NAND_OPERATION,
    },
    {
        .manuf_id  = 0xc8,
        .device_id = 0x00c9,
        GIGA_NAND_OPERATION,
    },
    {
        .manuf_id  = 0xc8,
        .device_id = 0x00d9,
        GIGA_NAND_OPERATION,
    },
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     gd5fxx_nand_c8_00f1,    0,      &gd5fxx_nand_drivers[0],    NULL);
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     gd5fxx_nand_c8_00c9,    0,      &gd5fxx_nand_drivers[1],    NULL);
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     gd5fxx_nand_c8_00d9,    0,      &gd5fxx_nand_drivers[2],    NULL);

