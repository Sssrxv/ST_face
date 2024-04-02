#include "mx25lxx.h"
#include "flash.h"
#include "spi.h"
#include "aiva_sleep.h"
#include "aiva_utils.h"
#include "dmac.h"
#include "syslog.h"
#include "sysctl.h"
#include "aiva_malloc.h"
#include "udevice.h"
#include "nor_flash_dev.h"
#include <string.h>

/* --------------------------------------------------------------------------*/
/*#define FLASH_USE_DMA*/
#if 0
#define M_PAGE_SIZE      (256)
#define M_SEC_SIZE       (4  * 1024)
#define M_BLK_SIZE       (64 * 1024)
#define M_BLK32_SIZE     (32 * 1024)
#endif

#define mx25lxx_FLASH_PAGE_SIZE     (256)
#define mx25lxx_FLASH_SECTOR_SIZE   (4096)
/* --------------------------------------------------------------------------*/
static uint8_t m_spi_bus_no     = SPI_DEVICE_0;
static uint8_t m_spi_chip_select      = SPI_CHIP_SELECT_0;
static uint8_t m_quad_read   = 0;
static uint8_t m_quad_write  = 0;
static uint32_t m_total_size = 0;
static uint32_t m_chip_size = (2 * 1024 * 1024);
static nor_flash_param_t m_param = {0};
/* --------------------------------------------------------------------------*/

#define MX_CMD_WREN         (0x06) // Write Enable
#define MX_CMD_WRDI         (0x04) // Write Disable
#define MX_CMD_RDID         (0x9F) // Read Identification
#define MX_CMD_RDST         (0x05) // Read Status Register
#define MX_CMD_RDCR         (0x15) // Read Configuration Register
#define MX_CMD_WRSR         (0x01) // Write Status Register
#define MX_CMD_READ         (0x03) // Standard SPI Read
#define MX_CMD_4READ        (0xEB) // 4xIO read

#define MX_CMD_SE           (0x20) // Sector Erase 4KB
#define MX_CMD_BE           (0xD8) // Block Erase 64KB
#define MX_CMD_BE32K        (0x52) // Block Erase 32KB
#define MX_CMD_CE           (0x60) // Chip Erase
#define MX_CMD_PP           (0x02) // Page Program
#define MX_CMD_4PP          (0x38) // 4xIO Page Prgoram

#define MX_CMD_DP           (0xB9) // Deep Power Down
#define MX_CMD_RES          (0xAB) // Release from Deep Power Down
#define MX_CMD_RDSCUR       (0x2B) // Read Security Register

/* Status Register Bits */
#define MX_STATUS_WIP       (1 << 0) // Writing In Progress
#define MX_STATUS_WEL       (1 << 1) // Write Enable Latch
#define MX_STATUS_QE        (1 << 6) // Quad Enable
#define MX_STATUS_SRWD      (1 << 7) // Status Write Disable

/* Configuration Register Bits */
#define MX_CONFIG_ODS       (1 << 0) // Output Drive Strength
#define MX_CONFIG_TB        (1 << 3) // Top/Bottom Protect Select
#define MX_CONFIG_DC        (1 << 6) // Dummy Cycle for 2READ/4READ

/* Security Register Bits */
#define MX_SEC_OTP          (1 << 0) // Secured OTP Indicator
#define MX_SEC_LDSO         (1 << 1) // Lock-down first 4K-bit Secured OTP
#define MX_SEC_PSB          (1 << 2) // Program Suspend Status
#define MX_SEC_ESB          (1 << 3) // Erase Suspend Status
#define MX_SEC_P_FAIL       (1 << 5) // Program Fail: 0 - succeed, 1 - failed
#define MX_SEC_E_FAIL       (1 << 6) // Erase Fail: 0 - succeed, 1 - failed

#define mx25l16_FLASH_CHIP_SIZE             (2*1024*1024)
#define mx25l32_FLASH_CHIP_SIZE             (4*1024*1024)
#define mx25l64_FLASH_CHIP_SIZE             (8*1024*1024)

typedef struct feature_prot_cfg {
    uint32_t addr_protected_bound;
    uint8_t  feature_prot_cfg_value;
} feature_prot_cfg_t;

typedef int (*page_program_func_t)(uint32_t addr, uint8_t * data_buf, uint32_t length);
typedef int (*read_func_t)(uint32_t addr, uint8_t *data_buf, uint32_t length);

static feature_prot_cfg_t feature_prot_cfg_lut_16m[]={
    {mx25l16_FLASH_CHIP_SIZE /  1, (0b0110) << 2},
};

static feature_prot_cfg_t feature_prot_cfg_lut_32m[]={
    {mx25l32_FLASH_CHIP_SIZE / 64, (0b0001) << 2},
    {mx25l32_FLASH_CHIP_SIZE / 32, (0b0010) << 2},
    {mx25l32_FLASH_CHIP_SIZE / 16, (0b0011) << 2},
    {mx25l32_FLASH_CHIP_SIZE /  8, (0b0100) << 2},
    {mx25l32_FLASH_CHIP_SIZE /  4, (0b0101) << 2},
    {mx25l32_FLASH_CHIP_SIZE /  2, (0b0110) << 2},
    {mx25l32_FLASH_CHIP_SIZE /  1, (0b0111) << 2},
};

static feature_prot_cfg_t feature_prot_cfg_lut_64m[]={
    {mx25l64_FLASH_CHIP_SIZE / 128,(0b0001) << 2},
    {mx25l64_FLASH_CHIP_SIZE / 64, (0b0001) << 2},
    {mx25l64_FLASH_CHIP_SIZE / 32, (0b0010) << 2},
    {mx25l64_FLASH_CHIP_SIZE / 16, (0b0011) << 2},
    {mx25l64_FLASH_CHIP_SIZE /  8, (0b0100) << 2},
    {mx25l64_FLASH_CHIP_SIZE /  4, (0b0101) << 2},
    {mx25l64_FLASH_CHIP_SIZE /  2, (0b0110) << 2},
    {mx25l64_FLASH_CHIP_SIZE /  1, (0b0111) << 2},
};

/* --------------------------------------------------------------------------*/
static int mx25lxx_standard_read(uint32_t addr, uint8_t *rx_buf, uint32_t length);
static int mx25lxx_quad_read(uint32_t addr, uint8_t *rx_buf, uint32_t length);
static page_program_func_t m_read_func;

static int _mx25lxx_page_program(uint32_t addr, uint8_t *data_buf,uint32_t length);
static int _mx25lxx_quad_page_program(uint32_t addr, uint8_t *data_buf,uint32_t length);
static read_func_t m_page_program_func;

static int mx25lxx_wait_for_ready();

static int mx25lxx_recv_data(uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff,uint32_t rx_len)
{
    int ret;
    spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_standard(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    ret = spi_receive_data_standard_dma(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int mx25lxx_recv_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff,uint32_t rx_len)
{
    int ret;

    spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(m_spi_bus_no, 8/*instrction length*/, 32/*address length*/, 4/*wait cycles*/,SPI_AITM_ADDR_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_multiple(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    ret = spi_receive_data_multiple_dma(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int mx25lxx_send_data(uint8_t *cmd_buff, uint8_t cmd_len , uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_standard(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_standard_dma(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int mx25lxx_send_data_enhanced(uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;

    /*spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W8);*/
    spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(m_spi_bus_no, 8/*instrction length*/, 24/*address length*/, 0/*wait cycles*/, SPI_AITM_ADDR_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_multiple(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_multiple_dma(m_spi_bus_no, m_spi_chip_select, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    return FLASH_OK;
}


static int mx25lxx_write_enable(int flag)
{
    uint8_t cmd[1];
    int ret;
    if (flag) {
        cmd[0] = MX_CMD_WREN;
    }
    else {
        cmd[0] = MX_CMD_WRDI;
    }
    ret = mx25lxx_send_data(cmd,1, NULL, 0);
    return ret;
}


static int mx25lxx_read_config(uint8_t *config)
{
    uint8_t rx_buf[1];
    int ret;
    uint8_t cmd = {MX_CMD_RDCR};
    ret = mx25lxx_recv_data(&cmd, 1, rx_buf, sizeof(rx_buf));
    *config = rx_buf[0];
    return ret;
}


static int mx25lxx_write_status(uint8_t status)
{
    uint8_t tx_buf[2];
    int ret;
    uint8_t cmd[1] = {MX_CMD_WRSR};
    // 1st byte: status register
    tx_buf[0] = status;
    // 2nd byte: configuration register
    ret = mx25lxx_read_config(&tx_buf[1]);
    if (ret < 0) {
        return ret;
    }
    /*tx_buf[1] |= 0x08;*/

    mx25lxx_write_enable(1);

    ret = mx25lxx_send_data(cmd,1, tx_buf, 2);
    if (ret < 0) {
        return ret;
    }

    ret = mx25lxx_wait_for_ready();
    if (ret < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }

    return ret;
}


static int mx25lxx_read_status(uint8_t *status)
{
    uint8_t rx_buf[1];
    uint8_t cmd = MX_CMD_RDST;
    int ret = mx25lxx_recv_data(&cmd, 1, rx_buf, sizeof(rx_buf));
    *status = rx_buf[0];
    return ret;
}


static int mx25lxx_read_sec_status(uint8_t *status)
{
    uint8_t rx_buf[1];
    uint8_t cmd = MX_CMD_RDSCUR;
    int ret = mx25lxx_recv_data(&cmd,1, rx_buf, sizeof(rx_buf));
    *status = rx_buf[0];
    return ret;
}


static int mx25lxx_wait_for_ready()
{
    uint32_t timeout;
    uint8_t  status;
    int ret;

    timeout = 20 * 1000 * 100; // 20 sec

    while(timeout--) {
        /*status = mx25lxx_read_status();*/
        ret = mx25lxx_read_status(&status);
        if (ret < 0) {
            return ret;
        }
        if (!(status & MX_STATUS_WIP)) {
            return 0;
        }
        aiva_usleep(10);
    }

    return -1;
}


static int mx25lxx_enable_quad_mode()
{
    uint8_t status;
    int ret;

    /*status = mx25lxx_read_status();*/
    ret = mx25lxx_read_status(&status);
    if (ret < 0) {
        return ret;
    }

    status |= MX_STATUS_QE | MX_STATUS_WEL;

    ret = mx25lxx_write_status(status);

    //aiva_printf("mx25lxx_enable_quad_mode,status = %x\n",status);
    m_read_func = mx25lxx_quad_read;
    m_page_program_func = _mx25lxx_quad_page_program;

    return ret;
}


static int mx25lxx_disable_quad_mode()
{
    uint8_t status;
    int ret;

    /*status = mx25lxx_read_status();*/
    ret = mx25lxx_read_status(&status);
    if (ret < 0) {
        return ret;
    }

    status &= (~MX_STATUS_QE);
    ret = mx25lxx_write_status(status);
    m_read_func = mx25lxx_standard_read;
    m_page_program_func = _mx25lxx_page_program;
    return ret;
}


static int mx25lxx_write_prot_cfg(uint8_t prot_value)
{
    int ret;
    uint8_t reg_data1 = 0;

    ret = mx25lxx_read_status(&reg_data1);
    if (ret < 0) {
        return ret;
    }
    
    reg_data1 &= 0xC3;
    reg_data1 |= prot_value;

    mx25lxx_write_status(reg_data1);

    mx25lxx_read_status(&reg_data1);
    return FLASH_OK;
}


static int mx25lxx_erase(uint8_t type, uint32_t addr)
{
    /*uint8_t cmd[4];*/
    uint8_t cmd[4];
    uint8_t status;
    int ret;

    cmd[0] = type;
    cmd[1] = (uint8_t)((addr >> 16) & 0xff);
    cmd[2] = (uint8_t)((addr >> 8) & 0xff);
    cmd[3] = (uint8_t)(addr & 0xff);

    ret = mx25lxx_write_enable(1);
    if (ret < 0) {
        return ret;
    }

    /*status = mx25lxx_read_status();*/
    ret = mx25lxx_read_status(&status);
    if (ret < 0) {
        return ret;
    }

    if (!(status & MX_STATUS_WEL)) {
        return -FLASH_WRITE_DISABLE;
    }
    /*ret = spi_send_data_normal(m_spi_num, m_spi_ss, cmd, 4);*/
    ret = mx25lxx_send_data(cmd, 4, NULL, 0);
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    ret = mx25lxx_wait_for_ready();
    if (ret < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }

    /*status = mx25lxx_read_sec_status();*/
    ret = mx25lxx_read_sec_status(&status);
    if (ret < 0) {
        return ret;
    }
    if (status & MX_SEC_E_FAIL) {
        return -FLASH_ERROR_ERASE;
    }

    return FLASH_OK;
}


int mx25lxx_sector_erase(uint32_t addr)
{
    return mx25lxx_erase(MX_CMD_SE, addr);
}


static int mx25lxx_blk_erase(uint32_t addr)
{
    return mx25lxx_erase(MX_CMD_BE, addr);
}


static int mx25lxx_chip_erase()
{
    /*uint8_t cmd = MX_CMD_CE;*/
    uint8_t cmd = MX_CMD_CE;
    int ret;
    uint8_t status;

    ret = mx25lxx_write_enable(1);
    if (ret < 0) {
        return ret;
    }
    /*ret = spi_send_data_normal(m_spi_num, m_spi_ss, &cmd, 1);*/
    ret = mx25lxx_send_data(&cmd,1, NULL, 0);
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    ret = mx25lxx_wait_for_ready();

    if (ret < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }

    /*status = mx25lxx_read_sec_status();*/
    ret = mx25lxx_read_sec_status(&status);
    if (status & MX_SEC_E_FAIL) {
        return -FLASH_ERROR_ERASE;
    }

    return FLASH_OK;

}


static int _mx25lxx_page_program(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint8_t cmds[4];
    int ret;
    uint8_t status;

    cmds[0] = MX_CMD_PP;
    cmds[1] = (addr >> 16) & 0xff;
    cmds[2] = (addr >> 8)  & 0xff;
    cmds[3] = (addr >> 0)  & 0xff;

    ret = mx25lxx_write_enable(1);
    if (ret < 0) {
        return ret;
    }

    //ret = spi_send_data_multiple(m_spi_num,
    ret = mx25lxx_send_data(cmds, 4, data_buf, length );
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    ret = mx25lxx_wait_for_ready();
    if (ret < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }

    /*status = mx25lxx_read_sec_status();*/
    ret = mx25lxx_read_sec_status(&status);
    if (ret < 0) {
        return ret;
    }
    if (status & MX_SEC_P_FAIL) {
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}


static int _mx25lxx_quad_page_program(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint32_t cmds[2];
    int ret;
    uint8_t status;

    cmds[0] = MX_CMD_4PP;
    cmds[1] = addr;

    ret = mx25lxx_enable_quad_mode();
    if (ret < 0) {
        return ret;
    }
    ret = mx25lxx_write_enable(1);
    if (ret < 0) {
        return ret;
    }

    /*spi_init(m_spi_num, SPI_WORK_MODE_0, SPI_FF_QUAD, 32);*/
    /*spi_init(m_spi_num, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W8);
    spi_init_non_standard(m_spi_num,
            8,      // instruction length
            24,     // address length
            0,      // wait cycles
            SPI_AITM_ADDR_STANDARD
            );
    */
    /*ret = spi_send_data_multiple(m_spi_num, */
    ret = mx25lxx_send_data_enhanced(cmds,
                2,
                data_buf,
                length
                );
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    ret = mx25lxx_wait_for_ready();
    if (ret < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }

    /*status = mx25lxx_read_sec_status();*/
    ret = mx25lxx_read_sec_status(&status);
    if (ret < 0) {
        return ret;
    }
    if (status & MX_SEC_P_FAIL) {
        return -FLASH_ERROR_WRITE;
    }

    return FLASH_OK;
}


int mx25lxx_page_program(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    ret = m_page_program_func(addr, data_buf, mx25lxx_FLASH_PAGE_SIZE);
    return ret;
}


static int mx25lxx_sector_program(uint32_t addr, uint8_t *data_buf)
{
    int ret;
    int i;
    int PAGES_PER_SEC = (mx25lxx_FLASH_SECTOR_SIZE / mx25lxx_FLASH_PAGE_SIZE);

    for (i = 0; i < PAGES_PER_SEC; i++) {
        ret = mx25lxx_page_program(addr, data_buf,mx25lxx_FLASH_PAGE_SIZE);
        if (ret < 0) {
            return ret;
        }
        addr += mx25lxx_FLASH_PAGE_SIZE;
        data_buf += mx25lxx_FLASH_PAGE_SIZE;
    }

    return FLASH_OK;
}


static int mx25lxx_standard_read(uint32_t addr, uint8_t *rx_buf, uint32_t length)
{
    uint8_t cmds[4];
    int ret;

    cmds[0] = MX_CMD_READ;
    cmds[1] = (addr >> 16) & 0xff;
    cmds[2] = (addr >>  8) & 0xff;
    cmds[3] = (addr >>  0) & 0xff;

    //spi_init(m_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
    ret = mx25lxx_recv_data(cmds, 4, rx_buf, length);
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    return FLASH_OK;
}


static int mx25lxx_quad_read(uint32_t addr, uint8_t *rx_buf, uint32_t length)
{
    uint32_t cmds[2];
    int ret;

    cmds[0] = MX_CMD_4READ;
    cmds[1] = addr << 8;
    
    ret = mx25lxx_recv_data_enhanced(cmds, 2, rx_buf, length);

    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    return FLASH_OK;
}


int mx25lxx_read_id(uint8_t *manuf_id, uint16_t *device_id)
{
    uint8_t rx_buf[4];
    int ret;
    uint8_t cmd = MX_CMD_RDID;
    ret = mx25lxx_recv_data(&cmd, 1,  rx_buf, 4);
    if (ret < 0) {
        return ret;
    }

    if (rx_buf[0] != 0xC2) {
        return -FLASH_ERROR;
    }

    *manuf_id = (uint16_t) rx_buf[0];
    *device_id = ((uint16_t)(rx_buf[1]) << 8) | (uint16_t)(rx_buf[2]);

    return FLASH_OK;
}


int mx25lxx_read_unique_id(flash_unique_id_t *unique_id)
{
    LOGE(__func__, "Unsupported now!");
    return -FLASH_UNSPPORT;
}


int mx25lxx_init(uint8_t spi_index, uint8_t spi_ss, uint32_t rate, int quad_rw)
{
    int ret;
    uint8_t manuf_id;
    uint16_t device_id;

    m_spi_bus_no = spi_index;
    m_spi_chip_select = spi_ss;
    spi_init(m_spi_bus_no, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
    spi_set_clk_rate(m_spi_bus_no, rate);

    ret = mx25lxx_read_id(&manuf_id, &device_id);
    if (ret < 0) {
        return ret;
    }

    if (manuf_id != 0xc2) {
        return -FLASH_ERROR;
    }

    m_param.mid = manuf_id;
    m_param.did = device_id;

    switch (device_id) {
        case 0x2018: // mx25l128
            m_chip_size = 16 * 1024 * 1024; // 16MB
            m_param.dev_name = "mx25l128";
            break;
        case 0x2017: // mx25l64
            m_chip_size = 8 * 1024 * 1024;  // 8MB
            m_param.dev_name = "mx25l64";
            break;
        case 0x2537: // mx25e64
            m_chip_size = 8 * 1024 * 1024;  // 8MB
            m_param.dev_name = "mx25e64";
            break;
        case 0x2016: // mx25l32
            m_chip_size = 4 * 1024 * 1024;  // 4MB
            m_param.dev_name = "mx25l32";
            break;
        case 0x2015: // mx25l16
            m_chip_size = 2 * 1024 * 1024;  // 2MB
            m_param.dev_name = "mx25l16";
            quad_rw = 0;                     // don't support quad rw
            break;
        default:
            return -FLASH_UNSPPORT;
    }

    m_param.chip_size   = m_chip_size;
    m_param.sector_size = mx25lxx_FLASH_SECTOR_SIZE;
    m_param.page_size   = mx25lxx_FLASH_PAGE_SIZE;

    if (quad_rw) {
        ret = mx25lxx_enable_quad_mode();
    }
    else {
        ret = mx25lxx_disable_quad_mode();
    }

    return ret;
}


int mx25lxx_read(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t v_remain;
    uint32_t len = 0;

    // 1. Check whether the addr is legal
    if (addr >= m_chip_size) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > m_chip_size ? (m_chip_size - addr) : length;

    while (length) {
        len = ((length >= FIFO_NOR_LEN) ? FIFO_NOR_LEN : length);
        ret = m_read_func(addr, data_buf, len);
        if (ret < 0) {
            return ret;
        }
        addr += len;
        data_buf += len;
        length -= len;
    }
    return FLASH_OK;
}


int mx25lxx_write(uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t sector_addr = 0;
    uint32_t sector_offset = 0;
    uint32_t sector_remain = 0;
    uint32_t write_len = 0;
    uint32_t index = 0;
    uint8_t *pread = NULL;
    uint8_t *pwrite = NULL;
    /*uint8_t swap_buf[w25qxx_FLASH_SECTOR_SIZE] = {0};*/
    uint8_t *swap_buf = aiva_malloc(mx25lxx_FLASH_SECTOR_SIZE);

    configASSERT(swap_buf != NULL);

    // 1. Check whether the addr is legal
    if (addr >= m_chip_size) {
        ret = -FLASH_WRONG_PARAM;
        goto out;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > m_chip_size ? (m_chip_size - addr) : length;

    while (length)
    {
        sector_addr   = addr & (~(mx25lxx_FLASH_SECTOR_SIZE - 1));
        sector_offset = addr & (mx25lxx_FLASH_SECTOR_SIZE - 1);
        sector_remain = mx25lxx_FLASH_SECTOR_SIZE - sector_offset;
        write_len     = ((length < sector_remain) ? length : sector_remain);
        ret = mx25lxx_read(sector_addr, swap_buf, mx25lxx_FLASH_SECTOR_SIZE);
        if (ret < 0) {
            goto out;
        }
        pread = swap_buf + sector_offset;
        pwrite = data_buf;
        for (index = 0; index < write_len; index++) {
            if ((*pwrite) != ((*pwrite) & (*pread))) {
                ret = mx25lxx_sector_erase(sector_addr);
                if (ret < 0) {
                    goto out;
                }
                break;
            }
            pwrite++;
            pread++;
        }
        if (write_len == mx25lxx_FLASH_SECTOR_SIZE) {
            ret = mx25lxx_sector_program(sector_addr, data_buf);
        }
        else {
            pread = swap_buf + sector_offset;
            pwrite = data_buf;
            for (index = 0; index < write_len; index++)
                *pread++ = *pwrite++;
            ret = mx25lxx_sector_program(sector_addr, swap_buf);
        }
        if (ret < 0) {
            goto out;
        }
        length   -= write_len;
        addr     += write_len;
        data_buf += write_len;
    }

out:
    aiva_free(swap_buf);
    return FLASH_OK;
}


int mx25lxx_get_param(nor_flash_param_t *pparam)
{
    *pparam = m_param;
    return 0;
}


int mx25lxx_lock(uint32_t addr, uint32_t length)
{
    uint32_t addr_bound;
    uint8_t feature_prot_cfg_value = 0;
    uint8_t i;

    addr_bound = addr + length;

    if(m_param.chip_size == mx25l64_FLASH_CHIP_SIZE)
    {
        for(i = 0; i < sizeof(feature_prot_cfg_lut_64m) / sizeof(feature_prot_cfg_lut_64m[0]); i++)
        {
            if(addr_bound < feature_prot_cfg_lut_64m[i].addr_protected_bound)
            {
                feature_prot_cfg_value = feature_prot_cfg_lut_64m[i].feature_prot_cfg_value;
                break;
            }
        }
        if(i == sizeof(feature_prot_cfg_lut_64m) / sizeof(feature_prot_cfg_lut_64m[0]))
        {
            feature_prot_cfg_value = feature_prot_cfg_lut_64m[i - 1].feature_prot_cfg_value;
        }
    }
    else if(m_param.chip_size == mx25l32_FLASH_CHIP_SIZE)
    {
        for(i = 0; i < sizeof(feature_prot_cfg_lut_32m) / sizeof(feature_prot_cfg_lut_32m[0]); i++)
        {
            if(addr_bound < feature_prot_cfg_lut_32m[i].addr_protected_bound)
            {
                feature_prot_cfg_value = feature_prot_cfg_lut_32m[i].feature_prot_cfg_value;
                break;
            }
        }
        if(i == sizeof(feature_prot_cfg_lut_32m) / sizeof(feature_prot_cfg_lut_32m[0]))
        {
            feature_prot_cfg_value = feature_prot_cfg_lut_32m[i - 1].feature_prot_cfg_value;
        }
    }
    else if(m_param.chip_size == mx25l16_FLASH_CHIP_SIZE)
    {
        for(i = 0; i < sizeof(feature_prot_cfg_lut_16m) / sizeof(feature_prot_cfg_lut_16m[0]); i++)
        {
            if(addr_bound < feature_prot_cfg_lut_16m[i].addr_protected_bound)
            {
                feature_prot_cfg_value = feature_prot_cfg_lut_16m[i].feature_prot_cfg_value;
                break;
            }
        }
        if(i == sizeof(feature_prot_cfg_lut_16m) / sizeof(feature_prot_cfg_lut_16m[0]))
        {
            feature_prot_cfg_value = feature_prot_cfg_lut_16m[i - 1].feature_prot_cfg_value;
        }
    }
    mx25lxx_write_prot_cfg(feature_prot_cfg_value);

    return FLASH_OK;
}


int mx25lxx_unlock(uint32_t addr, uint32_t length)
{
    mx25lxx_write_prot_cfg(0);
    return FLASH_OK;
}


#define MXIC_NOR_OPERATION \
        .init         = mx25lxx_init,\
        .read         = mx25lxx_read,\
        .write        = mx25lxx_write,\
        .page_prog    = mx25lxx_page_program,\
        .sector_erase = mx25lxx_sector_erase,\
        .get_param    = mx25lxx_get_param,\
        .lock         = mx25lxx_lock,\
        .unlock       = mx25lxx_unlock,\
        .read_unique_id = mx25lxx_read_unique_id

static nor_flash_drv_t mx25lxx_nor_drivers[] = {
    // MXIC NOR
    {
        .manuf_id     = 0xc2,
        .device_id    = 0x2015,
        MXIC_NOR_OPERATION,
    },
    {
        .manuf_id     = 0xc2,
        .device_id    = 0x2017,
        MXIC_NOR_OPERATION,
    },
    {
        .manuf_id     = 0xc2,
        .device_id    = 0x2537,
        MXIC_NOR_OPERATION,
    },
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     mx25lxx_c2_2015,    0,      &mx25lxx_nor_drivers[0],    NULL);
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     mx25lxx_c2_2017,    0,      &mx25lxx_nor_drivers[1],    NULL);
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     mx25lxx_c2_2537,    0,      &mx25lxx_nor_drivers[2],    NULL);
