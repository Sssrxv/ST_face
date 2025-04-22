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
#include "shell_port.h"
#include "os_shell_app.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif

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

int serial_upload_file(const char *filename)
{
    Shell *shell = shellGetCurrent();
    modem_chan_t chan;

    if (!filename) {
        shellPrint(shell, "upload_error, file name is NULL\n");
        return -1;
    }
    
    if (get_curr_fs() == NULL) {
        shellPrint(shell, "upload_error, fs hasn't been mounted!\n");
        return -2;
    }

    fs_file_t *fp = fs_file_open(filename, FS_O_RDONLY);
    if (fp == NULL) {
        shellPrint(shell, "upload_error, open file %s error!\n", filename);
        return -3;
    }
    
    chan = get_modem_chan();

    int ret = 0;
    char *snd_buf = NULL;
    const uint32_t file_size = fs_file_size(fp);
    if (0 == file_size) {
        shellPrint(shell, "upload_error, file_size is 0\n");
        ret = -4;
        goto out;
    }   
    snd_buf = (char*)aiva_malloc(file_size);
    if (!snd_buf) {
        shellPrint(shell, "upload_error, aiva_malloc(%d) error\n", file_size);
        ret = -5;
        goto out;
    }
    ret = fs_file_read(fp, snd_buf, file_size);
    if ((uint32_t)ret != file_size) {
        shellPrint(shell, "upload_error, read file error! %d bytes has been read, %d expected!\n",
                ret, file_size);
        ret = -6;
        goto out;
    }
    
    // only save and disable irq during uart mode
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }

    userShellSuspend();
    os_log_disable();
    scu_wdt_stop();

    connection_info_t info;
    info.mode       = xyzModem_ymodem;
    info.uart_num   = UART_DEVICE_0; // TODO
    info.uart_baudr = 460800;
    info.chan = chan;
    memcpy(info.filename, filename, strlen(filename) + 1);

    ret = ymodem_transfer(&info, (uint8_t *)snd_buf, file_size);
    shellPrint(shell, "upload, ymodem_transfer ret %d\n", ret);
    ret = 0 > ret ? -7 : ret;

    userShellResume();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }
    os_log_enable();

out:

    if (fp != NULL) {
        fs_file_close(fp);
    }
    if (snd_buf != NULL) {
        aiva_free(snd_buf);
    }
    return ret;
}

int serial_upload_eeprom(void)
{
    Shell *shell = shellGetCurrent();
    eeprom_ctrl eeprom = {
        .name        = "FM24C02",
        .dev_addr    = 0x50,
        .page_num    = 32,
        .page_size   = 8,
        .wr_delay    = 5,
        .i2c.dev_num = I2C_DEVICE_1,
        .i2c.bus_clk = 400000,
    };
    uint32_t eeprom_size = eeprom.page_num * eeprom.page_size;
    uint32_t calib_file_size = sizeof(eeprom_vde_param_t);
    const uint32_t buf_size = calib_file_size > eeprom_size ? eeprom_size : calib_file_size;
    char *snd_buf = (char*)aiva_malloc(buf_size);
    if (!snd_buf) {
        shellPrint(shell, "upload_error, aiva_malloc(%d) error\n", buf_size);
        return -1;
    }
    int ret = eeprom_read(&eeprom, (uint8_t *)snd_buf, 0, buf_size);
    if (ret != 0) {
        shellPrint(shell, "upload_error, reeprom_read error, %d\n", ret);
        ret = -2;
        goto out;
    }

    userShellSuspend();
    os_log_disable();
    scu_wdt_stop();
    // only save and disable irq during uart mode
    modem_chan_t chan = get_modem_chan();
    if (chan == MODEM_CHAN_UART) {
        ota_save_local_irqs();
    }

    connection_info_t info;
    info.mode       = xyzModem_ymodem;
    info.uart_num   = UART_DEVICE_0; // TODO
    info.uart_baudr = 460800;
    info.chan = chan;
    memcpy(info.filename, "eeprom", strlen("eeprom") + 1);

    ret = ymodem_transfer(&info, (uint8_t *)snd_buf, buf_size);
    shellPrint(shell, "upload, ymodem_transfer ret %d\n", ret);
    ret = 0 > ret ? -3 : ret;
    
    userShellResume();
    scu_wdt_feed();
    scu_wdt_start();
    if (chan == MODEM_CHAN_UART) {
        ota_restore_local_irqs();
    }
    os_log_enable();

out:
    if (snd_buf != NULL) {
        aiva_free(snd_buf);
    }
    return ret;
}

static void cons_cmd_upload_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: upload [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - file: upload file, ARGS: [filename]\r\n"
                "      - mem:  upload the specific address data of mem, ARGS: [addr] [size]\r\n"
                "      - flash:  upload the specific address data of flash, ARGS: [off] [size]\r\n"
                "      - eeprom: upload data in eeprom, ARGS: none\r\n"
            );
}

int cons_cmd_upload(int argc, char *argv[])
{
    if (argc == 1) {
        cons_cmd_upload_help();
        return -1;
    }

    int ret = 0;
    int shell_origin_priority = os_shell_app_priority_get();
    int shell_cur_priority = (shell_origin_priority + 4) > configMAX_PRIORITIES ? 
                                    configMAX_PRIORITIES : (shell_origin_priority + 4);
    os_shell_app_priority_set(shell_cur_priority);

    if (strncmp(argv[1], "file", 4) == 0) {
        ret = serial_upload_file(argv[2]);
    }
    else if (strncmp(argv[1], "mem", 3) == 0) {
        LOGE(__func__, "upload_error, unsupported command");
    }
    else if (strncmp(argv[1], "flash", 4) == 0) {
        LOGE(__func__, "upload_error, unsupported command");
    }
    else if (strncmp(argv[1], "eeprom", 6) == 0) {
        ret = serial_upload_eeprom();
    }
    else {
        cons_cmd_upload_help();
    }

    os_shell_app_priority_set(shell_origin_priority);

    return ret;
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
upload, cons_cmd_upload, Upload binary from serial port);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
