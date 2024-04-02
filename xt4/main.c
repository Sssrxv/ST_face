#include "sys/syslog.h"
#include "os_startup.h"
#include "os_shell_app.h"
#include "os_init_app.h"
#include "fs.h"
#include "spi.h"
#include "flash_part.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"

#include "facelock_sdk.h"
#include "facelock_sdk_test.h"

static pthread_t tid1;

static void* test_facelock_entry(void* parameter)
{
    (void)parameter;

    facelock_sdk_uart_mode_test_entry();

    return NULL;
}

int xt4_app_entry(void)
{
    int result;
 
    /* Mount file system */
    os_mount_fs();

    /* Start shell */
    os_shell_app_entry();

    result = pthread_create(&tid1, NULL, test_facelock_entry, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid1 fail!");
    }

    return result;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    /* Register custom entry */
    os_app_set_custom_entry(xt4_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
