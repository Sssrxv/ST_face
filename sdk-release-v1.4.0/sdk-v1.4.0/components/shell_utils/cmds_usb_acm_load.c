#include "cmds_usb_acm_load.h"
#include "uvc_comm.h"
#include "boot_flash.h"
#include "spi.h"
#include "flash.h"
#include "syslog.h"
#include "flash_stream.h"
#include "flash_part.h"
#include "shell.h"
#include "fs_file.h"
#include "eeprom.h"
#include "gunzip.h"
#include "AI3100.h"
#include "libota.h"
#include "fs_file.h"
#include "os_init_app.h"
#include "lib_simpl_trans.h"
#include "shell_port.h"
#include "os_shell_app.h"
#include "aiva_scu_wdt.h"
#include "aiva_sleep.h"
#include "md5.h"
#include "crc16.h"
#include "crc32.h"
#include "libvde.h"
#include "os_log.h"
#include <stdlib.h>
#include <string.h>

static cmds_load_state_callback_t m_state_callback = NULL;

void cmds_acm_load_register_state_callback(cmds_load_state_callback_t cb)
{
    if (cb)
        m_state_callback = cb;
}

void cmds_acm_load_unregister_state_callback()
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

    /*LOGI(__func__, "ret is %d", ret);*/

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
    /*LOGI(__func__, "ret is %d", ret);*/

    return ret;
}

static int _usb_acm_load(void *recv_buf, int recv_buf_len)
{
    int ret = 0;

    // recv file
    simpl_trans_op_t op = {
        .init = NULL,           // usb acm shell has inited usb.
        .send = usb_acm_send,
        .recv = usb_acm_recv,
        .uninit = NULL,
    };
    
    simpl_handle_t *hdl = simpl_trans_init(&op, NULL);

    os_log_disable();
    userShellSuspend();
    ret = simpl_trans_recv(hdl, recv_buf, recv_buf_len);
    os_log_enable();
    userShellResume();

    if (ret < 0) {
        LOGI(__func__, "simpl_trans_recv error %d", ret);
    }

    simpl_trans_uninit(hdl);

    return ret;
}




static void cmds_usb_acm_load_help(void)
{
    Shell *shell = shellGetCurrent();
    shellPrint(shell, "Usage: load [OPTIONS] [ARGS] \r\n"
                "   [OPTIONS]: \r\n"
                "      - addr: load to the specific address of mem/flash, ARGS: [offset] [target]\r\n"
                "      - part: load to flash partition, ARGS: [part_name] [filename]\r\n"
                "      - file: load to file, ARGS: [filename]\r\n"
                "      - ota:  load to otafile, ARGS: [ota filename] [filesize]\r\n"
            );
}

int usb_acm_load_file(const char *filename)
{
    int ret = -1;
    char *read_buf = NULL;
    
    char *recv_buf = NULL;
    const int recv_buf_len = 8 * 1024 * 1024;
    int recv_bytes = 0;
    const char *filename_strip = NULL;

    // strip path and get the bare filename
    filename_strip = strrchr(filename, '/');
    if (filename_strip != NULL) {
        filename_strip++;
    }
    else {
        filename_strip = filename;
    }

    recv_buf = malloc(recv_buf_len);
    configASSERT(recv_buf != NULL);

    fs_file_t *fp = NULL;
    
    if (get_curr_fs() == NULL) {
        LOGI(__func__, "fs hasn't been mounted!\n");     
        ret = -1;   
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    ret = _usb_acm_load(recv_buf, recv_buf_len);
    if (ret < 0) {
        LOGI(__func__, "simpl_trans_recv error %d", ret);
        goto out;
    }

    recv_bytes = ret;

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    // write to fs
    fp = fs_file_open(filename_strip, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
    if (fp == NULL) {
        LOGI(__func__, "Open file %s error!\n", filename);
        ret = -1;
        goto out;
    }
    ret = fs_file_write(fp, recv_buf, recv_bytes);
    if (ret != recv_bytes) {
        LOGI(__func__, "Write error! %d bytes has been written, %d expected!\n", ret, recv_bytes);
        ret = -1;
        goto out;
    }
    LOGI(__func__, "Write %d bytes into file %s\n", recv_bytes, filename);

    // Verify
    LOGI(__func__, "Verify start: ...\n"); 

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    fs_file_seek(fp, 0, FS_SEEK_SET);
    read_buf = malloc(recv_bytes);

    ret = fs_file_read(fp, read_buf, recv_bytes);
    if (ret != recv_bytes) {
        LOGI(__func__, "Read error! %d bytes has been written, %d expected!\n", ret, recv_bytes);
        ret = -1;
        goto out;
    }
 
    if (memcmp(read_buf, recv_buf, recv_bytes) == 0) {
        LOGI(__func__, "Verify end: SUCCESS!\n");
        ret = 0;
    }
    else {
        LOGI(__func__, "Verify end: FAIL!\n");
        ret = -1;
    }
    
out:
    if (fp != NULL) {
        fs_file_close(fp);
    }
    if (read_buf != NULL) {
        free(read_buf);
    }
    if (recv_buf != NULL) {
        free(recv_buf);
    }

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

int usb_acm_load_eeprom(void)
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
    int write_result, read_result;
    uint8_t recv_buf[256];
    int recv_bytes = 256;

    eeprom_vde_param_t eeprom_read_data;
    int filelength = sizeof(eeprom_vde_param_t);
    int datalength = sizeof(eeprom_vde_param_t) - sizeof(eeprom_vde_hdr_t);
    uint16_t crc16 = 0;

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    recv_bytes = _usb_acm_load(recv_buf, recv_bytes);
    if(recv_bytes != filelength) {
        LOGI(__func__, "read file error, file too long or too short.\n");
        ret = -1;
        goto error_out;
    }
    
    eeprom_vde_param_t *data_ptr = (eeprom_vde_param_t *)(recv_buf);

    if (data_ptr->header.length != datalength) {
        LOGI(__func__, "header.length != datalength.\n");
        ret = -1;
        goto error_out;
    }

    crc_update_buffer(&crc16, (uint8_t *)&data_ptr->header.id, datalength + sizeof(data_ptr->header.id));
    if (data_ptr->header.crc16 != crc16) {
        LOGI(__func__, "crc16 check failed, header.crc16 0x%x, crc16 0x%x", data_ptr->header.crc16, crc16);
        ret = -1;
        goto error_out;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    write_result = eeprom_write(&eeprom_entity, (uint8_t *)recv_buf, 0, filelength);
    if(write_result != 0) {
        LOGI(__func__, "eeprom write error.\n");
        ret = -1;
        goto error_out;
    }

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    LOGI(__func__, "Verify start: ...\n"); 
    read_result = read_eeprom_vde_param(&eeprom_entity, &eeprom_read_data);

    if(read_result != 0) {
        LOGI(__func__, "eeprom read back error.\n");
        ret = -1;
        goto error_out;
    }

    dump_eeprom_vde_param(&eeprom_read_data);
    
    if (memcmp(&eeprom_read_data, recv_buf, filelength) == 0) {
        LOGI(__func__, "Verify end: SUCCESS.\n");
    }
    else {
        goto error_out;
    }

    return 0;

error_out:
    LOGI(__func__, "Verify end: FAIL!\n");

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

int usb_acm_load_any(uint32_t offset, const char *target)
{
    flash_stream_info_t flash_info;
    int ret = 0;
    uint32_t mask_flags;
    uint8_t *recv_buf = NULL;
    int recv_bytes = 0;
    uint8_t *wr_buf   = NULL;
    uint32_t wr_bytes = 0;
    const uint32_t max_buf_len = 18*1024*1024;

    flash_info.spi_num      = SPI_DEVICE_0;
    flash_info.spi_chip_sel = SPI_CHIP_SELECT_0;
    flash_info.stream_attr  = FLASH_STREAM_WR;
    /*flash_info.flash_type   = os_get_flash_type();*/
    flash_info.clk_rate     = 40 * 1000 * 1000;

    if (flash_info.flash_type == FLASH_NOR) {
        mask_flags = MASK_FLAG_NOR_FLASH;
    }
    else {
        mask_flags = MASK_FLAG_NAND_FLASH;
    }

    (void)mask_flags; // warning free

    if (offset == 0) {
        flash_info.quad_en = FLASH_QUAD_DIS;
    }
    else {
        flash_info.quad_en = FLASH_QUAD_EN;
    }

    if (strncmp(target, "nand", 4) == 0) {
        flash_info.flash_type = FLASH_NAND;
    }
    else {
        flash_info.flash_type = FLASH_NOR;
    }
    
    LOGI(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");
    scu_wdt_stop();

    LOGI(__func__, "Update flash offset at 0x%08x...\n", offset);
    
    flash_info.start_rw_addr = offset;
    
    recv_buf = malloc(max_buf_len);
    if (recv_buf == NULL) {
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    recv_bytes = _usb_acm_load(recv_buf, max_buf_len);
    if (recv_bytes <= 0) {
        ret = -1;
        goto out;
    }

    wr_buf   = recv_buf;
    wr_bytes = recv_bytes;

    LOGD(__func__, "recv_bytes is %d bytes done!", recv_bytes);
#if 1
    {
        report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

        fs_get_lock();
        ret = ota_write_flash_raw_data(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            LOGE(__func__, "ota_write_flash_raw_data failed!\n");
            goto out;
        }
    }

    flash_info.stream_attr = FLASH_STREAM_RD;

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    uint32_t crc = crc32(0, (uint8_t *)wr_buf, wr_bytes);
    memset(wr_buf, 0, wr_bytes);
    fs_get_lock();
    LOGI(__func__, "Verify start: ...\r\n"); 

    ret = ota_verify_flash_data_crc32(&flash_info, wr_buf, wr_bytes, crc);
    if (ret < 0) {
        LOGI(__func__, "Verify end: FAIL!\r\n");
    }
    else {
        LOGI(__func__, "Verify end: SUCCESS!\r\n");
    }
    fs_release_lock();

#endif

out:
    scu_wdt_feed();
    scu_wdt_start();

    if (recv_buf != NULL) {
        free(recv_buf);
    }

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

int usb_acm_load_part(const char *part_name, const char *filename)
{
    flash_stream_info_t flash_info;
    flash_part_hdr_t part_hdr;
    int ret = 0;
    uint32_t mask_flags;
    uint8_t *recv_buf = NULL;
    int recv_bytes = 0;
    bool is_fw = false;
    uint8_t *wr_buf   = NULL;
    uint32_t wr_bytes = 0;
    uint8_t *tmp_buf = NULL;

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
    
    LOGI(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");
    scu_wdt_stop();
    ret = flash_part_get((char *)part_name, mask_flags, &part_hdr);
    if (ret < 0) {
        goto out;
    }

    LOGI(__func__, "Find partition '%s' at 0x%08x...\n", part_name, part_hdr.offset);
    flash_info.start_rw_addr = part_hdr.offset;
    
    recv_buf = malloc(part_hdr.size);
    if (recv_buf == NULL) {
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    recv_bytes = _usb_acm_load(recv_buf, part_hdr.size);
    if (recv_bytes <= 0) {
        ret = -1;
        goto out;
    }

    if (strncmp(part_name, "fw", 2) == 0) {
        is_fw = true;
    }

    LOGI(__func__, "Recv filename: %s\n", filename);
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

        tmp_buf = malloc(wr_size);
        if (tmp_buf == NULL) {
            ret = -1;
            goto out;
        }

        rd_size = recv_bytes;
        ret = gunzip(tmp_buf, wr_size, recv_buf, &rd_size);
        if (ret < 0) {
            LOGE(__func__, "Corrupted file: %s\n", filename);
            goto out;
        }
        configASSERT((int)rd_size < wr_size);

        wr_buf   = tmp_buf;
        wr_bytes = rd_size;

        // free recv_buf to save memory, otherwise malloc in ota_verify_flash_data might fail!
        free(recv_buf);
        recv_buf = NULL;
    }


    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    if (is_fw) {
        // NOTE: lock file system
        fs_get_lock();
        ret = ota_write_flash_fw(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            LOGE(__func__, "ota_write_flash_fw failed!\n");
            goto out;
        }
    }
    else {
        fs_get_lock();
        ret = ota_write_flash_raw_data(&flash_info, wr_buf, wr_bytes);
        fs_release_lock();
        if (ret < 0) {
            LOGE(__func__, "ota_write_flash_raw_data failed!\n");
            goto out;
        }
    }

    flash_info.stream_attr = FLASH_STREAM_RD;

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    uint32_t crc = crc32(0, (uint8_t *)wr_buf, wr_bytes);
    memset(wr_buf, 0, wr_bytes);
    fs_get_lock();
    LOGI(__func__, "Verify start: ...\r\n"); 
    ret = ota_verify_flash_data_crc32(&flash_info, wr_buf, wr_bytes, crc);
    if (ret < 0) {
        LOGI(__func__, "Verify end: FAIL!\r\n");
    }
    else {
        LOGI(__func__, "Verify end: SUCCESS!\r\n");
    }
    fs_release_lock();


out:
    scu_wdt_feed();
    scu_wdt_start();

    if (recv_buf != NULL) {
        free(recv_buf);
    }

    if (tmp_buf != NULL) {
        free(tmp_buf);
    }

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

#define PACK_OTA_MARKER 0x61766961
#define PACK_OTA_PART_NAME_LEN (32)
#define PACK_OTA_FILE_NAME_LEN (64)

#define PACK_OTA_DATA_ATTR_GZIP (1UL << 0)
#define PACK_OTA_DATA_ATTR_FW_IMG (1UL << 1)
#define PACK_OTA_DATA_ATTR_RAW_BIN (1UL << 2)
#define PACK_OTA_DATA_ATTR_NORMAL_FILE (1UL << 3)

// ota header defines
typedef struct ota_item
{
    uint32_t data_attr;
    uint32_t data_offset;
    uint32_t data_size;
    uint8_t part_name[PACK_OTA_PART_NAME_LEN];
    uint8_t file_name[PACK_OTA_FILE_NAME_LEN];
    uint8_t md5sum[16]; // md5sum from data_offset to data_offset+data_size
    uint32_t uncompressed_size;
} __attribute__((packed, aligned(4))) ota_item_t;

typedef struct pack_ota_hdr
{
    uint32_t magic;
    uint32_t checksum; // checksum of all ota_items
    uint32_t ota_item_num;
    ota_item_t ota_item[0];
} __attribute__((packed, aligned(4))) pack_ota_hdr_t;

static uint32_t get_xor_checksum(uint32_t *buffer, int length)
{
    int i;
    uint32_t xor_sum;

    xor_sum = 0;
    length /= 4;
    for (i = 0; i < length; i++)
    {
        xor_sum ^= buffer[i];
    }
    return xor_sum;
}

uint8_t* md5_hash2buf(const uint8_t* data, uint32_t size, uint8_t out_buf[16])
{
    Md5 md5;
    wc_InitMd5(&md5);
    wc_Md5Update(&md5, (const uint8_t*)data, size);
    wc_Md5Final(&md5, out_buf);

    return out_buf;
}

static int load_part(const char *part_name, uint8_t *data_buffer, uint32_t data_size)
{
    int ret = 0;
    flash_stream_info_t flash_info;
    flash_part_hdr_t part_hdr;
    uint32_t mask_flags;

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
    
    LOGI(__func__, "Flash type: %s", flash_info.flash_type == FLASH_NOR ? "NOR" : "NAND");
    ret = flash_part_get((char *)part_name, mask_flags, &part_hdr);
    if (ret < 0) {
        LOGE(__func__, "get part %s failed", part_name);
        goto out;
    }

    if (data_size > part_hdr.size) {
        ret = -1;
        LOGE(__func__, "exceed partition size: %u > %u", data_size, part_hdr.size);
        goto out;
    }

    LOGI(__func__, "Find partition '%s' at 0x%08x...", part_name, part_hdr.offset);
    flash_info.start_rw_addr = part_hdr.offset;

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    bool is_fw = strncmp(part_name, "fw", 2) == 0;
    if (is_fw) {
        // NOTE: lock file system
        fs_get_lock();
        ret = ota_write_flash_fw(&flash_info, data_buffer, data_size);
        fs_release_lock();
        if (ret < 0) {
            LOGE(__func__, "ota_write_flash_fw failed!\n");
            goto out;
        }
    }
    else {
        fs_get_lock();
        ret = ota_write_flash_raw_data(&flash_info, data_buffer, data_size);
        fs_release_lock();
        if (ret < 0) {
            LOGE(__func__, "ota_write_flash_raw_data failed!\n");
            goto out;
        }
    }

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    uint32_t crc = crc32(0, (uint8_t *)data_buffer, data_size);
    memset(data_buffer, 0, data_size);
    fs_get_lock();
    LOGI(__func__, "Verify start: ...\r\n"); 
    flash_info.stream_attr = FLASH_STREAM_RD;
    ret = ota_verify_flash_data_crc32(&flash_info, data_buffer, data_size, crc);
    if (ret < 0) {
        LOGI(__func__, "Verify end: FAIL!\r\n");
    }
    else {
        LOGI(__func__, "Verify end, ok!\r\n");
    }
    fs_release_lock();

out:
    // report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

static int load_fs(const char *filename, uint8_t *data_buffer, uint32_t data_size)
{
    int ret = -1;
    char *read_buf = NULL;
    const char *filename_strip = NULL;

    // strip path and get the bare filename
    filename_strip = strrchr(filename, '/');
    if (filename_strip != NULL) {
        filename_strip++;
    }
    else {
        filename_strip = filename;
    }

    fs_file_t *fp = NULL;

    if (get_curr_fs() == NULL) {
        LOGI(__func__, "fs hasn't been mounted!\n");     
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);

    // write to fs
    fp = fs_file_open(filename_strip, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
    if (fp == NULL) {
        LOGI(__func__, "Open file %s error!\n", filename);
        ret = -1;
        goto out;
    }
    ret = fs_file_write(fp, data_buffer, data_size);
    if (ret != (int)data_size) {
        LOGI(__func__, "Write error! %d bytes has been written, %d expected!\n", ret, (int)data_size);
        ret = -1;
        goto out;
    }
    LOGI(__func__, "Write %d bytes into file %s\n", data_size, filename);

    // Verify
    LOGI(__func__, "Verify start: ...\n"); 

    report_current_state(CMDS_LOAD_STATE_VERIFY);

    fs_file_seek(fp, 0, FS_SEEK_SET);
    read_buf = malloc(data_size);

    ret = fs_file_read(fp, read_buf, data_size);
    if (ret != (int)data_size) {
        LOGI(__func__, "Read error! %d bytes has been written, %d expected!\n", ret, (int)data_size);
        ret = -1;
        goto out;
    }
 
    if (memcmp(read_buf, data_buffer, data_size) == 0) {
        LOGI(__func__, "Verify end: ok!\n");
        ret = 0;
    }
    else {
        LOGI(__func__, "Verify end: FAIL!\n");
        ret = -1;
    }
    
out:
    if (fp != NULL) {
        fs_file_close(fp);
    }

    if (read_buf) {
        free(read_buf);
    }

    // report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

static int ota(uint8_t *ota_buffer, int ota_buffer_size)
{
    pack_ota_hdr_t *hdr = (pack_ota_hdr_t*)ota_buffer;
    if (hdr->magic != PACK_OTA_MARKER)
    {
        LOGE(__func__, "magic number mismatch, %u != %u", hdr->magic, PACK_OTA_MARKER);
        return -1;
    }

    int ota_item_num = hdr->ota_item_num;
    if (ota_buffer_size < (int)(ota_item_num * sizeof(ota_item_t) + sizeof(pack_ota_hdr_t)))
    {
        LOGE(__func__, "invalid ota file size %d with %d ota items!", ota_buffer_size, ota_item_num);
        return -1;
    }

    ota_item_t *ota_items = (ota_item_t*)(ota_buffer + sizeof(pack_ota_hdr_t));
    // verify checksum
    uint32_t checksum = get_xor_checksum((uint32_t*)ota_items, sizeof(ota_item_t) * ota_item_num);
    if (checksum != hdr->checksum)
    {
        LOGE(__func__, "checksum error: 0x%x != 0x%x", checksum, hdr->checksum);
        return -1;
    }

    int ret = 0;
    uint8_t *unzip_buffer = NULL;
    uint32_t unzip_filesize = 0;
    for (int i = 0; i < ota_item_num; i++)
    {
        ota_item_t *ota_item = ota_items + i;
        const char *filename = (const char*)ota_item->file_name;
        const char *part_name = (const char*)ota_item->part_name;
        //
        uint8_t md5[16] = {0};
        md5_hash2buf(ota_buffer + ota_item->data_offset, ota_item->data_size, md5);
        if (memcmp(md5, ota_item->md5sum, sizeof(md5)) == 0)
        {
            uint8_t *data_buffer = ota_buffer + ota_item->data_offset;
            uint32_t data_size = ota_item->data_size;
            if (is_compressed_file(filename))
            {
                uint32_t unzip_buffer_size = data_size * 3;
                if (unzip_buffer_size < 2 * 1024 * 1024)
                {
                    unzip_buffer_size = 2 * 1024 * 1024;
                }
                unzip_buffer = malloc(unzip_buffer_size);
                if (unzip_buffer == NULL)
                {
                    LOGE(__func__, "No spare memory");
                    break;
                }
                unzip_filesize = data_size;
                int ret = gunzip(unzip_buffer, unzip_buffer_size, data_buffer, &unzip_filesize);
                if (ret < 0 || unzip_filesize > unzip_buffer_size)
                {
                    LOGE(__func__, "Corrupted zip file");
                    break;
                }
                //
                data_buffer = unzip_buffer;
                data_size = unzip_filesize;
            }
            //
            if (strncmp(part_name, "fs", 2) == 0)
            {
                ret = load_fs(filename, data_buffer, data_size);
            }
            else
            {
                ret = load_part(part_name, data_buffer, data_size);
            }
            if (ret != 0)
            {
                LOGE(__func__, "write %s to device falied", filename);
                break;
            }
        }
        else
        {
            LOGE(__func__, "md5 check failed for %s", filename);
        }
    }

    if (unzip_buffer)
        free(unzip_buffer);

    return ret;
}

int usb_acm_load_ota(int ota_size)
{
    int ret = 0;

    scu_wdt_stop();

    uint8_t *recv_buf = (uint8_t*)malloc(ota_size);
    if (recv_buf == NULL)
    {
        LOGE(__func__, "no spare memory for size:%u", ota_size);
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_TRANSFER);

    int recv_bytes = _usb_acm_load(recv_buf, ota_size);
    if(recv_bytes != ota_size) {
        LOGI(__func__, "read file error, file too long or too short.\n");
        ret = -1;
        goto out;
    }

    report_current_state(CMDS_LOAD_STATE_SYNC_TO_DEVICE);
    ret = ota(recv_buf, ota_size);

    if (ret == 0) {
        LOGI(__func__, "Ota end: SUCCESS!\n");
    }
    else {
        LOGI(__func__, "Ota end: FAIL!\n");
    } 
out:
    if (recv_buf)
        free(recv_buf);

    scu_wdt_feed();
    scu_wdt_start();

    report_current_state(ret < 0 ? CMDS_LOAD_STATE_FAILED : CMDS_LOAD_STATE_SUCCESS);

    return ret;
}

int usb_acm_load(int argc, char *argv[])
{
    if (argc == 1) {
        cmds_usb_acm_load_help();
        return -1;
    }

    int shell_origin_priority = os_shell_app_priority_get();
    int shell_cur_priority = (shell_origin_priority + 4) > configMAX_PRIORITIES ? 
                                    configMAX_PRIORITIES : (shell_origin_priority + 4);
    os_shell_app_priority_set(shell_cur_priority);

    if (strncmp(argv[1], "addr", 4) == 0) {
        // offset, target
        if (argc == 4) {
            uint32_t offset;
            offset = strtoul(argv[2], NULL, 0);
            usb_acm_load_any(offset, argv[3]);
        }
        else {
            cmds_usb_acm_load_help();
        }

    }
    else if (strncmp(argv[1], "part", 4) == 0) {
        if (argc == 4) {
            usb_acm_load_part(argv[2], argv[3]);
        }
        else {
            cmds_usb_acm_load_help();
        }
    }
    else if (strncmp(argv[1], "file", 4) == 0) {
        usb_acm_load_file(argv[2]);
    }
    else if (strncmp(argv[1], "ota", 3) == 0) {
        if (argc == 3) {
            usb_acm_load_ota(strtoul(argv[2], NULL, 0));
        } else {
            cmds_usb_acm_load_help();
        }
    }
    else {
        cmds_usb_acm_load_help();
    }

    os_shell_app_priority_set(shell_origin_priority);

    return 0;
}

/* _attr, _name, _func, _desc */
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
usb_acm_load, usb_acm_load, Load binary by usb acm);
