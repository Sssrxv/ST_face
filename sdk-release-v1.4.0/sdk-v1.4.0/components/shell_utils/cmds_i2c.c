#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "FreeRTOS_POSIX_portable.h"

#include "i2c.h"

static i2c_device_number_t m_i2c_num = I2C_DEVICE_MAX;
static uint32_t m_slave_address;
static uint32_t m_addr_width;

static void i2c_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: i2c [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - init    : init i2c, ARGS: [i2c num], [slave addr(7 bit)], [addr width(Byte)] \r\n"
                "                : [i2c num],          0 ~ 2 \r\n"
                "                : [slave addr(7 bit)],  0 ~ 127 \r\n"
                "                : [addr width(Byte)],    1, 2 \r\n"
                "      - write   : write,  ARGS: [reg addr], [value] \r\n"
                "                : [reg addr],         int, register addr \r\n"
		        "                : [value],            int, data value \r\n"
                "      - read    : read,   ARGS: [reg addr]\r\n"
                "                : [reg addr],         int, register addr \r\n"
                "      - scan    : scan,   ARGS: [i2c num]\r\n"
                "                : [i2c num],          0 ~ 2 \r\n"
            );      
}

static int i2c_write_reg_1(int i2c_num, uint8_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[2];
    data_buf[0] = reg_addr;
    data_buf[1] = reg_val;
    ret = i2c_send_data(i2c_num, m_slave_address, data_buf, 2);
    return ret;
}

static int i2c_read_reg_1(int i2c_num, uint8_t reg_addr, uint8_t *reg_val)
{
    int ret;
    
    ret = i2c_send_data(i2c_num, m_slave_address, &reg_addr, 1);
    if (ret < 0) {
        return ret;
    }
    ret = i2c_recv_data(i2c_num, m_slave_address, 0, 0, reg_val, 1);
    
    return ret;
}

static int i2c_write_reg_2(int i2c_num, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];

    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, m_slave_address, data_buf, 3);

    return ret;
}

static int i2c_read_reg_2(int i2c_num, uint16_t reg_addr, uint8_t *reg_val)
{
    int ret;

    uint8_t addr_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xff;
    addr_buf[1] = (reg_addr >> 0) & 0xff;
    
    ret = i2c_send_data(i2c_num, m_slave_address, &addr_buf[0], 2);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_recv_data(i2c_num, m_slave_address, 0, 0, reg_val, 1);
    
    return ret;
}

void i2c_write(uint32_t reg_addr, uint32_t value)
{
    i2c_init(m_i2c_num, m_slave_address, 7, 350*1000);

    if (m_addr_width == 1)
    {
        i2c_write_reg_1(m_i2c_num, reg_addr&0xff, value&0xff);
    } 
    else if (m_addr_width == 2) {
        i2c_write_reg_2(m_i2c_num, reg_addr&0xffff, value&0xff);
    }

    return;
}

void i2c_read(uint32_t reg_addr)
{
    uint8_t reg_val;
    Shell *shell = shellGetCurrent();

    i2c_init(m_i2c_num, m_slave_address, 7, 350*1000);

    if (m_addr_width == 1)
    {
        i2c_read_reg_1(m_i2c_num, reg_addr&0xff, &reg_val);
    } 
    else if (m_addr_width == 2) {
        i2c_read_reg_2(m_i2c_num, reg_addr&0xffff, &reg_val);
    }

    shellPrint(shell, "reg addr 0x%x, value 0x%x \r\n", reg_addr, reg_val);

    return;
}

void i2c_driver(int argc, char *argv[]) 
{
    if (argc < 3) {
        i2c_help();
        return ;
    }
 
    if (strncmp(argv[1], "init", 4) == 0) {
        if (argc != 5) {
            i2c_help();
            return ;
        }
        m_i2c_num = strtol(argv[2], NULL, 0);
        m_slave_address  = strtol(argv[3], NULL, 0);
        m_addr_width = strtol(argv[4], NULL, 0);
        i2c_init(m_i2c_num, m_slave_address, 7, 350*1000);
    }
    else if (strncmp(argv[1], "write", 5) == 0) {
        if (argc != 4) {
            i2c_help();
            return ;
        }

        uint32_t reg_addr = strtol(argv[2], NULL, 0);
        uint32_t value = strtol(argv[3], NULL, 0);
        i2c_write(reg_addr, value);
    }
    else if (strncmp(argv[1], "read", 4) == 0) {
        if (argc != 3) {
            i2c_help();
            return ;
        }

        uint32_t reg_addr = strtol(argv[2], NULL, 0);
        i2c_read(reg_addr);
    }
    else if (strncmp(argv[1], "scan", 4) == 0) {
        i2c_device_number_t i2c_num = strtol(argv[2], NULL, 0);
        if (i2c_num < I2C_DEVICE_MAX) {
            i2c_scan_dev_addr(i2c_num, 7, 10*1000);
        }
    }
    else {
        i2c_help();
    }
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
i2c, i2c_driver, i2c read/write);
