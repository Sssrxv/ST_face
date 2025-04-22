/* Winbond NAND flash driver */
#include "mx35lfxxe4.h"
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

#define MX35LF_PAGE_SIZE              (2048)
#define MX35LF_EXTRA_SIZE             (64)
#define MX35LF_BLOCK_SIZE             (128 * 1024)
#define MX35LF_PAGE_NUM_PER_BLK       (MX35LF_BLOCK_SIZE/MX35LF_PAGE_SIZE)

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
#define FLASH_READ              0x03    /* read data from cache */
#define FLASH_READ_FAST         0x0B    /* read data from cache */
#define FLASH_QREAD_FAST        0x6B    /* quad-read data from cache */

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

static feature_prot_cfg feature_prot_cfg_lut_2g[]={
    {0x07ff, (0b00110) << 1},
    {0x0fff, (0b01010) << 1},
    {0x1fff, (0b01110) << 1},
    {0x3fff, (0b10010) << 1},
    {0x7fff, (0b10110) << 1},
    {0xffff, (0b11010) << 1},
    {0xfffff,(0b11101) << 1},
};
static uint32_t spi_bus_no = 0;
static uint32_t spi_chip_select = 0;
static uint32_t mx35lfxxe4_FLASH_CHIP_SIZE = (128 * 1024 * 1024);
static nand_flash_param_t m_param = {0};

static volatile int m_mx35lfxxe4_inited    = 0;
static volatile int m_mx35lfxxe4_cur_die   = 0;
static volatile int m_mx35lfxxe4_die_size  = 0;
static bblk_lut_t m_bblk_lut;

int mx35lfxxer_mark_bad_blk(uint16_t blk, uint32_t badblockmarker);

static int (*mx35lfxxe4_read_page_func)(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc);
static int (*mx35lfxxe4_page_program_func)(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len);

int mx35lfxxe4_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc);

static int mx35lfxxe4_wait_for_ready();

static int mx35lfxxe4_receive_data(uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
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

static int mx35lfxxe4_send_data(uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
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

static int mx35lfxxe4_receive_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    int ret;
    
    /*spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, DATALENGTH);*/
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(spi_bus_no, 8/*instrction length*/, 16/*address length*/, 8/*wait cycles*/,
                          SPI_AITM_STANDARD/*spi address trans mode*/);
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

static int mx35lfxxe4_send_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    
    /*spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, DATALENGTH);*/
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

static int mx35lfxxe4_select_die(uint8_t die_num)
{
    int ret;
    uint8_t cmd[2];

    cmd[0] = FLASH_SELECT_DIE;
    cmd[1] = die_num;

    ret = mx35lfxxe4_send_data(cmd, 2, NULL, 0);
    
    if (ret == 0) {
        LOGI(__func__, "select die %d suc!", die_num);
        m_mx35lfxxe4_cur_die = die_num;
    }

    return ret;
}


static int mx35lfxxe4_read_status(uint8_t reg, uint8_t *reg_data)
{
    int ret;
    uint8_t cmd[2] = {FLASH_GFEAT, reg};
    
    ret = mx35lfxxe4_receive_data(cmd, 2, reg_data, 1);
    CHECK_RET(ret);

    return ret;
}

static int mx35lfxxe4_is_busy(void)
{
    uint8_t status = 0;
    
    mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &status);
    if (status & 0x1) {
        return 1;
    }
    return 0;
}

static int mx35lfxxe4_wait_for_ready()
{
    uint32_t timeout;

    timeout = 20 * 1000 * 100; // 20 sec

    while(timeout--) {
        if (!mx35lfxxe4_is_busy()) {
            return FLASH_OK;
        }
        aiva_usleep(10);
    }
    return -FLASH_ERROR_TIMEOUT;
}

static int mx35lfxxe4_write_status(uint8_t reg, uint8_t reg_data)
{
    int ret;
    uint8_t cmd[3] = {FLASH_SFEAT, reg, reg_data};

    ret = mx35lfxxe4_send_data(cmd, 3, 0, 0);
    CHECK_RET(ret);
    
    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);

    return ret;
}


// mx35lfxxe4_write_enable() must be called before any change to the 
// device such as write, erase. It also unlocks the blocks.
static int mx35lfxxe4_write_enable(void)
{
    int ret;
    uint8_t prot;
#ifndef POTECT_SUPPORT
    /* make sure it is not locked firstly */
    ret = mx35lfxxe4_read_status(FEATURE_PROT_ADDR, &prot);
    CHECK_RET(ret);

    if (prot != 0) {
        prot = 0;
        ret = mx35lfxxe4_write_status(FEATURE_PROT_ADDR, prot);
        CHECK_RET(ret);
    }
#endif
    uint8_t cmd[1] = {FLASH_WREN};

    ret = mx35lfxxe4_send_data(cmd, 1, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    if (ret < 0) {
        return  ret;
    }
    
    ret = mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &prot);
    CHECK_RET(ret);

    if (!(prot & STAT_WEL)) {
        return -FLASH_ERROR_STATUS;
    }

    return ret;
}

/* Set ECC-E in configuration register */
static int mx35lfxxe4_enable_ecc()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status |= FEAT_ECC_EN;

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}

static int mx35lfxxe4_enable_quad()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status |= FEAT_QUAD_EN;

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
/*Clear ECC-E in configuration register*/
static int mx35lfxxe4_disable_ecc()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status &= (~FEAT_ECC_EN);

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
static int mx35lfxxe4_disable_quad()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status &= (~FEAT_QUAD_EN);

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}
/* Set BUF in configuration register */
static int mx35lfxxe4_enable_buf()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status |= FEAT_BUF_EN;

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}

/*Clear BUF in configuration register*/
static int mx35lfxxe4_disable_buf()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_FEAT_ADDR, &status);
    CHECK_RET(ret);

    status &= (~FEAT_BUF_EN);

    ret = mx35lfxxe4_write_status(FEATURE_FEAT_ADDR, status);
    CHECK_RET(ret);

    return FLASH_OK;
}


static int mx35lfxxe4_is_ecc_good()
{
    int ret;
    uint8_t status;

    ret = mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if ((status & STAT_ECC_MASK) >= STAT_ECC_UNCORR) {
        return -FLASH_ERROR_STATUS;
    }
    
    return FLASH_OK;
}

static int _mx35lfxxe4_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }
    
    if (check_ecc) {
        ret = mx35lfxxe4_enable_ecc();
        CHECK_RET(ret);
    }
    else {
        ret = mx35lfxxe4_disable_ecc();
        CHECK_RET(ret);
    }

    page_addr = GET_PAGE_ADDR(blk, page);

    cmd[0] = FLASH_PREAD;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // PAGE READ (13h), transfer data from NAND flash array to cache
    ret = mx35lfxxe4_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);

    if (check_ecc) {
        ret = mx35lfxxe4_is_ecc_good();
        if (ret < 0) {
            LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
            nand_bblk_lut_set(&m_bblk_lut, blk, 1);
            mx35lfxxer_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
            return ret;
        }
    }

    col_addr = GET_COL_ADDR(page_offset);
    
    cmd[0] = FLASH_READ;
    cmd[1] = (col_addr >> 8) & 0xff;
    cmd[2] = col_addr & 0xff;
    cmd[3] = 0; // dummy byte
    
    // READ (03h), read from cache
    ret = mx35lfxxe4_receive_data(cmd, 4, rx_buf, rx_len);
    CHECK_RET(ret);

    return FLASH_OK;
}

static int _mx35lfxxe4_quad_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc)
{
    int ret;
    uint16_t page_addr, col_addr;
    uint8_t cmd[4];
    uint32_t read_cmd[2];
    
    if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
        LOGE(__func__, "bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }
    
    if (check_ecc) {
        ret = mx35lfxxe4_enable_ecc();
        CHECK_RET(ret);
    }
    else {
        ret = mx35lfxxe4_disable_ecc();
        CHECK_RET(ret);
    }

    page_addr = GET_PAGE_ADDR(blk, page);

    cmd[0] = FLASH_PREAD;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // PAGE READ (13h), transfer data from NAND flash array to cache
    ret = mx35lfxxe4_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);

    if (check_ecc) {
        ret = mx35lfxxe4_is_ecc_good();
        if (ret < 0) {
            LOGE(__func__, "ECC error: blk %d, page %d.", blk, page);
            nand_bblk_lut_set(&m_bblk_lut, blk, 1);
            mx35lfxxer_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
            return ret;
        }
    }
    
    col_addr = GET_COL_ADDR(page_offset);
    read_cmd[0] = FLASH_QREAD_FAST;
    read_cmd[1] = col_addr;
    
    
    // Quad READ (EBh), read from cache
    ret = mx35lfxxe4_receive_data_enhanced(read_cmd, 2, rx_buf, rx_len);
    CHECK_RET(ret);

    return FLASH_OK;
}

// Read the 1st byte of the 64-byte spare area in the 1st page and 2nd page.
// Non-FFh data indicates bad block.
static int mx35lfxxe4_is_blk_good(uint16_t blk)
{
    int ret;
    uint8_t byte0, byte1;
    uint32_t first_dword;

    int check_ecc;
    
    // 1. disable ecc to read the 64-byte extra data of the 1st page
    check_ecc = 0;
    ret = mx35lfxxe4_read_page(blk, 0, MX35LF_PAGE_SIZE, (uint8_t *)&first_dword, 4, check_ecc);
    CHECK_RET(ret);

    // 2. Non-FFh checks.
    byte0 = first_dword & 0xff;
    byte1 = (first_dword >> 8) & 0xff;
    if ((byte0 != 0xff) || (byte1 != 0xff)) {
        LOGE(__func__, "Bad blk %d", blk);
        return -FLASH_ERROR_STATUS;
    }
    
    return FLASH_OK;
}

static int _mx35lfxxe4_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
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
    
    ret = mx35lfxxe4_enable_ecc();
    CHECK_RET(ret);

    ret = mx35lfxxe4_write_enable();
    CHECK_RET(ret);

    col_addr = GET_COL_ADDR(page_offset);
    cmd[0] = FLASH_PROG; // FLASH_PROG_RAN
    cmd[1] = (col_addr >> 8) & 0xff;
    cmd[2] = col_addr & 0xff;

    // program to cache (02h/84h)
    ret = mx35lfxxe4_send_data(cmd, 3, tx_buf, tx_len);
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, page);
    cmd[0] = FLASH_PEXEC;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // execute program (10h)
    ret = mx35lfxxe4_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);

    ret = mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_PFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        mx35lfxxer_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}

static int _mx35lfxxe4_quad_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
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
    
    ret = mx35lfxxe4_enable_ecc();
    CHECK_RET(ret);

    ret = mx35lfxxe4_write_enable();
    CHECK_RET(ret);

    col_addr    = GET_COL_ADDR(page_offset);
    prog_cmd[0] = FLASH_QPROG;
    prog_cmd[1] = col_addr;

    // program to cache (32h/34h)
    ret = mx35lfxxe4_send_data_enhanced(prog_cmd, 2, tx_buf, tx_len);
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, page);
    cmd[0] = FLASH_PEXEC;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    // execute program (10h)
    ret = mx35lfxxe4_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);
    
    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);
    
    ret = mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_PFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        mx35lfxxer_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}


static int mx35lfxxe4_blk_program(uint16_t blk, uint8_t *tx_buf, uint32_t tx_len)
{
    uint16_t page;
    uint32_t tx_remain;
    int ret = FLASH_OK;

    uint16_t page_cnt = tx_len / MX35LF_PAGE_SIZE;

    tx_remain = tx_len & (MX35LF_PAGE_SIZE - 1);

    configASSERT(page_cnt <= MX35LF_PAGE_NUM_PER_BLK);

    for (page = 0; page < page_cnt; page++) {
        ret = mx35lfxxe4_page_program(blk, page, 0, tx_buf, MX35LF_PAGE_SIZE);
        if (ret < 0) {
            break;
        }
        tx_buf += MX35LF_PAGE_SIZE;
    }
    if (tx_remain) {
        ret = mx35lfxxe4_page_program(blk, page, 0, tx_buf, tx_remain);
    }

    return ret;
}

static int mx35lfxxe4_quad_blk_program(uint16_t blk, uint8_t *tx_buf, uint32_t tx_len)
{
    uint16_t page;
    uint32_t tx_remain;
    int ret = FLASH_OK;

    uint16_t page_cnt = tx_len / MX35LF_PAGE_SIZE;

    tx_remain = tx_len & (MX35LF_PAGE_SIZE - 1);

    configASSERT(page_cnt <= MX35LF_PAGE_NUM_PER_BLK);

    for (page = 0; page < page_cnt; page++) {
        ret = _mx35lfxxe4_quad_page_program(blk, page, 0, tx_buf, MX35LF_PAGE_SIZE);
        if (ret < 0) {
            break;
        }
        tx_buf += MX35LF_PAGE_SIZE;
    }
    if (tx_remain) {
        ret = _mx35lfxxe4_quad_page_program(blk, page, 0, tx_buf, tx_remain);
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

    blk  = addr / MX35LF_BLOCK_SIZE;
    blk_remain = addr & (MX35LF_BLOCK_SIZE - 1);
    page = blk_remain / MX35LF_PAGE_SIZE;
    page_remain = blk_remain & (MX35LF_PAGE_SIZE - 1);

    *blk_ptr  = blk & 0xffff;
    *page_ptr = page & 0xffff;
    *page_off_ptr = page_remain & 0xffff;
}

static int mx35lfxxe4_enable_quad_mode(void)
{
    mx35lfxxe4_enable_quad();
    mx35lfxxe4_page_program_func = _mx35lfxxe4_quad_page_program;
    mx35lfxxe4_read_page_func    = _mx35lfxxe4_quad_read_page;

    return 0;
}

static int mx35lfxxe4_disable_quad_mode(void)
{
    mx35lfxxe4_disable_quad();
    mx35lfxxe4_page_program_func = _mx35lfxxe4_page_program;
    mx35lfxxe4_read_page_func    = _mx35lfxxe4_read_page;

    return 0;
}


int mx35lfxxe4_read_id(uint8_t *manuf_id, uint16_t *device_id)
{
    uint8_t cmd[2] = {FLASH_RDID, 0};
    uint8_t data[3] = {0};
    int ret;

    ret = mx35lfxxe4_receive_data(cmd, 2, data, 3);
    if (ret < 0) {
        return ret;
    }
    if (data[0] != 0xC2) {
        return -FLASH_UNSPPORT;
    }
    *manuf_id = data[0];
    //*device_id = ((uint16_t)(data[1]) << 8) | (uint16_t)(data[2]); 
    *device_id = data[1]; 
    LOGD(__func__, "manuf_id = %x,device_id = %x\n",*manuf_id,*device_id);
    return FLASH_OK;
}

int mx35lfxxe4_read_unique_id(flash_unique_id_t *unique_id)
{
    LOGE(__func__, "Unsupported now!");

    return -FLASH_UNSPPORT;
}

int mx35lfxxe4_read_page(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *rx_buf, int rx_len, int check_ecc)
{
    int ret;
    uint32_t addr;
    uint8_t  die_num;
    
    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    
    if (m_param.feature & NAND_MULT_DIE_SUP) {
        addr = (uint32_t)blk * MX35LF_BLOCK_SIZE + (uint32_t)page * MX35LF_PAGE_SIZE + page_offset;
        die_num = addr / m_mx35lfxxe4_die_size;
        if (die_num != m_mx35lfxxe4_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_mx35lfxxe4_cur_die, die_num, addr);
            ret = mx35lfxxe4_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    
    ret = mx35lfxxe4_read_page_func(blk, page, page_offset, rx_buf, rx_len, check_ecc);
   
    return ret;
}

int mx35lfxxe4_page_program(uint16_t blk, uint16_t page, uint16_t page_offset, 
        uint8_t *tx_buf, int tx_len)
{
    int ret;
    uint32_t addr;
    uint8_t  die_num;

    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    if (m_param.feature & NAND_MULT_DIE_SUP) {
        addr = (uint32_t)blk * MX35LF_BLOCK_SIZE + (uint32_t)page * MX35LF_PAGE_SIZE + page_offset;
        die_num = addr / m_mx35lfxxe4_die_size;
        if (die_num != m_mx35lfxxe4_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_mx35lfxxe4_cur_die, die_num, addr);
            ret = mx35lfxxe4_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    ret = mx35lfxxe4_page_program_func(blk, page, page_offset, tx_buf, tx_len);
    return ret;
}

int mx35lfxxe4_blk_erase(uint32_t addr, int force_erase)
{
    int ret;
    uint8_t cmd[4];
    uint16_t page_addr;
    uint8_t status;

    uint16_t blk;
    
    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    
    if (m_param.feature & NAND_MULT_DIE_SUP) {
        uint8_t die_num = addr / m_mx35lfxxe4_die_size;
        if (die_num != m_mx35lfxxe4_cur_die) {
            LOGI(__func__, "cur die num %d, expected die num %d, addr 0x%x", m_mx35lfxxe4_cur_die, die_num, addr);
            ret = mx35lfxxe4_select_die(die_num);
            CHECK_RET(ret);
        }
    }
    
    blk = addr / MX35LF_BLOCK_SIZE;

    /*ret = mx35lfxxe4_is_blk_good(blk);*/
    /*CHECK_RET(ret);*/
    if (!force_erase) {
        if (nand_bblk_lut_get(&m_bblk_lut, blk) != 0) {
            LOGE(__func__, "bad blk %d", blk);
            return -FLASH_ERROR_STATUS;
        }
    }

    ret = mx35lfxxe4_write_enable();
    CHECK_RET(ret);

    page_addr = GET_PAGE_ADDR(blk, 0);
    cmd[0] = FLASH_BERASE;
    cmd[1] = 0; // dummy byte
    cmd[2] = (page_addr >> 8) & 0xff;
    cmd[3] = page_addr & 0xff;

    ret = mx35lfxxe4_send_data(cmd, 4, 0, 0);
    CHECK_RET(ret);

    ret = mx35lfxxe4_wait_for_ready();
    CHECK_RET(ret);

    ret = mx35lfxxe4_read_status(FEATURE_STAT_ADDR, &status);
    CHECK_RET(ret);

    if (status & STAT_EFAIL) {
        nand_bblk_lut_set(&m_bblk_lut, blk, 1);
        mx35lfxxer_mark_bad_blk(blk, FLASH_BAD_BLOCK_MARKER);
        return -FLASH_ERROR_ERASE;
    }
    nand_bblk_lut_set(&m_bblk_lut, blk, 0);

    return FLASH_OK;
}

int mx35lfxxe4_read(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t len = 0;
    uint16_t blk, page, page_offset;
    uint16_t page_remain;
    
    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    // 1. Check whether the addr is legal
    if (addr >= mx35lfxxe4_FLASH_CHIP_SIZE) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > mx35lfxxe4_FLASH_CHIP_SIZE ? (mx35lfxxe4_FLASH_CHIP_SIZE - addr) : length;
    
    blk  = 0;
    page = 0;
    page_offset = 0;

    while (length) {
        len = (length >= FIFO_NAND_LEN) ? FIFO_NAND_LEN : length;
        get_blk_page_and_offset(addr, &blk, &page, &page_offset);
        page_remain = MX35LF_PAGE_SIZE - page_offset;
        if (len > page_remain) {
            uint32_t new_addr = addr + page_remain;
            ret = mx35lfxxe4_read_page(blk, page, page_offset, data_buf, page_remain, 1);
            get_blk_page_and_offset(new_addr, &blk, &page, &page_offset);
            ret = mx35lfxxe4_read_page(blk, page, page_offset, data_buf+page_remain, len-page_remain, 1);
        }
        else {
            ret = mx35lfxxe4_read_page(blk, page, page_offset, data_buf, len, 1);
        }

        if (ret < 0) {
            return ret;
        }
        addr     += len;
        data_buf += len;
        length   -= len;
    }

    return ret;
}

int mx35lfxxe4_write(uint32_t addr, uint8_t *data_buf, uint32_t length)
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

    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }

    // 1. Check whether the addr is legal
    if (addr >= mx35lfxxe4_FLASH_CHIP_SIZE) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > mx35lfxxe4_FLASH_CHIP_SIZE ? (mx35lfxxe4_FLASH_CHIP_SIZE - addr) : length;

    while (length)
    {
        blk_addr   = addr & (~(MX35LF_BLOCK_SIZE - 1));
        blk_offset = addr & (MX35LF_BLOCK_SIZE - 1);
        blk_remain = MX35LF_BLOCK_SIZE - blk_offset;

        write_len = ((length < blk_remain) ? length : blk_remain);

        if (write_len == MX35LF_BLOCK_SIZE && addr == blk_addr) {
            src_buf = data_buf;
        }
        else {
            if (!swap_buf) {
                swap_buf = aiva_malloc(MX35LF_BLOCK_SIZE);
                configASSERT(swap_buf != NULL);
            }
            ret = mx35lfxxe4_read(blk_addr, swap_buf, MX35LF_BLOCK_SIZE);
            // if (ret < 0) {
            //     goto out;
            // }
            pread = swap_buf + blk_offset;
            pwrite = data_buf;
            for (index = 0; index < write_len; index++)
                *pread++ = *pwrite++;
            src_buf = swap_buf;
        }

        ret = mx35lfxxe4_blk_erase(blk_addr, NAND_FORCE_ERASE);
        if (ret < 0) {
            goto out;
        }

        ret = mx35lfxxe4_blk_program(blk_addr/MX35LF_BLOCK_SIZE, src_buf, MX35LF_BLOCK_SIZE);
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

int mx35lfxxe4_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw)
{
    int ret;
    uint8_t manuf_id;
    uint16_t device_id;

    if (m_mx35lfxxe4_inited) {
        mx35lfxxe4_release();
    }

    spi_bus_no = spi_index;
    spi_chip_select = spi_ss;
    spi_init(spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
    spi_set_clk_rate(spi_bus_no, rate);
    
    ret = mx35lfxxe4_read_id(&manuf_id, &device_id);
    if (ret < 0) {
        return ret;
    }

    if (manuf_id != 0xc2) {
        return -FLASH_UNSPPORT;
    }

    m_param.mid = manuf_id;
    m_param.did = device_id;
    
    switch (device_id) {
        case 0x0013: // mx35lf2ge4 
            mx35lfxxe4_FLASH_CHIP_SIZE = 256 * 1024 * 1024; // 256MB 
            m_param.feature |= NAND_ECC_STAT_SUP;
            m_mx35lfxxe4_die_size = 128 * 1024 * 1024;
            break;
        case 0x0012: // mx35lf1ge4
            mx35lfxxe4_FLASH_CHIP_SIZE = 128 * 1024 * 1024; // 128MB 
            m_param.feature |= NAND_ECC_STAT_SUP;
            break;
        default:
            return -FLASH_UNSPPORT;
    }
    
    m_param.chip_size   = mx35lfxxe4_FLASH_CHIP_SIZE;
    m_param.block_size  = MX35LF_BLOCK_SIZE;
    m_param.page_size   = MX35LF_PAGE_SIZE;
    m_param.extra_size  = MX35LF_EXTRA_SIZE;

    if (quad_rw) {
        ret = mx35lfxxe4_enable_quad_mode();
    }
    else {
        ret = mx35lfxxe4_disable_quad_mode();
    }
   
    int max_blk;
    max_blk = m_param.chip_size / m_param.block_size;
    m_bblk_lut.lut = aiva_malloc(max_blk);
    m_bblk_lut.max_blk = max_blk;
    memset(m_bblk_lut.lut, 0, max_blk);

    configASSERT(m_bblk_lut.lut != NULL);


    m_mx35lfxxe4_inited = (ret == 0) ? 1 : 0;
    nand_bblk_lut_init(&m_bblk_lut, mx35lfxxe4_is_blk_good);
    
    return ret;
}

int mx35lfxxe4_release()
{
    m_mx35lfxxe4_inited = 0;
    if (m_bblk_lut.lut) {
        aiva_free(m_bblk_lut.lut);
    }
    return FLASH_OK;
}

int mx35lfxxe4_get_param(nand_flash_param_t *pparam)
{
    if (!m_mx35lfxxe4_inited) {
        LOGE (__func__, "mx35lfxxe4 hasn't been inited!");
        return -FLASH_ERROR_STATUS;
    }
    *pparam = m_param;
    return 0;
}

int mx35lfxxer_mark_bad_blk(uint16_t blk, uint32_t badblockmarker)
{
    int ret;
    ret = mx35lfxxe4_page_program(blk, 0, MX35LF_PAGE_SIZE, (uint8_t *)&badblockmarker, 4);
    return ret;
}

// TODO
int mx35lfxxe4_lock(uint32_t addr, uint32_t length)
{
    int ret;
    uint8_t feature_prot_cfg_value;
    uint8_t i;
    uint32_t addr_bound;
    addr_bound = addr + length;
    /*translate addr_bound to page address*/
    addr_bound = addr_bound / MX35LF_PAGE_SIZE;
    if(m_param.chip_size   == 128 * 1024 * 1024)
    {
         for(i = 0; i < sizeof(feature_prot_cfg_lut) / sizeof(feature_prot_cfg_lut[0]); i++)
        {
            if(addr_bound <= feature_prot_cfg_lut[i].addr_protected_bound)
            {
                feature_prot_cfg_value = feature_prot_cfg_lut[i].feature_prot_cfg_value;
                break;
            }
        }
    }
    else if(m_param.chip_size == 256 * 1024 *1024) 
    {
        for(i = 0; i < sizeof(feature_prot_cfg_lut_2g) / sizeof(feature_prot_cfg_lut_2g[0]); i++)
        {
            if(addr_bound <= feature_prot_cfg_lut_2g[i].addr_protected_bound)
            {
                feature_prot_cfg_value = feature_prot_cfg_lut_2g[i].feature_prot_cfg_value;
                break;
            }
        }
    }
    ret = mx35lfxxe4_write_status(FEATURE_PROT_ADDR, feature_prot_cfg_value);
    CHECK_RET(ret);

    return FLASH_OK;
}

// TODO
int mx35lfxxe4_unlock(uint32_t addr, uint32_t length)
{
    int ret;
    uint8_t feature_prot_cfg_value;

    feature_prot_cfg_value = 0;

    ret = mx35lfxxe4_write_status(FEATURE_PROT_ADDR, feature_prot_cfg_value);
    CHECK_RET(ret);

    return FLASH_OK;
}

#define MXIC_NAND_OPERATION \
        .init      = mx35lfxxe4_init,\
        .release   = mx35lfxxe4_release,\
        .read      = mx35lfxxe4_read,\
        .write     = mx35lfxxe4_write,\
        .page_prog = mx35lfxxe4_page_program,\
        .blk_erase = mx35lfxxe4_blk_erase,\
        .get_param = mx35lfxxe4_get_param,\
        .lock      = mx35lfxxe4_lock,\
        .unlock    = mx35lfxxe4_unlock,\
        .read_unique_id = mx35lfxxe4_read_unique_id

static nand_flash_drv_t mx35lfxx_nand_drivers[] = {
    // --------------- MXIC NAND ----------------
    {
        .manuf_id  = 0xc2,
        .device_id = 0x0012,
        MXIC_NAND_OPERATION,
    },
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     mx35lfxx_nand_c2_0012,    0,      &mx35lfxx_nand_drivers[0],    NULL);
