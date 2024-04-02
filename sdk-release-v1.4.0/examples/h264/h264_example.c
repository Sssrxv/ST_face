#include "h264_encoder.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "syslog.h"
#include "aiva_sleep.h"
#include "fs_file.h"
#include "aiva_malloc.h"
#include "flash_part.h"
#include "spi.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

#include "system_AI3100.h"
#include "os_init_app.h"



#define DEMO_PIC_W             (176)
#define DEMO_PIC_H             (144)
#define DEMO_PIC_TYPE          (FRAME_FMT_I420)
#define DEMO_PIC_NUM_IN_FILE   (3)
#define DEMO_PIC_FILE_NAME     "176x144.yuv"
#define DEMO_ENCODE_TIMES      (30)
#define DEMO_H264_FILE_NAME    "176x144.h264"


static const char* TAG = "h264_example";



#define DUMP_BUF_SIZE   (200 * 1024)
static uint8_t m_dump_buf[DUMP_BUF_SIZE];
static uint32_t m_dump_cnt = 0;

static int save_data_2_dump_buf(uint32_t buf, uint32_t size, int need_dcache_inval)
{
    if (need_dcache_inval) {
        dcache_inval(buf, size);
    }
    
    if (m_dump_cnt + size <= DUMP_BUF_SIZE) {
        memcpy(m_dump_buf + m_dump_cnt, buf, size);
        //dcache_flush(m_dump_buf + m_dump_cnt, size);
        m_dump_cnt += size;
        LOGD(__func__, "0 tmp_buf 0x%08X  size %u, dump 0x%08X  size %u", buf, size, m_dump_buf, m_dump_cnt);
    }
    else {
        LOGE(__func__, " size %u,  m_dump_cnt %u", size, m_dump_cnt);
        LOGE(__func__, "0 m_dump_cnt + size > DUMP_BUF_SIZE");
    }

    return 0;
}

static int mem_2_file(uint8_t buf[], uint32_t buf_size, const char *file)
{
    int ret = 0;
    if (!buf || !buf_size || !file) {
        return -1;
    }

    fs_file_t *fil = fs_file_open(file, LFS_O_CREAT | LFS_O_WRONLY);
    if (!fil) {
        return -2;
    }

    int rd_size = fs_file_write(fil, buf, buf_size);
    // dcache_flush(buf, buf_size);

out:
    if (fil) { fs_file_close(fil); }
    return ret;
}

static int file_2_mem(uint8_t buf[], uint32_t buf_size, const char *file)
{
    int ret = 0;
    if (!buf || !buf_size || !file) {
        return -1;
    }

    fs_file_t *fil = fs_file_open(file, FS_O_RDONLY);
    if (!fil) {
        return -2;
    }

    uint32_t fil_size = fs_file_size(fil);

    if (fil_size > buf_size) {
        ret = -3;
        goto out;
    }

    int rd_size = fs_file_read(fil, buf, fil_size);
    dcache_flush(buf, fil_size);

out:
    if (fil) { fs_file_close(fil); }
    return ret;
}

static uint32_t get_raw_picture_size(uint16_t w, uint16_t h, frame_fmt_t input_type)
{
    uint32_t raw_pic_size = 0;
    switch (input_type) {
        case FRAME_FMT_RGB565:  raw_pic_size = w * h * 2; break;
        case FRAME_FMT_I420:    raw_pic_size = w * h * 3 / 2; break;
        default: LOGE(__func__, "invalid pic type"); break;
    }

    return raw_pic_size;
}


static uint8_t *m_raw_pics_buf = NULL;

static int load_all_raw_pictures(void)
{
    if (m_raw_pics_buf) {
        return 0;
    }

    int ret = 0;
    const int      pic_type = (int)DEMO_PIC_TYPE;
    const int      pic_w = DEMO_PIC_W, pic_h = DEMO_PIC_H;
    const uint32_t pic_size = get_raw_picture_size(pic_w, pic_h, pic_type);
    const uint32_t pic_num = DEMO_PIC_NUM_IN_FILE;
    const uint32_t pic_file_size = pic_num * pic_size; //114048;

    ret = os_mount_fs();
    if (0 != ret) {
        LOGE(__func__, "os_mount_fs error, ret %d", ret);
        return -1;
    }
    LOGI(__func__, "mout fs successfully");

    if (NULL == m_raw_pics_buf) {
        m_raw_pics_buf = (uint8_t*)aiva_malloc(pic_file_size);
        if (!m_raw_pics_buf) {
            LOGE(__func__, "aiva_malloc error");
            return -2;
        }

        ret = file_2_mem(m_raw_pics_buf, pic_file_size, DEMO_PIC_FILE_NAME);
        if (0 != ret) {
            LOGE(__func__, "file_2_mem error %d", ret);
            aiva_free(m_raw_pics_buf);
            m_raw_pics_buf = NULL;
            return -3;
        }
    }

    return 0;
}

static void release_loaded_raw_pictures(void)
{
    if (m_raw_pics_buf) {
        aiva_free(m_raw_pics_buf);
        m_raw_pics_buf = NULL;
    }
}

static void* get_one_raw_picture(void)
{    
    static uint32_t pic_cnt = 0;

    if (!m_raw_pics_buf) {
        int ret = load_all_raw_pictures();
        if (0 != ret) {
            return NULL;
        }
    }

    const uint32_t pic_size = get_raw_picture_size(DEMO_PIC_W, DEMO_PIC_H, DEMO_PIC_TYPE);

    void *pic_buf = m_raw_pics_buf + (pic_cnt % DEMO_PIC_NUM_IN_FILE) * pic_size;
    ++pic_cnt;
    
    return pic_buf;
}

static int save_h264_2_file(void)
{
    return mem_2_file(m_dump_buf, m_dump_cnt, DEMO_H264_FILE_NAME);
}


static int h264_encoder_demo(void)
{
    LOGI(__func__, "called\r\n");


    // 0 prepare raw picture data
    int ret = load_all_raw_pictures();
    if (0 != ret) {
        LOGE(__func__, "load_all_raw_pictures error, %d", ret);
        return -2;
    }
    LOGI(__func__, "load_all_raw_pictures ok\r\n");


    // 1 config and init
    frame_buf_t *frame_out = NULL;
    uint32_t encode_times = DEMO_ENCODE_TIMES;
    h264_encoder_t h264_encoder = NULL;
    h264_cfg_t h264_cfg = H264_CFG_PARAM_INITIALIZER;
    h264_cfg.w = DEMO_PIC_W;
    h264_cfg.h = DEMO_PIC_H;
    h264_cfg.input_type = DEMO_PIC_TYPE;
    h264_cfg.gop = 3;
    h264_cfg.fps = 30;
    ret = h264_init(&h264_cfg, &h264_encoder);
    if (0 != ret) {
        LOGE(__func__, "h264_init error, %d", ret);
        return -2;
    }
    LOGI(__func__, "h264_init ok\r\n");


    // 2 start
    ret = h264_start(&h264_encoder, &frame_out);
    if (0 != ret) {
        LOGE(__func__, "h264_start error, %d", ret);
        return -3;
    }
    LOGI(__func__, "h264_start ok, h264_hdr %#X, h264_hdr_size %u\r\n", frame_out->data[0], frame_out->used_bytes);
    save_data_2_dump_buf(frame_out->data[0], frame_out->used_bytes, 0);
    frame_mgr_decr_ref(frame_out);


    // 3 encode
    void *input_pic_buf = NULL;
    frame_buf_t frame_in = {
        .width = DEMO_PIC_W,
        .height = DEMO_PIC_H,
        .fmt = DEMO_PIC_TYPE,
        .used_bytes = get_raw_picture_size(DEMO_PIC_W, DEMO_PIC_H, DEMO_PIC_TYPE),
        .is_continous = 1,
        .stride[0] = DEMO_PIC_W,
        .stride[1] = DEMO_PIC_W,
        .stride[2] = DEMO_PIC_W,
        .channels = 3,
    };
    for (uint32_t frame_cnt = 0; frame_cnt < encode_times; ++frame_cnt) {
        input_pic_buf = get_one_raw_picture();
        if (NULL == input_pic_buf) {
            LOGE(__func__, "get_one_raw_picture error, ret %d", ret);
            ret = -4;
            break;
        }

        frame_in.data[0] = input_pic_buf;                                     // Y
        frame_in.data[1] = input_pic_buf + DEMO_PIC_W * DEMO_PIC_H;           // U
        frame_in.data[2] = frame_in.data[1] + DEMO_PIC_W * DEMO_PIC_H / 4;    // V

        ret = h264_encode(&h264_encoder, &frame_in, &frame_out);
        if (0 != ret) {
            LOGE(__func__, "h264_encode ret error %d", ret);
            ret = -5;
            break;
        }

        LOGI(__func__, "%u outbuf 0x%#X size %u", frame_cnt, frame_out->data[0], frame_out->used_bytes);
        save_data_2_dump_buf(frame_out->data[0], frame_out->used_bytes, 0);
        frame_mgr_decr_ref(frame_out);
    }
    LOGI(__func__, "encode over\r\n");

    
    // 4 stop
    ret = h264_stop(&h264_encoder, &frame_out);
    if (0 != ret) {
        LOGE(__func__, "h264_stop error, %d", ret);
        return -6;
    }
    LOGI(__func__, "h264_stop ok, h264_tail %#X, tail_size %u\r\n", frame_out->data[0], frame_out->used_bytes);
    save_data_2_dump_buf(frame_out->data[0], frame_out->used_bytes, 0);
    frame_mgr_decr_ref(frame_out);


    // 5 release
    ret = h264_release(&h264_encoder);
    if (0 != ret) {
        LOGE(__func__, "h264_release error, %d", ret);
        return -7;
    }
    LOGI(__func__, "h264_release ok\r\n");


    // 6 release loaded raw pictures
    release_loaded_raw_pictures();


    // 7 save encoded h264 data to file
    ret = save_h264_2_file();
    if (0 != ret) {
        LOGE(__func__, "save_h264_2_file error, %d", ret);
        return -8;
    }
    LOGI(__func__, "save_h264_2_file ok\r\n");

    LOGI(__func__, "exit\r\n");

    return ret;
}

static void* thread1_entry(void* parameter)
{
    h264_encoder_demo();
    
    return NULL;
}

static int my_app_entry(void)
{
    int result;
    pthread_t tid1;

    result = pthread_create(&tid1, NULL, thread1_entry, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid1 fail!");
    }

    return result;
}

int main(int argc, char *argv[])
{
    LOGI(TAG, "AIVA H264 EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
