#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "timer_example";
static pthread_t tid1;

extern uint32_t appGetRuntimeCounterValue(void);

static void timer_test()
{
    LOGI(__func__, "start...");
    uint32_t counter;

    while(1)
    {
        counter = appGetRuntimeCounterValue();
        LOGI(__func__, "%d", counter);
        usleep(1000*1000);
    }

    LOGI(__func__, "PASS!!!");
}

static void* thread1_entry(void* parameter)
{
    (void)parameter;
    timer_test();
    
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

    LOGI(TAG, "AIVA TIMER EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
