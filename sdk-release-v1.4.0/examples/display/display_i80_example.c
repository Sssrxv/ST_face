#include "display.h"
#include "i80_lcd_driver.h"
#include "videoin.h"
#include "rsz.h"

#include "frame_mgr.h"
#include "syslog.h"
#include <string.h>
#include "os_startup.h"
#include "system_AI3100.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"

#define ALIGN_SIZE(x, n) ((x + n - 1) & (-n))
#define ALIGN_STRIDE(x) ALIGN_SIZE(x, 32)

static const char* TAG = "display_i80_example";

static pthread_t tid1;

static const char *dev_name = "i80_alientek28";

static int i80_lcd_continous_test(void);

static int i80_lcd_camera_test(void);

static void* thread1_entry(void* parameter)
{
    (void)parameter;
#if 0
    i80_lcd_continous_test();
#else
    i80_lcd_camera_test();
#endif
    return NULL;
}

static int my_app_entry(void)
{
    int result;

    result = pthread_create(&tid1, NULL, thread1_entry, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid1 fail!");
    }

    return result;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    LOGI(TAG, "AIVA DISPLAY I80 EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

static int i80_lcd_continous_test(void) 
{
    int ret = 0;
    frame_buf_t *cur0 = NULL;

    display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_I80, dev_name);
    if (handle == NULL) {
        LOGE(__func__, "handle is NULL");
        return -1;
    }

    display_frame_param_t param;
    ret = display_get_frame_param(handle, &param);
    CHECK_RET(ret);

    int w = param.w;
    int h = param.h;
    LOGI(__func__, "w is %d, h is %d", w, h);

    int fmt = param.format;
    LOGI(__func__, "fmt is %d", fmt);

    frame_param_t fparam = FRAME_PARAM_INITIALIZER;
    fparam.width  = w;
    fparam.height = h;
    fparam.stereo_mode = FRAME_STEREO_NONE;
    if (fmt == I80_OUT_FMT_16BIT_RGB565) {
        //fparam.fmt = FRAME_FMT_RGB565;
        fparam.fmt = FRAME_FMT_RGBA8888;
    } else {
        configASSERT(!"Unknown format!");
    }

    ret = display_start(handle);
    CHECK_RET(ret);

    int id = 0;

    while (1) {
        cur0 = frame_mgr_get_one(&fparam);
        //LOGI(__func__, "stride %d", cur0->stride[0]);
        configASSERT(cur0 != NULL);

        uint32_t *data;

        if (id % 3 == 0) {
            for (int j = 0; j < h; j++) {
                data = (uint32_t *)(cur0->data[0] + cur0->stride[0]*j);
                for (int i = 0; i < w; i++) {
                    data[i] = 0x000000FF; 
                }
            }
        }
        else if (id % 3 == 1) {
            for (int j = 0; j < h; j++) {
                data = (uint32_t *)(cur0->data[0] + cur0->stride[0]*j);
                for (int i = 0; i < w; i++) {
                    data[i] = 0x00FF0000; 
                }
            }
        }
        else if (id % 3 == 2) {
            for (int j = 0; j < h; j++) {
                data = (uint32_t *)(cur0->data[0] + cur0->stride[0]*j);
                for (int i = 0; i < w; i++) {
                    data[i] = 0x0000FF00; 
                }
            }
        } 

        dcache_flush((void *)cur0->data[0], cur0->stride[0] * h);

        display_send_frame(handle, cur0);
            
    	id++;  

        sleep(1);
    }

    display_stop(handle);
    display_release(handle);

    return ret;
}


static int i80_lcd_camera_test(void)
{
    int ret = 0;
    const char *sens_type = "ov02b";
    cis_dev_driver_t *find_drv[3];
    videoin_id_t id = VIDEOIN_ID_MIPI2;
    cis_frame_param_t frame_param;
    cis_interface_param_t param_inf;
    videoin_frame_t videoin_frame;

    ret = cis_find_dev_driver_list(sens_type, &find_drv[0], 3);
    if (ret < 0) {
        LOGE(__func__, "Can't find sensor '%s' ...", sens_type);
        goto out;
    }

    find_drv[id]->get_frame_parameter(find_drv[id],&frame_param);
    find_drv[id]->get_interface_param(find_drv[id], &param_inf);
    ret = videoin_register_cis_driver(id, find_drv[id]);
        if (ret < 0) {
        LOGE(__func__, "register sensor driver failed !");
        goto out;
    }

    videoin_config_t config;
    memset(&config, 0, sizeof(videoin_config_t));
    config.camera_num = 1;
    config.mode = VIDEOIN_MODE_CONTINUE;
    config.stereo_mode = FRAME_STEREO_NONE;
    config.skip_power_on = 0;
    config.capture_config[0].videoin_id = id;
    config.capture_config[0].hoffset = 0;
    config.capture_config[0].voffset = 0;
    config.capture_config[0].hsize = frame_param.width;
    config.capture_config[0].vsize = frame_param.height;
    config.capture_config[0].channels = param_inf.mipi_param.vc_num;
    config.capture_config[0].fmt = FRAME_FMT_RAW8;
    config.capture_config[0].hstride = ALIGN_STRIDE(frame_param.width);
    videoin_context_t *context = videoin_init(&config);
    if (context == NULL) {
        LOGE(__func__, "videoin init failed!");
        goto out;
    }
    ret = videoin_start_stream(context);
    if (ret != 0) {
        LOGE(__func__, "videoin start stream failed!");
        goto out;
    }

    frame_param_t fparam  = FRAME_PARAM_INITIALIZER;
    fparam.fmt = FRAME_FMT_RGBA8888;
    fparam.width   = 240;
    fparam.height  = 320;
    fparam.stride[0] = 240*4;
    fparam.stride[1] = 0;
    fparam.stride[2] = 0;
    fparam.set_stride = 1;

    display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_I80, dev_name);
    if (handle == NULL) {
        LOGE(TAG, "display init fail\n");
        goto out;
    }

    ret = display_start(handle);
    if (ret != 0) {
        LOGE(TAG, "display start fail\n");
        goto out;
    }

    while(1)
    {
        ret = videoin_get_frame(context, &videoin_frame, VIDEOIN_WAIT_FOREVER);
        if(ret != 0)
        {
            LOGD(__func__, " videoin_get_frame: %d frame null:%d", ret, videoin_frame.frame_buf[0]==NULL);
            continue;
        }
        else
        {
            static int cnt = 0;
            LOGD(__func__, " videoin_get_frame: %d", cnt++);
        }
        frame_buf_t *frame_buf = videoin_frame.frame_buf[0];
        frame_buf_t *frame_rsz = rsz_resize(frame_buf, fparam.width, fparam.height, RSZ_GRAY2GRAY);
        frame_mgr_decr_ref(frame_buf);
        frame_buf = frame_rsz;
        frame_buf_t *frame_yuv422sp =  rsz_cvt_raw8_to_yuv422sp(frame_rsz);
        frame_mgr_decr_ref(frame_rsz);
        frame_buf_t * frame_send = rsz_cvt_yuv422sp_to_rgba(frame_yuv422sp);
        frame_mgr_decr_ref(frame_yuv422sp);
        frame_mgr_flush_cache(frame_send); 
        // TODO: Auto calculate stride size.
        frame_send->stride[0] = 240*4;

        ret = display_send_frame(handle, frame_send);
        if (ret != 0) {
            LOGE(TAG, "send frame to display fail\n");
        }
    }

out:
    if(context)
    {
        videoin_stop_stream(context);
        videoin_release(context);
    }
    if(handle)
    {
        display_stop(handle);
        display_release(handle);
    }

    return -1;
}
