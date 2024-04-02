#include "i2s.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "i2s_example";
static pthread_t tid1;


static int i2s_test(void)
{
    return 0;
}

static void* thread1_entry(void* parameter)
{
    (void)parameter;

    i2s_test();
    
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

    LOGI(TAG, "AIVA I2S EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

