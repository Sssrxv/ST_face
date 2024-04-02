#include "pwm/pwm.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "pwm_example";
static pthread_t tid1;

static int pwm_test(void)
{
    int pwm_dev;

    pwm_dev = PWM_DEVICE_2;

    pwm_init(pwm_dev);
    pwm_set_enable(pwm_dev, 0);
    pwm_set_frequency(pwm_dev, 6*1000*1000, 0.5);
    pwm_set_enable(pwm_dev, 1);

    return 0;
}

static void* thread1_entry(void* parameter)
{
    (void)parameter;

    pwm_test();
    
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

    LOGI(TAG, "AIVA PWM EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
