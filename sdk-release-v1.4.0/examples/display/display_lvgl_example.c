#include "display.h"
#include "dvpout_lcd_driver.h"
#include "os_shell_app.h"
#include "os_init_app.h"
#include "frame_mgr.h"
#include "syslog.h"
#include "indev.h"

#include <string.h>
#include "os_startup.h"
#include "system_AI3100.h"
#include "aiva_sleep.h"

#include "lv_port_disp.h"
#include "lvgl_case/benchmark/lv_demo_benchmark.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "./lvgl_case/lvgl_case.h"

static const char* TAG = "display_lvgl_example";
static pthread_t tid1;

static const char *dev_name = "i80_st7365p";
int dvpout_lcd_lgvl_example(void);

static void* thread1_entry(void* parameter)
{
    dvpout_lcd_lgvl_example();
    return NULL;
}

static int my_app_entry(void)
{
    int result;

    /* Mount file system */
    os_mount_fs();

    /* Start shell */
    os_shell_app_entry();

    result = pthread_create(&tid1, NULL, thread1_entry, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid1 fail!");
    }

    return result;
}

int main(int argc, char *argv[])
{
    LOGI(TAG, "AIVA DISPLAY LVGL EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}


int dvpout_lcd_lgvl_example(void)
{
    // init display device.
    display_handle_t handle = display_init(DISPLAY_INTERFACE_TYPE_I80, dev_name);

    // init lvgl with display dev.
    lv_port_and_display_init(handle);
    LOGI(__func__, "LVGL init done......");

    // font test case -- OK
    // lv_label_font_test_case();

    // benchmark test -- OK
    // lv_demo_benchmark();

    // btn matrix test case -- OK.
    btn_matrix_test_case();

    while (1) {
        lv_tick_inc(1);
        lv_task_handler();
        aiva_msleep(1);
    }

    return 0;
}
