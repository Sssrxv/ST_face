#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "shell.h"
#include "syslog.h"
#include "io.h"
#include "spi.h"
#include "boot_flash.h"
#include "aiva_malloc.h"
#include "eeprom.h"
#include "libvde.h"

#define FMT_BYTE    (0)
#define FMT_WORD    (1)
#define FMT_DWORD   (2)

static void w4_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: w4 addr data\n");
}

static int _w4(uint32_t addr, uint32_t data)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "w4 addr: 0x%08x data: 0x%08x\n", addr, data);
    writel(data, (uintptr_t)addr);
    return 0;
}

void w4(int argc, char *argv[])
{
    uint32_t addr, data;
    if (argc != 3)  {
        w4_help();
        return;
    }
    addr = strtoul(argv[1], NULL, 0);
    data = strtoul(argv[2], NULL, 0);
    _w4(addr, data);
}

static void r4_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: r4 [addr]");
}

static uint32_t _r4(uint32_t addr)
{
    uint32_t data;
    Shell *shell = shellGetCurrent();
    data = readl((uintptr_t)addr);
    shellPrint(shell, "r4 addr: 0x%x data: 0x%x\n", addr, data);
    return data;
}


void r4(int argc, char *argv[])
{
    uint32_t addr;
    if (argc != 2)  {
        r4_help();
        return;
    }
    addr = strtoul(argv[1], NULL, 0);
    _r4(addr);
}

static void memdump_b(uintptr_t addr, int cnt)
{
    int i;
    int j;
    char line[16];
    Shell *shell = shellGetCurrent();
    
    for (i = 0; i < cnt; i++) {
        line[i%16] = readb(addr);
        shellPrint(shell, "%02x, ", readb(addr));
        addr += 1;
        if ( (i+1) % 16 == 0 ) {
            for (j = 0; j < 16; j++) {
                if (line[j] >= 0x20 && line[j] <= 0x7E) {
                    shellPrint(shell, "%c", line[j]);
                }
                else {
                    shellPrint(shell, ".");
                }
            }
            shellPrint(shell, "\n");
        }
    }
    shellPrint(shell, "\n");
}

static void memdump_w(uintptr_t addr, int cnt)
{
    int i;
    Shell *shell = shellGetCurrent();

    for (i = 0; i < cnt; i++) {
        shellPrint(shell, "%04x, ", readw(addr));
        addr += 2;
        if ( (i+1) % 8 == 0 ) {
            shellPrint(shell, "\n");
        }
    }
    shellPrint(shell, "\n");
}

static void memdump_d(uintptr_t addr, int cnt)
{
    int i;
    Shell *shell = shellGetCurrent();
    for (i = 0; i < cnt; i++) {
        shellPrint(shell, "%08x, ", readl(addr));
        addr += 4;
        if ( (i+1) % 4 == 0 ) {
            shellPrint(shell, "\n");
        }
    }
    shellPrint(shell, "\n");
}


static void memdump(int fmt, uint32_t addr, int cnt)
{
    switch (fmt) {
        case FMT_BYTE:
            memdump_b(addr, cnt);
            break;
        case FMT_WORD:
            memdump_w(addr, cnt);
            break;
        case FMT_DWORD:
            memdump_d(addr, cnt);
            break;
        default:
            return ;
    }
}

static int flash_dump(int fmt, uint32_t addr, int cnt)
{
    uint32_t baudr = 8;
    flash_type_t flash_type;
    int bytes;
    uint8_t *buf = NULL;

	boot_flash_init(SPI_DEVICE_0, SPI_CHIP_SELECT_0, baudr);
    flash_type = boot_get_flash_type(SPI_DEVICE_0, baudr);

    switch (fmt) {
        case FMT_BYTE:
            bytes = cnt;
            buf = aiva_malloc(bytes);
            boot_flash_read(flash_type, addr, buf, bytes, SPI_DMA_EN);
            memdump_b((uint32_t)buf, cnt);
            break;
        case FMT_WORD:
            bytes = cnt*2;
            buf = aiva_malloc(bytes);
            boot_flash_read(flash_type, addr, buf, bytes, SPI_DMA_EN);
            memdump_w((uint32_t)buf, cnt);
            break;
        case FMT_DWORD:
            bytes = cnt*4;
            buf = aiva_malloc(bytes);
            boot_flash_read(flash_type, addr, buf, bytes, SPI_DMA_EN);
            memdump_d((uint32_t)buf, cnt);
            break;
        default:
            return -1;
    }

    if (buf != NULL) {
        aiva_free(buf);
    }
    return 0;
}
    

static int read_module_id(i2c_device_number_t i2c_num)
{
    int ret;

    eeprom_ctrl eeprom_entity = {
        .name        = "FM24C02",
        .dev_addr    = 0x50,
        .page_num    = 32,
        .page_size   = 8,
        .wr_delay    = 5,
        .i2c.dev_num = i2c_num,
        .i2c.bus_clk = 100000,
    };

    eeprom_vde_param_t eeprom_read_data = {0};

    ret = read_eeprom_vde_param(&eeprom_entity, &eeprom_read_data);

    if (ret == 0) {
        LOGI(__func__, "module id is %lu", eeprom_read_data.header.id);
    }
    else {
        LOGI(__func__, "Invalid module ID!");
    }

    return 0;
}


static void cmds_dump_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell,   "Usage: dump [OPTIONS] [ARGS] \r\n"
                        "   [OPTIONS]: \r\n"
                        "      - memb : dump memory region in bytes,  ARGS: [offset], [cnt] \r\n"
                        "      - memw : dump memory region in words,  ARGS: [offset], [cnt] \r\n"
                        "      - memd : dump memory region in dwords, ARGS: [offset], [cnt] \r\n"
                        "      - flsb : dump flash region in bytes,   ARGS: [offset], [cnt] \r\n"
                        "      - flsw : dump flash region in words,   ARGS: [offset], [cnt] \r\n"
                        "      - flsd : dump flash region in dwords,  ARGS: [offset], [cnt] \r\n"
                        "      - id   : dump module id in eeprom, ARGS: [i2c num] \r\n"
               );
}


int cmds_dump(int argc, char **argv)
{
    uint32_t addr;
    int cnt;
    int fmt;
    Shell *shell = shellGetCurrent();

    
    if (strncmp("mem", argv[1], 3) == 0) {
        if (argc != 4) {
            cmds_dump_help();
            return -1;
        }
        addr = strtoul(argv[2], NULL, 0);
        cnt  = strtol (argv[3], NULL, 0);

        if (strncmp("memb", argv[1], 4) == 0) {
            fmt = FMT_BYTE;
        }
        else if (strncmp("memw", argv[1], 4) == 0) {
            fmt = FMT_WORD;
        }
        else if (strncmp("memd", argv[1], 4) == 0) {
            fmt = FMT_DWORD;
        }
        else {
            shellPrint(shell, "Unknown option: %s\n", argv[1]);
            cmds_dump_help();
            return -1;
        }
        memdump(fmt, addr, cnt);
    }
    else if (strncmp("fls", argv[1], 3) == 0) {
        if (argc != 4) {
            cmds_dump_help();
            return -1;
        }
        addr = strtoul(argv[2], NULL, 0);
        cnt  = strtol (argv[3], NULL, 0);
        if (strncmp("flsb", argv[1], 4) == 0) {
            fmt = FMT_BYTE;
        }
        else if (strncmp("flsw", argv[1], 4) == 0) {
            fmt = FMT_WORD;
        }
        else if (strncmp("flsd", argv[1], 4) == 0) {
            fmt = FMT_DWORD;
        }
        else {
            shellPrint(shell, "Unknown option: %s\n", argv[1]);
            cmds_dump_help();
            return -1;
        }
        flash_dump(fmt, addr, cnt);
    }
    else if (strncmp("id", argv[1], 9) == 0) {
        if (argc == 3) {
            i2c_device_number_t i2c_num = strtol(argv[2], NULL, 0);
            /*LOGD(__func__, "i2c_num is %d", i2c_num);*/
            if (i2c_num < I2C_DEVICE_MAX) {
                read_module_id(i2c_num);
            }
        }
        else {
            cmds_dump_help();
            return -1;
        }

    }
    else {
        shellPrint(shell, "Unknown option: %s\n", argv[1]);
        cmds_dump_help();
        return -1;
    }

    return 0;
}


/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
r4, r4, read 4 bytes);

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
w4, w4, write 4 bytes);

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
dump, cmds_dump, dump memory/flash);
