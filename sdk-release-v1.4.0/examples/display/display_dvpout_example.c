#include "display.h"
#include "dvpout_lcd_driver.h"

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

static const char* TAG = "display_dvpout_example";

static pthread_t tid1;

static const char *dev_name = "dvpout_ili9806e";

int dvpout_lcd_continous_test(void);

static void* thread1_entry(void* parameter)
{
    dvpout_lcd_continous_test();
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
    LOGI(TAG, "AIVA DISPLAY DVPOUT EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

int dvpout_lcd_continous_test(void) 
{
    int ret = 0;
    frame_buf_t *cur0 = NULL;

    display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_DVPO, dev_name);
    if (handle == NULL) {
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
    if (fmt == DVPO_FMT_RGBA8888)
        fparam.fmt = FRAME_FMT_RGBA8888;
    else
        configASSERT(!"Unknown format!");

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
