#include "uvc_camera.h"
#include <stdlib.h>
#include <string.h>
#include "boot_flash.h"
#include "syslog.h"
#include "sysctl.h"
#include "flash_stream.h"
#include "flash_part.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "fs_file.h"
#include "aiva_malloc.h"
#include "eeprom.h"
#include "AI3100.h"
#include "libota.h"
#include "os_log.h"
#include "fs_file.h"
#include "aiva_scu_wdt.h"
#include "os_init_app.h"
#include "aiva_sleep.h"
#include "lib_simpl_trans.h"
#include "shell_port.h"
#include "os_shell_app.h"
#include "spi.h"

static int usb_acm_recv(const void *devh, void *buf, int buf_len, int timeout_ms)
{
    (void)(devh);

    int ret = 0;

    while (timeout_ms--) {
        int max_size = buf_len;
        ret = usb_function_acm_recv_buffer(buf, &max_size, USB_FUNC_NO_WAIT);
        if (ret > 0) {
            break;
        }
        aiva_msleep(1);
    }

    LOGD(__func__, "ret is %d", ret);

    return ret;
}

static int usb_acm_send(const void *devh, void *buf, int bytes, int timeout_ms)
{
    (void)(devh);

    int ret = 0;

    while (timeout_ms--) {
        ret = usb_function_acm_send_buffer(buf, bytes, USB_FUNC_NO_WAIT);
        if (ret > 0) {
            break;
        }
        aiva_msleep(1);
    }
    LOGD(__func__, "ret is %d", ret);

    return ret;
}


int usb_acm_upload_part(const char *part_name)
{
    int ret = 0;
    uint8_t *snd_buf = NULL;
    flash_stream_info_t flash_info;
    flash_part_hdr_t part_hdr;
    uint32_t mask_flags;

    flash_info.spi_num      = SPI_DEVICE_0;
    flash_info.spi_chip_sel = SPI_CHIP_SELECT_0;
    flash_info.stream_attr  = FLASH_STREAM_RD;
    flash_info.quad_en      = FLASH_QUAD_EN;
    flash_info.flash_type   = os_get_flash_type();
    flash_info.clk_rate     = 40 * 1000 * 1000;

    if (flash_info.flash_type == FLASH_NOR) {
        mask_flags = MASK_FLAG_NOR_FLASH;
    }
    else {
        mask_flags = MASK_FLAG_NAND_FLASH;
    }

    /*LOGI(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");*/
    scu_wdt_stop();
    ret = flash_part_get((char *)part_name, mask_flags, &part_hdr);
    if (ret < 0) {
        LOGE(__func__, "Can't find part %s", part_name);
        goto out;
    }

    const uint32_t buf_size = part_hdr.size;
    if (0 == buf_size) {
        LOGE(__func__, "upload_error, part size is 0\n");
        ret = -1;
        goto out;
    }

    /*LOGI(__func__, "Find part '%s' at offset 0x%08x, size %d", part_name, part_hdr.offset, part_hdr.size);*/

    flash_info.start_rw_addr = part_hdr.offset;

    snd_buf = (uint8_t *)aiva_malloc(buf_size);
    if (!snd_buf) {
        LOGE(__func__, "upload_error, aiva_malloc(%d) error\n", buf_size);
        ret = -1;
        goto out;
    }

    ret = flash_stream_open(&flash_info);
    if (ret < 0) {
        LOGE(__func__, "Open flash stream error!");
        goto out;
    }

    ret = flash_stream_read(snd_buf, buf_size);
    if (ret < 0) {
        LOGE(__func__, "Read flash stream error!");
        goto out;
    }

    flash_stream_close();

    /*LOGI(__func__, "Read %d bytes from part %s successfully!", buf_size, part_name);*/

    // send file
    simpl_trans_op_t op = {
        .init = NULL,
        .send = usb_acm_send,
        .recv = usb_acm_recv,
        .uninit = NULL,
    };
    
    os_log_disable();
    userShellSuspend();
    simpl_handle_t *hdl = simpl_trans_init(&op, NULL);
    ret = simpl_trans_send(hdl, snd_buf, buf_size);
    simpl_trans_uninit(hdl);
    os_log_enable();
    userShellResume();
    
    if (ret < 0) {
        LOGD(__func__, "simpl_trans_send error %d", ret);
        goto out;
    }

out:
    scu_wdt_feed();
    scu_wdt_start();

    if (snd_buf != NULL) {
        aiva_free(snd_buf);
    }

    return ret;
}


int usb_acm_upload_file(const char *filename)
{
    int ret = 0;
    char *snd_buf = NULL;
    fs_file_t *fp = NULL;

    if (!filename) {
        LOGD(__func__, "upload_error, file name is NULL\n");
        goto out;
    }
    
    if (get_curr_fs() == NULL) {
        LOGD(__func__, "upload_error, fs hasn't been mounted!\n");
        return -1;
    }

    fp = fs_file_open(filename, FS_O_RDONLY);
    if (fp == NULL) {
        LOGD(__func__, "upload_error, open file %s error!\n", filename);
        goto out;
    }

    const uint32_t file_size = fs_file_size(fp);
    if (0 == file_size) {
        LOGD(__func__, "upload_error, file_size is 0\n");
        ret = -1;
        goto out;
    }   
    snd_buf = (char*)aiva_malloc(file_size);
    if (!snd_buf) {
        LOGD(__func__, "upload_error, aiva_malloc(%d) error\n", file_size);
        ret = -1;
        goto out;
    }
    ret = fs_file_read(fp, snd_buf, file_size);
    if ((uint32_t)ret != file_size) {
        LOGD(__func__, "upload_error, read file error! %d bytes has been read, %d expected!\n",
                ret, file_size);
        ret = -1;
        goto out;
    }

    // send file
    simpl_trans_op_t op = {
        .init = NULL,
        .send = usb_acm_send,
        .recv = usb_acm_recv,
        .uninit = NULL,
    };
    
    os_log_disable();
    userShellSuspend();
    simpl_handle_t *hdl = simpl_trans_init(&op, NULL);
    ret = simpl_trans_send(hdl, snd_buf, file_size);
    simpl_trans_uninit(hdl);
    os_log_enable();
    userShellResume();
    
    if (ret < 0) {
        LOGD(__func__, "simpl_trans_send error %d", ret);
        goto out;
    }

out:
    if (fp != NULL) {
        fs_file_close(fp);
    }
    if (snd_buf != NULL) {
        aiva_free(snd_buf);
    }
    return ret;
}

int usb_acm_upload_eeprom(void)
{
#ifdef USE_SL18
    eeprom_ctrl eeprom = {
        .name        = "P24S64E",
        .dev_addr    = 0x50,
        .page_num    = 256,
        .page_size   = 32,
        .wr_delay    = 5,
        .i2c.dev_num = I2C_DEVICE_1,
        .i2c.bus_clk = 200000,
    };
#else
    eeprom_ctrl eeprom = {
        .name        = "FM24C02",
        .dev_addr    = 0x50,
        .page_num    = 32,
        .page_size   = 8,
        .wr_delay    = 5,
        .i2c.dev_num = I2C_DEVICE_1,
        .i2c.bus_clk = 200000,
    };
#endif


    const uint32_t buf_size = eeprom.page_num * eeprom.page_size;
    uint8_t *snd_buf = aiva_malloc(buf_size);
    if (!snd_buf) {
        LOGD(__func__, "upload_error, aiva_malloc(%d) error\n", buf_size);
        return -1;
    }

    int ret = eeprom_read(&eeprom, snd_buf, 0, buf_size);
    if (ret != 0) {
        LOGD(__func__, "upload_error, reeprom_read error, %d\n", ret);
        ret = -1;
        goto out;
    }

    // send file
    simpl_trans_op_t op = {
        .init = NULL,
        .send = usb_acm_send,
        .recv = usb_acm_recv,
        .uninit = NULL,
    };
    
    simpl_handle_t *hdl = simpl_trans_init(&op, NULL);
    os_log_disable();
    userShellSuspend();
    ret = simpl_trans_send(hdl, snd_buf, buf_size);
    os_log_enable();
    userShellResume();
    
    if (ret < 0) {
        LOGD(__func__, "simpl_trans_send error %d", ret);
        goto out;
    }

out:
    if (snd_buf != NULL) {
        aiva_free(snd_buf);
    }
    return ret;
}

static void usb_acm_upload_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: usb_acm_upload [OPTIONS]\r\n"
                "   [OPTIONS]: \r\n"
                "      - file: upload file, ARGS: [filename]\r\n"
                "      - eeprom: upload data in eeprom, ARGS: none\r\n"
            );
}

int usb_acm_upload(int argc, char *argv[])
{
    if (argc == 1) {
        usb_acm_upload_help();
        return -1;
    }

    int ret = 0;
    int shell_origin_priority = os_shell_app_priority_get();
    int shell_cur_priority = (shell_origin_priority + 4) > configMAX_PRIORITIES ? 
                                    configMAX_PRIORITIES : (shell_origin_priority + 4);
    os_shell_app_priority_set(shell_cur_priority);

    if (strncmp(argv[1], "file", 4) == 0) {
        ret = usb_acm_upload_file(argv[2]);
    }
    else if (strncmp(argv[1], "part", 4) == 0) {
        ret = usb_acm_upload_part(argv[2]);
    }
    else if (strncmp(argv[1], "eeprom", 6) == 0) {
        ret = usb_acm_upload_eeprom();
    }
    else {
        usb_acm_upload_help();
    }

    os_shell_app_priority_set(shell_origin_priority);

    /*os_log_set_mode(OS_LOG_UART_MODE);*/
    /*userShellSetMode(USER_SHELL_UART_MODE);*/
    /*LOGD(__func__, "out...");*/

    return ret;
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
usb_acm_upload, usb_acm_upload, usb acm upload);








