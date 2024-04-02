#include "cmds_load.h"
#include "xyzModem.h"
#include <stdlib.h>
#include <string.h>
#include "uart_puts.h"
#include "boot_flash.h"
#include "spi.h"
#include "flash.h"
#include "syslog.h"
#include "sysctl.h"
#include "flash_stream.h"
#include "flash_part.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "fs_file.h"
#include "aiva_malloc.h"
#include "eeprom.h"
#include "gunzip.h"
#include "AI3100.h"
#include "libota.h"
#include "os_log.h"
#include "fs_file.h"
#include "aiva_scu_wdt.h"
#include "os_init_app.h"
#include "libvde.h"
#include "crc16.h"
#include "crc32.h"
#include "shell_port.h"
#include "os_shell_app.h"

#define OTA_BAUD_RATE (460800)
#define MAX_LOAD_SIZE (18 * 1024 * 1024)

static cmds_load_state_callback_t m_state_callback = NULL;

void cmds_load_register_state_callback(cmds_load_state_callback_t cb)
{
    if (cb)
        m_state_callback = cb;
}

void cmds_load_unregister_state_callback()
{
    m_state_callback = NULL;
}

static void report_current_state(cmds_load_state_t current_state)
{
    if(m_state_callback)
    {
        // disable log before transfer
        if(current_state == CMDS_LOAD_STATE_TRANSFER)
        {
            os_log_disable();
        }
        m_state_callback(current_state);
    }
}

static void help(void) 
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: serial_load offset target\n");
    shellPrint(shell, "   offset(uint32): offset address\n");
    shellPrint(shell, "   target(string): mem, nor, nand\n");
    shellPrint(shell, "Load binary to target through serial port\n");
}

static modem_chan_t get_modem_chan()
{
    os_log_mode_t log_mode = os_log_get_mode();
    if (log_mode == OS_LOG_UART_MODE || log_mode == OS_LOG_RTT_MODE) {
        return MODEM_CHAN_UART;
    }
    else if (log_mode == OS_LOG_USB_MODE) {
        return MODEM_CHAN_USB_ACM;
    }
    else {
        LOGE(__func__, "unsupported logmode");
    }

    return MODEM_CHAN_UART;
}

int serial_load(int argc, char **argv)
{
    uint32_t offset;
    int mode;
    int ret = 0;
    flash_stream_info_t flash_info;
    const int max_recv_size = MAX_LOAD_SIZE;
    char filename[FILENAME_MAX];
    uint8_t *recv_buf = NULL;
    uint8_t *tmp_buf  = NULL;
    modem_chan_t chan;

    Shell *shell = shellGetCurrent();
    if (argc != 3) {
        help();
        return -1;
    }

    chan = get_modem_chan();

    offset = strtoul(argv[1], NULL, 0);
    
    flash_info.spi_num       = SPI_DEVICE_0;
    flash_info.spi_chip_sel  = SPI_CHIP_SELECT_0;
    flash_info.start_rw_addr = (int)offset;
    flash_info.stream_attr   = FLASH_STREAM_WR;
    flash_info.clk_rate      = 40 * 1000 * 1000;

    /* 
     * NOTE: Enable quad r/w by default.
     *       Disable quad r/w if programming SPL(addr 0x0) 
     *       since bootrom don't support quad r/w.
     */
    if (offset != 0) {
        flash_info.quad_en = FLASH_QUAD_EN;
    }
    else {
        flash_info.quad_en = FLASH_QUAD_DIS;
    }
    
    if (strncmp(argv[2], "mem", 3) == 0) {
        mode = LOAD_TO_MEM;
        shellPrint(shell, "Load to memory at offset 0x%08x\n", offset);
    }
    else if (strncmp(argv[2], "nor", 3) == 0) {
        mode = LOAD_TO_FLASH;
        flash_info.flash_type = FLASH_NOR;
        shellPrint(shell, "Load to NOR flash at offset 0x%08x\n", offset);
    }
    else if (strncmp(argv[2], "nand", 4) == 0) {
        mode = LOAD_TO_FLASH;
        flash_info.flash_type = FLASH_NAND;
        shellPrint(shell, "Load to NAND flash at offset 0x%08x\n", offset);
    }
    else {
        shellPrint(shell, "parameters error!\n");
        help();
        return -1;
    }

    /*LOGD(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");*/
    shellPrint(shell, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");
   
    recv_buf = aiva_malloc(max_recv_size);
    if (recv_buf == NULL) {
        LOGE(__func__, "recv_buf is NULL!");
        goto out;
    }
    
    scu_wdt_stop();
    
    // only save and disable irq during uart mode
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }

    os_log_disable();
    userShellSuspend();

    uart_puts_config_t cfg;
    uart_puts_get_config(&cfg);

    int recv_bytes = ota_load_ymodem(chan, 
            cfg.dev, cfg.baudr,
            (uint32_t)recv_buf, LOAD_TO_MEM, max_recv_size, &filename[0]);
   
    if (recv_bytes == 0) {
        goto out;
    }


    if (mode != LOAD_TO_MEM) {
        uint8_t *wr_buf = NULL;
        uint32_t wr_bytes = 0;

        if (!is_compressed_file(filename)) {
            wr_buf   = recv_buf;
            wr_bytes = recv_bytes;
        }
        else {
            unsigned long rd_size;
            int wr_size;

            shellPrint(shell, "Compressed file: %s\n", filename);

            wr_size = max_recv_size;
            tmp_buf = aiva_malloc(wr_size);
            if (tmp_buf == NULL) {
                LOGE(__func__, "Alloc tmp_buf failed!");
                ret = -1;
                goto out;
            }

            rd_size = recv_bytes;
            ret = gunzip(tmp_buf, wr_size, recv_buf, &rd_size);
            if (ret < 0) {
                shellPrint(shell, "Corrupted file: %s\n", filename);
                goto out;
            }

            wr_buf   = tmp_buf;
            wr_bytes = rd_size;
        }
        
        fs_get_lock();
        ret = ota_write_flash_raw_data(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            shellPrint(shell, "ota_write_flash_raw_data failed!\n");
            goto out;
        }

        flash_info.stream_attr = FLASH_STREAM_RD;
        uint32_t crc = crc32(0, (uint8_t *)wr_buf, wr_bytes);
        memset(wr_buf, 0, wr_bytes);
        fs_get_lock();
        ret = ota_verify_flash_data_crc32(&flash_info, wr_buf, wr_bytes, crc);
        fs_release_lock();
        if (ret < 0) {
            goto out;
        }
    }
    else {
        memcpy((void *)offset, recv_buf, recv_bytes);
    }

out:
    userShellResume();
    os_log_enable();

    if (ret < 0) {
        shellPrint(shell, "Verify end: FAIL!\r\n");
    }
    else {
        shellPrint(shell, "Verify end: SUCCESS!\r\n");
    }

    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }

    if (recv_buf != NULL) {
        aiva_free(recv_buf);
    }

    if (tmp_buf != NULL) {
        aiva_free(tmp_buf);
    }

    return 0;
}

int serial_load_file(const char *filename)
{
    int ret = 0;
    char *recv_buf = NULL;
    char *read_buf = NULL;
    const int buf_size = 2*1024*1024;
    int recv_bytes = 0;
    modem_chan_t chan;
    
    Shell *shell = shellGetCurrent();
    fs_file_t *fp = NULL;
    
    chan = get_modem_chan();

    
    if (get_curr_fs() == NULL) {
        shellPrint(shell, "fs hasn't been mounted!\n");
        ret = -1;
        goto out;
    }

    recv_buf = aiva_malloc(buf_size);
    if (recv_buf == NULL) {
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    os_log_disable();
    
    scu_wdt_stop();
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }
    userShellSuspend();
    
    uart_puts_config_t cfg;
    uart_puts_get_config(&cfg);

    recv_bytes = ota_load_ymodem(chan, cfg.dev, cfg.baudr,
            (uint32_t)recv_buf, LOAD_TO_MEM, buf_size, NULL);
    userShellResume();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }


    if (recv_bytes == 0) {
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    fp = fs_file_open(filename, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
    if (fp == NULL) {
        shellPrint(shell, "Open file %s error!\n", filename);
        ret = -1;
        goto out;
    }
    shellPrint(shell, "Write %d bytes into file %s\n", recv_bytes, filename);
    ret = fs_file_write(fp, recv_buf, recv_bytes);
    if (ret != recv_bytes) {
        shellPrint(shell, "Write error! %d bytes has been written, %d expected!\n",
                ret, recv_bytes);
        ret = -1;
        goto out;
    }

    // Verify
    shellPrint(shell, "Verify start: ...\n"); 

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    fs_file_seek(fp, 0, FS_SEEK_SET);
    read_buf = aiva_malloc(recv_bytes);

    ret = fs_file_read(fp, read_buf, recv_bytes);
    if (ret != recv_bytes) {
        shellPrint(shell, "Read error! %d bytes has been written, %d expected!\n",
                ret, recv_bytes);
        ret = -1;
        goto out;
    }
 
    if (memcmp(read_buf, recv_buf, recv_bytes) == 0) {
        shellPrint(shell, "Verify end: SUCCESS!\n");
    }
    else {
        shellPrint(shell, "Verify end: FAIL!\n");
    }


out:
    os_log_enable();
    if (recv_buf != NULL) {
        aiva_free(recv_buf);
    }
    if (fp != NULL) {
        fs_file_close(fp);
    }
    if (read_buf != NULL) {
        aiva_free(read_buf);
    }

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}



int serial_load_eeprom()
{
#ifdef USE_SL18
    eeprom_ctrl eeprom_entity = {
        .name        = "P24S64E",
        .dev_addr    = 0x50,
        .page_num    = 256,
        .page_size   = 32,
        .wr_delay    = 5,
        .i2c.dev_num = I2C_DEVICE_1,
        .i2c.bus_clk = 100000,
    };
#else
    eeprom_ctrl eeprom_entity = {
        .name        = "FM24C02",
        .dev_addr    = 0x50,
        .page_num    = 32,
        .page_size   = 8,
        .wr_delay    = 5,
        .i2c.dev_num = I2C_DEVICE_1,
        .i2c.bus_clk = 100000,
    };
#endif

    int ret = 0;
    int file_recv_bytes = 0;
    int write_result, read_result;
    uint8_t recv_buf[256];
    eeprom_vde_param_t eeprom_read_data;
    int filelength = sizeof(eeprom_vde_param_t);
    int datalength = sizeof(eeprom_vde_param_t) - sizeof(eeprom_vde_hdr_t);
    uint16_t crc16 = 0;
    modem_chan_t chan;

    Shell *shell = shellGetCurrent();
    
    chan = get_modem_chan();

    scu_wdt_stop();
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    os_log_disable();
    userShellSuspend();
    
    uart_puts_config_t cfg;
    uart_puts_get_config(&cfg);

    file_recv_bytes = ota_load_ymodem(chan, cfg.dev, cfg.baudr,
            (unsigned long)recv_buf, LOAD_TO_MEM, 256, NULL);

    userShellResume();

    if(file_recv_bytes != filelength) {
        shellPrint(shell, "read file error, file too long or too short.\n");
        ret = -1;
        goto error_out;
    }
    
    eeprom_vde_param_t *data_ptr = (eeprom_vde_param_t *)(recv_buf);

    if (data_ptr->header.length != datalength) {
        shellPrint(shell, "header.length != datalength.\n");
        ret = -1;
        goto error_out;
    }

    crc_update_buffer(&crc16, (uint8_t *)&data_ptr->header.id, datalength + sizeof(data_ptr->header.id));
    if (data_ptr->header.crc16 != crc16) {
        shellPrint(shell, "crc16 check failed, header.crc16 0x%x, crc16 0x%x", data_ptr->header.crc16, crc16);
        ret = -1;
        goto error_out;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    write_result = eeprom_write(&eeprom_entity, (uint8_t *)recv_buf, 0, filelength);
    if(write_result != 0) {
        shellPrint(shell, "eeprom write error.\n");
        ret = -1;
        goto error_out;
    }

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    shellPrint(shell, "Verify start: ...\n"); 
    read_result = read_eeprom_vde_param(&eeprom_entity, &eeprom_read_data);

    if(read_result != 0) {
        shellPrint(shell, "eeprom read back error.\n");
        ret = -1;
        goto error_out;
    }

    dump_eeprom_vde_param(&eeprom_read_data);
    
    if (memcmp(&eeprom_read_data, recv_buf, filelength) == 0) {
        shellPrint(shell, "Verify end: SUCCESS.\n");
    }
    else {
        ret = -1;
        goto error_out;
    }

    os_log_enable();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }

    report_current_state(CMDS_LOAD_STATE_SUCCESS);
    return 0;

error_out:
    os_log_enable();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }
    shellPrint(shell, "Verify end: FAIL!\n");
    report_current_state(CMDS_LOAD_STATE_FAILED);


    return ret;

}

static int _serial_load_part(char *part_name, uart_device_number_t uart_num, int uart_baudr)
{
    (void)(uart_num);
    (void)(uart_baudr);

    flash_stream_info_t flash_info;
    int ret;
    uint32_t mask_flags;
    uint8_t *recv_buf = NULL;
    int recv_bytes = 0;
    char filename[FILENAME_MAX+1];
    uint8_t *tmp_buf = NULL;
    bool is_fw = false;
    flash_part_hdr_t part_hdr;
    uint8_t *wr_buf   = NULL;
    uint32_t wr_bytes = 0;
    modem_chan_t chan;

    
    Shell *shell = shellGetCurrent();
    
    chan = get_modem_chan();

    flash_info.spi_num      = SPI_DEVICE_0;
    flash_info.spi_chip_sel = SPI_CHIP_SELECT_0;
    flash_info.stream_attr  = FLASH_STREAM_WR;
    flash_info.quad_en      = FLASH_QUAD_EN;
    /*flash_info.flash_type   = boot_get_flash_type(SPI_DEVICE_0, 4);*/
    flash_info.flash_type   = os_get_flash_type();
    flash_info.clk_rate     = 40 * 1000 * 1000;

    if (flash_info.flash_type == FLASH_NOR) {
        mask_flags = MASK_FLAG_NOR_FLASH;
    }
    else {
        mask_flags = MASK_FLAG_NAND_FLASH;
    }

    LOGD(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");

    scu_wdt_stop();
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }

    ret = flash_part_get(part_name, mask_flags, &part_hdr);
    if (ret < 0) {
        goto out;
    }

    shellPrint(shell, "Find partition '%s' at 0x%08x...\n", part_name, part_hdr.offset);
   
    flash_info.start_rw_addr = part_hdr.offset;

    recv_buf = aiva_malloc(part_hdr.size);
    if (recv_buf == NULL) {
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    os_log_disable();
    userShellSuspend();
    
    uart_puts_config_t cfg;
    uart_puts_get_config(&cfg);

    recv_bytes = ota_load_ymodem(chan, cfg.dev, cfg.baudr,
            (uint32_t)recv_buf, LOAD_TO_MEM, part_hdr.size, filename);
    userShellResume();
    if (recv_bytes == 0) {
        ret = -1;
        goto out;
    }

    if (strncmp(part_name, "fw", 2) == 0) {
        is_fw = true;
    }

    shellPrint(shell, "Recv filename: %s\n", filename);
    if (!is_compressed_file(filename)) {
        wr_buf   = recv_buf;
        wr_bytes = recv_bytes;
    }
    else {
        unsigned long rd_size;
        int wr_size;
        
        wr_size = recv_bytes * 3;
        if (wr_size < 2 * 1024 * 1024) {
            wr_size = 2 * 1024 * 1024;
        }

        tmp_buf = aiva_malloc(wr_size);
        if (tmp_buf == NULL) {
            ret = -1;
            goto out;
        }

        rd_size = recv_bytes;
        ret = gunzip(tmp_buf, wr_size, recv_buf, &rd_size);
        if (ret < 0) {
            shellPrint(shell, "Corrupted file: %s\n", filename);
            goto out;
        }
        configASSERT((int)rd_size < wr_size);

        wr_buf   = tmp_buf;
        wr_bytes = rd_size;

        // free recv_buf to save memory, otherwise malloc in ota_verify_flash_data might fail!
        aiva_free(recv_buf);
        recv_buf = NULL;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    if (is_fw) {
        // NOTE: lock file system
        fs_get_lock();
        ret = ota_write_flash_fw(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            shellPrint(shell, "ota_write_flash_fw failed!\n");
            goto out;
        }
    }
    else {
        fs_get_lock();
        ret = ota_write_flash_raw_data(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            shellPrint(shell, "ota_write_flash_raw_data failed!\n");
            goto out;
        }
    }

    flash_info.stream_attr = FLASH_STREAM_RD;
    report_current_state(CMDS_LOAD_STATE_VERIFY);

    uint32_t crc = crc32(0, (uint8_t *)wr_buf, wr_bytes);
    memset(wr_buf, 0, wr_bytes);
    fs_get_lock();
    shellPrint(shell, "Verify start: ...\r\n"); 
    ret = ota_verify_flash_data_crc32(&flash_info, wr_buf, wr_bytes, crc);
    if (ret < 0) {
        shellPrint(shell, "Verify end: FAIL!\r\n");
    }
    else {
        shellPrint(shell, "Verify end: SUCCESS!\r\n");
    }
    fs_release_lock();
    if (ret < 0) {
        goto out;
    }

out:
    os_log_enable();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }

    if (recv_buf != NULL) {
        aiva_free(recv_buf);
    }

    if (tmp_buf != NULL) {
        aiva_free(tmp_buf);
    }

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

static void serial_load_part_help(void) 
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: load_part [partition] [uart_num|0] [uart_baudr|OTA_BAUD_RATE]\n");
    shellPrint(shell, "Load binary to partition through serial port\n");
}


int serial_load_part(int argc, char *argv[])
{
    int ret;
    uart_device_number_t uart_num = UART_DEVICE_0;
    int uart_baudr = OTA_BAUD_RATE;

    if (argc < 2) {
        serial_load_part_help();
        return -1;
    }

    if (argc == 3) {
        uart_num   = strtol(argv[2], NULL, 0);
    }
    else if (argc == 4) {
        uart_num   = strtol(argv[2], NULL, 0);
        uart_baudr = strtol(argv[3], NULL, 0); 
    }

    LOGD(__func__, "part %s, uart_num %d, uart_baudr %d",
            argv[1], uart_num, uart_baudr);

    ret = _serial_load_part(argv[1], uart_num, uart_baudr);

    return ret;
}

static void cons_cmd_load_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: load [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - addr: load to the specific address of mem/flash, ARGS: [offset] [target]\r\n"
                "      - part: load to flash partition, ARGS: [part_name]\r\n"
                "      - file: load to file, ARGS: [filename]\r\n"
                "      - eeprom: load file to sensor eeprom, ARGS: none\r\n"
            );
}

int cons_cmd_load(int argc, char *argv[])
{
    if (argc == 1) {
        cons_cmd_load_help();
        return -1;
    }

    int shell_origin_priority = os_shell_app_priority_get();
    int shell_cur_priority = (shell_origin_priority + 4) > configMAX_PRIORITIES ? 
                                    configMAX_PRIORITIES : (shell_origin_priority + 4);
    os_shell_app_priority_set(shell_cur_priority);

    if (strncmp(argv[1], "addr", 4) == 0) {
        serial_load(argc-1, &argv[1]);
    }
    else if (strncmp(argv[1], "part", 4) == 0) {
        serial_load_part(argc-1, &argv[1]);
    }
    else if (strncmp(argv[1], "file", 4) == 0) {
        serial_load_file(argv[2]);
    }
    else if (strncmp(argv[1], "eeprom", 6) == 0) {
        serial_load_eeprom();
    }
    else {
        cons_cmd_load_help();
    }

    os_shell_app_priority_set(shell_origin_priority);

    return 0;
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
load, cons_cmd_load, Load binary from serial port);
