#include "spi.h"
#include "sysctl.h"
#include "dmac.h"
#include "flash.h"
#include "aiva_sleep.h"
#include "syslog.h"
#include "aiva_malloc.h"
#include "udevice.h"
#include "flash_dev.h"
#include "nor_flash.h"
#include <string.h>

/* clang-format off */

#define SPI_SLAVE_SELECT                    (0x01)

#define FLASH_PAGE_SIZE              256
#define FLASH_SECTOR_SIZE            4096
#define FLASH_PAGE_NUM_PER_SECTOR    16

#define WRITE_ENABLE                        0x06
#define WRITE_DISABLE                       0x04
#define READ_REG1                           0x05
#define READ_REG2                           0x35
#define READ_REG3                           0x15
#define WRITE_REG1                          0x01
#define WRITE_REG2                          0x31
#define WRITE_REG3                          0x11
#define READ_DATA                           0x03
#define FAST_READ                           0x0B
#define FAST_READ_DUAL_OUTPUT               0x3B
#define FAST_READ_QUAL_OUTPUT               0x6B
#define FAST_READ_DUAL_IO                   0xBB
#define FAST_READ_QUAL_IO                   0xEB
#define DUAL_READ_RESET                     0xFFFF
#define QUAL_READ_RESET                     0xFF
#define PAGE_PROGRAM                        0x02
#define QUAD_PAGE_PROGRAM                   0x32
#define SECTOR_ERASE                        0x20
#define BLOCK_32K_ERASE                     0x52
#define BLOCK_64K_ERASE                     0xD8
#define CHIP_ERASE                          0x60
#define READ_ID                             0x9F
#define READ_UNIQUE_ID                      0x4B
#define ENABLE_QPI                          0x38
#define EXIT_QPI                            0xFF
#define ENABLE_RESET                        0x66
#define RESET_DEVICE                        0x99

#define REG1_BUSY_MASK                      0x01
#define REG2_QUAL_MASK                      0x02

#define FLASH_CHIP_SIZE_16                 (2*1024*1024)
#define FLASH_CHIP_SIZE_32                 (4*1024*1024)
#define FLASH_CHIP_SIZE_64                 (8*1024*1024)
/* clang-format on */
typedef struct feature_prot_cfg {
    uint32_t addr_protected_bound;
    uint8_t  feature_prot_cfg_value;
} feature_prot_cfg_t;

typedef int (*page_program_func_t)(flash_dev_t *dev, uint32_t addr, uint8_t * data_buf, uint32_t length);
typedef int (*read_func_t)(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);

typedef struct nor_flash_std_priv_params
{
    uint8_t quad_read_wait_cycles;
} nor_flash_std_priv_params_t;

static feature_prot_cfg_t feature_prot_cfg_lut_16m[]={
    {FLASH_CHIP_SIZE_16 / 32, (0b01001) << 2},
    {FLASH_CHIP_SIZE_16 / 16, (0b01010) << 2},
    {FLASH_CHIP_SIZE_16 /  8, (0b01011) << 2},
    {FLASH_CHIP_SIZE_16 /  4, (0b10100) << 2},
    {FLASH_CHIP_SIZE_16 /  2, (0b10101) << 2},
    {FLASH_CHIP_SIZE_16 /  1, (0b11110) << 2},
};

static feature_prot_cfg_t feature_prot_cfg_lut_32m[]={
    {FLASH_CHIP_SIZE_32 / 64, (0b01001) << 2},
    {FLASH_CHIP_SIZE_32 / 32, (0b01010) << 2},
    {FLASH_CHIP_SIZE_32 / 16, (0b01011) << 2},
    {FLASH_CHIP_SIZE_32 /  8, (0b01100) << 2},
    {FLASH_CHIP_SIZE_32 /  4, (0b10101) << 2},
    {FLASH_CHIP_SIZE_32 /  2, (0b10110) << 2},
    {FLASH_CHIP_SIZE_32 /  1, (0b11111) << 2},
};

static feature_prot_cfg_t feature_prot_cfg_lut_64m[]={
    {FLASH_CHIP_SIZE_64 / 64, (0b01001) << 2},
    {FLASH_CHIP_SIZE_64 / 32, (0b01010) << 2},
    {FLASH_CHIP_SIZE_64 / 16, (0b01011) << 2},
    {FLASH_CHIP_SIZE_64 /  8, (0b01100) << 2},
    {FLASH_CHIP_SIZE_64 /  4, (0b10101) << 2},
    {FLASH_CHIP_SIZE_64 /  2, (0b10110) << 2},
    {FLASH_CHIP_SIZE_64 /  1, (0b11111) << 2},
};

static int nor_flash_quad_page_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
static int nor_flash_page_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
static int nor_flash_quad_read_data(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);
static int nor_flash_read_data(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length);

static int nor_flash_std_wait_for_ready(flash_dev_t *dev);


static int nor_flash_std_receive_data(flash_dev_t *dev, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    int ret;
    spi_device_num_t spi_num = dev->cfg.spi_num;
    spi_chip_select_t spi_cs = dev->cfg.spi_cs;

    spi_init(spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_standard(spi_num, spi_cs, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    ret = spi_receive_data_standard_dma(spi_num, spi_cs, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int nor_flash_std_send_data(flash_dev_t *dev, uint8_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    spi_device_num_t spi_num = dev->cfg.spi_num;
    spi_chip_select_t spi_cs = dev->cfg.spi_cs;

    spi_init(spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_standard(spi_num, spi_cs, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_standard_dma(spi_num, spi_cs, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int nor_flash_std_receive_data_enhanced(flash_dev_t *dev, uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *rx_buff, uint32_t rx_len)
{
    int ret;
    spi_device_num_t spi_num = dev->cfg.spi_num;
    spi_chip_select_t spi_cs = dev->cfg.spi_cs;

    spi_init(spi_num, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);

    uint32_t wait_cycles = 4;
    if (dev->priv) {
        wait_cycles = ((nor_flash_std_priv_params_t*)dev->priv)->quad_read_wait_cycles;
    }
    spi_init_non_standard(spi_num, 8/*instrction length*/, 32/*address length*/, wait_cycles/*wait cycles*/,
                            SPI_AITM_ADDR_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_receive_data_multiple(spi_num, spi_cs, cmd_buff, cmd_len, rx_buff, rx_len);
#else
    if (((uint32_t)rx_buff & 31UL) == 0 && rx_len >= cmd_len * sizeof(*cmd_buff) && (rx_len & 31UL) == 0)
        ret = spi_receive_data_multiple_dma_aligned32(spi_num, spi_cs, cmd_buff, cmd_len, rx_buff, rx_len);
    else
        ret = spi_receive_data_multiple_dma(spi_num, spi_cs, cmd_buff, cmd_len, rx_buff, rx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }
    return FLASH_OK;
}


static int nor_flash_std_send_data_enhanced(flash_dev_t *dev, uint32_t *cmd_buff, uint8_t cmd_len, uint8_t *tx_buff, uint32_t tx_len)
{
    int ret;
    spi_device_num_t spi_num = dev->cfg.spi_num;
    spi_chip_select_t spi_cs = dev->cfg.spi_cs;

    spi_init(spi_num, SPI_WORK_MODE_0, SPI_FF_QUAD, FIFO_W32);
    spi_init_non_standard(spi_num, 8/*instrction length*/, 24/*address length*/, 0/*wait cycles*/,
                            SPI_AITM_STANDARD/*spi address trans mode*/);
#ifndef FLASH_USE_DMA
    ret = spi_send_data_multiple(spi_num, spi_cs, cmd_buff, cmd_len, tx_buff, tx_len);
#else
    ret = spi_send_data_multiple_dma(spi_num, spi_cs, cmd_buff, cmd_len, tx_buff, tx_len);
#endif
    if (ret < 0) {
        return -FLASH_ERROR_SPI_TRANS;
    }

    return FLASH_OK;
}


static int nor_flash_std_write_enable(flash_dev_t *dev)
{
    int ret;
    uint8_t cmd[1] = {WRITE_ENABLE};

    ret = nor_flash_std_send_data(dev, cmd, 1, 0, 0);
    return ret;
}


static int nor_flash_std_write_status_reg1(flash_dev_t *dev, uint8_t reg1_data)
{
    int ret;
    uint8_t cmd[2] = {WRITE_REG1, reg1_data};

    ret = nor_flash_std_write_enable(dev);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_send_data(dev, cmd, 2, 0, 0);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_wait_for_ready(dev);
    return ret;
}


static int nor_flash_std_write_status_reg2(flash_dev_t *dev, uint8_t reg2_data)
{
    int ret;
    uint8_t cmd[2] = {WRITE_REG2, reg2_data};

    ret = nor_flash_std_write_enable(dev);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_send_data(dev, cmd, 2, 0, 0);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_wait_for_ready(dev);
    return ret;
}


static int nor_flash_std_read_status_reg1(flash_dev_t *dev, uint8_t *reg_data)
{
    int ret;
    uint8_t cmd[1] = {READ_REG1};
    uint8_t data[1] = {0};

    ret = nor_flash_std_receive_data(dev, cmd, 1, data, 1);
    if (ret < 0) {
        return ret;
    }
    *reg_data = data[0];
    return FLASH_OK;
}


static int nor_flash_std_read_status_reg2(flash_dev_t *dev, uint8_t *reg_data)
{
    int ret;
    uint8_t cmd[1] = {READ_REG2};
    uint8_t data[1] = {0};

    ret = nor_flash_std_receive_data(dev, cmd, 1, data, 1);
    if (ret < 0) {
        return ret;
    }
    *reg_data = data[0];
    return FLASH_OK;
}


static int nor_flash_std_enable_quad_mode(flash_dev_t *dev)
{
    int ret;
    uint8_t reg2_data = 0;

    ret = nor_flash_std_read_status_reg2(dev, &reg2_data);
    if (ret < 0) {
        return ret;
    }
    if (!(reg2_data & REG2_QUAL_MASK)) {
        reg2_data |= REG2_QUAL_MASK;
        nor_flash_std_write_status_reg2(dev, reg2_data);
    }

    // Read status register 2 and check the QE bit is enabled
    ret = nor_flash_std_read_status_reg2(dev, &reg2_data);
    if (ret < 0) {
        return ret;
    }
    if (!(reg2_data & REG2_QUAL_MASK)) {
        return -FLASH_ERROR_STATUS;
    }

    return FLASH_OK;
}


static int nor_flash_std_write_prot_cfg(flash_dev_t *dev, uint8_t prot_value)
{
    int ret;
    uint8_t reg_data1 = 0;

    ret = nor_flash_std_read_status_reg1(dev, &reg_data1);
    if (ret < 0) {
        return ret;
    }

    reg_data1 &= 0x83;
    reg_data1 |= prot_value;
    nor_flash_std_write_status_reg1(dev, reg_data1);

    return FLASH_OK;
}


static int nor_flash_std_disable_quad_mode(flash_dev_t *dev)
{
    int ret;
    uint8_t reg2_data = 0;
    ret = nor_flash_std_read_status_reg2(dev, &reg2_data);
    if (ret < 0) {
        return ret;
    }
    if (reg2_data & REG2_QUAL_MASK) {
        reg2_data &= (~REG2_QUAL_MASK);
        ret = nor_flash_std_write_status_reg2(dev, reg2_data);
    }

    // Read status register 2 and check the QE bit disabled
    ret = nor_flash_std_read_status_reg2(dev, &reg2_data);
    if (ret < 0) {
        return ret;
    }
    if (reg2_data & REG2_QUAL_MASK) {
        return -FLASH_ERROR_STATUS;
    }

    return ret;
}


static int nor_flash_std_is_busy(flash_dev_t *dev)
{
    uint8_t status = 0;

    nor_flash_std_read_status_reg1(dev, &status);
    if (status & REG1_BUSY_MASK) {
        return 1;
    }
    return 0;
}


static int nor_flash_std_wait_for_ready(flash_dev_t *dev)
{
    uint32_t timeout;

    timeout = 20 * 1000 * 100; // 20 sec

    while(timeout--) {
        if (!nor_flash_std_is_busy(dev)) {
            return FLASH_OK;
        }
        aiva_usleep(10);
    }
    return -FLASH_ERROR_TIMEOUT;
}


int nor_flash_std_sector_erase(flash_dev_t *dev, uint32_t addr, int force_erase)
{
    (void)force_erase;

    uint8_t cmd[4] = {SECTOR_ERASE};
    int ret;

    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr);
    ret = nor_flash_std_write_enable(dev);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_send_data(dev, cmd, 4, 0, 0);
    if (ret < 0) {
        return ret;
    }
    if (nor_flash_std_wait_for_ready(dev) < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }
    return FLASH_OK;
}


static int nor_flash_quad_page_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint32_t cmd[2] = {0};
    int ret;

    cmd[0] = QUAD_PAGE_PROGRAM;
    cmd[1] = addr;
    ret = nor_flash_std_write_enable(dev);
    if (ret < 0) {
        return ret;
    }

    ret = nor_flash_std_send_data_enhanced(dev, cmd, 2, data_buf, length);
    if (ret < 0) {
        return ret;
    }
    if (nor_flash_std_wait_for_ready(dev) < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }
    return FLASH_OK;
}


static int nor_flash_page_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint8_t cmd[4];
    int ret;

    cmd[0] = PAGE_PROGRAM;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr);

    ret = nor_flash_std_write_enable(dev);
    if (ret < 0) {
        return ret;
    }
    ret = nor_flash_std_send_data(dev, cmd, 4, data_buf, length);
    if (ret < 0) {
        return ret;
    }

    if (nor_flash_std_wait_for_ready(dev) < 0) {
        return -FLASH_ERROR_TIMEOUT;
    }
    return FLASH_OK;
}


int nor_flash_std_page_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    (void)(length);

    int ret;
    page_program_func_t program_func = dev->cfg.enable_quad_rw ? nor_flash_quad_page_program : nor_flash_page_program;
    ret = program_func(dev, addr, data_buf, FLASH_PAGE_SIZE);
    return ret;
}

static int nor_flash_std_sector_program(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf)
{
    int ret = 0;

    for (int index = 0; index < FLASH_PAGE_NUM_PER_SECTOR; index++)
    {
        ret = nor_flash_std_page_program(dev, addr, data_buf, FLASH_PAGE_SIZE);
        if (ret < 0) {
            return ret;
        }
        addr += FLASH_PAGE_SIZE;
        data_buf += FLASH_PAGE_SIZE;
    }
    return FLASH_OK;
}


static int nor_flash_quad_read_data(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint32_t cmd[2] = {0};
    int ret;

    cmd[0] = FAST_READ_QUAL_IO;
    cmd[1] = addr << 8;

    ret = nor_flash_std_receive_data_enhanced(dev, cmd, 2, data_buf, length);

    return ret;
}


static int nor_flash_read_data(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    uint8_t cmd[4];
    int ret;

    cmd[0] = READ_DATA;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr);

    ret = nor_flash_std_receive_data(dev, cmd, 4, data_buf, length);

    return ret;
}


int nor_flash_std_read_id(flash_dev_t *dev, uint8_t *manuf_id, uint16_t *device_id)
{
    uint8_t cmd[1] = {READ_ID};
    uint8_t data[3] = {0};
    int ret;

    ret = nor_flash_std_receive_data(dev, cmd, 1, data, 3);
    if (ret < 0) {
        return ret;
    }
    // if (data[0] != 0x85) {
    //     return -FLASH_UNSPPORT;
    // }
    *manuf_id = data[0];
    *device_id = ((uint16_t)(data[1]) << 8) | (uint16_t)(data[2]);
    return FLASH_OK;
}


int nor_flash_std_read_unique_id(flash_dev_t *dev, flash_unique_id_t *unique_id)
{
    uint8_t cmd[1] = {READ_UNIQUE_ID};
    uint8_t data[20] = {0};
    int ret;

    ret = nor_flash_std_receive_data(dev, cmd, 1, data, 20);
    if (ret < 0) {
        return ret;
    }

    unique_id->id_length = 16;
    memcpy(unique_id->uid, &data[4], 16);

    return FLASH_OK;
}


int nor_flash_std_read(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t len = 0;

    // 1. Check whether the addr is legal
    uint32_t chip_size = dev->chip_size;
    if (addr >= chip_size) {
        return -FLASH_WRONG_PARAM;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > chip_size ? (chip_size - addr) : length;

    read_func_t read_func = dev->cfg.enable_quad_rw ? nor_flash_quad_read_data : nor_flash_read_data;
    while (length) {
        len = ((length >= FIFO_NOR_LEN) ? FIFO_NOR_LEN : length);
        ret = read_func(dev, addr, data_buf, len);
        if (ret < 0) {
            return ret;
        }
        addr += len;
        data_buf += len;
        length -= len;
    }

    return FLASH_OK;
}


int nor_flash_std_write(flash_dev_t *dev, uint32_t addr, uint8_t *data_buf, uint32_t length)
{
    int ret;
    uint32_t sector_addr = 0;
    uint32_t sector_offset = 0;
    uint32_t sector_remain = 0;
    uint32_t write_len = 0;
    uint32_t index = 0;
    uint8_t *pread = NULL;
    uint8_t *pwrite = NULL;
    /*uint8_t swap_buf[FLASH_SECTOR_SIZE] = {0};*/
    uint8_t *swap_buf = aiva_malloc(FLASH_SECTOR_SIZE);

    configASSERT(swap_buf != NULL);

    // 1. Check whether the addr is legal
    uint32_t chip_size = dev->chip_size;
    if (addr >= chip_size) {
        ret = -FLASH_WRONG_PARAM;
        goto out;
    }
    // 2. Make sure the length is no longer than the max flash size
    length = (addr + length) > chip_size ? (chip_size - addr) : length;

    while (length)
    {
        sector_addr   = addr & (~(FLASH_SECTOR_SIZE - 1));
        sector_offset = addr & (FLASH_SECTOR_SIZE - 1);
        sector_remain = FLASH_SECTOR_SIZE - sector_offset;
        write_len     = ((length < sector_remain) ? length : sector_remain);
        ret = nor_flash_std_read(dev, sector_addr, swap_buf, FLASH_SECTOR_SIZE);
        if (ret < 0) {
            goto out;
        }
        pread = swap_buf + sector_offset;
        pwrite = data_buf;
        for (index = 0; index < write_len; index++) {
            if ((*pwrite) != ((*pwrite) & (*pread))) {
                ret = nor_flash_std_sector_erase(dev, sector_addr, 0);
                if (ret < 0) {
                    goto out;
                }
                break;
            }
            pwrite++;
            pread++;
        }
        if (write_len == FLASH_SECTOR_SIZE) {
            ret = nor_flash_std_sector_program(dev, sector_addr, data_buf);
        }
        else {
            pread = swap_buf + sector_offset;
            pwrite = data_buf;
            for (index = 0; index < write_len; index++)
                *pread++ = *pwrite++;
            ret = nor_flash_std_sector_program(dev, sector_addr, swap_buf);
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


int nor_flash_std_init(flash_dev_t *dev, const flash_dev_config_t *cfg)
{
    int ret;
    uint8_t manuf_id;
    uint16_t device_id;

    dev->cfg = *cfg;

    spi_device_num_t spi_num = cfg->spi_num;
    spi_init(spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, FIFO_W8);
    spi_set_clk_rate(spi_num, cfg->clk_rate);

    ret = nor_flash_std_read_id(dev, &manuf_id, &device_id);
    if (ret < 0) {
        return ret;
    }

    if (dev->manuf_id == 0) {
        dev->manuf_id = manuf_id;
    }

    if (dev->device_id == 0) {
        dev->device_id = device_id;
    }

    if (dev->chip_size == 0) {
        flash_sfdp_t sfdp = {0};
        ret = nor_flash_read_sfdp(dev, &sfdp);
        if (ret == 0) {
            dev->chip_size = sfdp.memory_bytes_density;
        } else {
            dev->chip_size = 2*1024*1024;
            LOGE("", "sfdp read failed, force set chip_size:%d", (int)dev->chip_size);
        }
    }

    if (cfg->enable_quad_rw) {
        ret = nor_flash_std_enable_quad_mode(dev);
    }
    else {
        ret = nor_flash_std_disable_quad_mode(dev);
    }


    return ret;
}


int nor_flash_std_get_param(flash_dev_t *dev, flash_param_t *pparam)
{
    *pparam = FLASH_PARAM_INITIALIZER;

    pparam->dev_name = dev->name;
    pparam->mid = dev->manuf_id;
    pparam->did = dev->device_id;
    pparam->chip_size   = dev->chip_size;
    pparam->page_size   = FLASH_PAGE_SIZE;
    pparam->sector_size = FLASH_SECTOR_SIZE;

    return 0;
}


int nor_flash_std_lock(flash_dev_t *dev, uint32_t addr, uint32_t length)
{
    uint32_t addr_bound;
    uint8_t feature_prot_cfg_value = 0;
    uint8_t i;

    addr_bound = addr + length;

    uint32_t chip_size = dev->chip_size;
    if(chip_size == FLASH_CHIP_SIZE_64)
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
    else if(chip_size == FLASH_CHIP_SIZE_32)
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
    else if(chip_size == FLASH_CHIP_SIZE_16)
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
    else {
        feature_prot_cfg_value = 0;
    }
    nor_flash_std_write_prot_cfg(dev, feature_prot_cfg_value);
    return FLASH_OK;
}

int nor_flash_std_unlock(flash_dev_t *dev, uint32_t addr, uint32_t length)
{
    (void)(addr);
    (void)(length);

    nor_flash_std_write_prot_cfg(dev, 0);
    return FLASH_OK;
}

static nor_flash_std_priv_params_t m_priv_params = {
    .quad_read_wait_cycles = 4,
};

static flash_dev_ops_t m_flash_ops = {
    .init = nor_flash_std_init,
    .read = nor_flash_std_read,
    .write = nor_flash_std_write,
    .erase = nor_flash_std_sector_erase,
    .program = nor_flash_std_page_program,
    .release = NULL,
    .read_unique_id = nor_flash_std_read_unique_id,
    .get_param = nor_flash_std_get_param,
    .lock = nor_flash_std_lock,
    .unlock = nor_flash_std_unlock,
};

// NOTE: mid did is set to 0 to mark update when init
// NOTE: "nor_std" is used by flash_dev to search for default nor flash driver
// NOTE: chip_size is set to 0 to mark use sfdp auto detect memesize
static flash_dev_t nor_std_devices[] = {
    [0] = FLASH_DEV_INITIALIZER_WITH_PARAMS(FLASH_NOR, 0x00, 0x0000, "nor_std", 0, &m_flash_ops, 0, &m_priv_params),
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_SPI_FLASH,     nor_std,     0,      &nor_std_devices[0],    NULL);
