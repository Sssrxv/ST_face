#include "drivers/gpio/gpio.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "gpio_example";
static pthread_t tid1;

static void test_output()
{
    int ret;
        
    ret = gpio_init();
    if (ret != 0) {
        LOGE(TAG, "gpio init fail\n");
        return ;
    }

    gpio_pin_value_t pin_value;
    for (int i = 0; i < 32; i++)
    {
        gpio_set_drive_mode(i, GPIO_DM_OUTPUT);
    }

    while(1) {
        for (int i = 0; i < 32; i++)
        {
            gpio_set_pin(i, GPIO_PV_LOW);
	    pin_value = gpio_get_pin(i);
            LOGD(TAG, "after set PV_LOW get gpio value:%d", pin_value);
        }
        LOGD(TAG, "\n\n");
        sleep(5);

        for (int i = 0; i < 32; i++)
        {
            gpio_set_pin(i, GPIO_PV_HIGH);

	    pin_value = gpio_get_pin(i);
            LOGD(TAG, "after set PV_HIGH get gpio value:%d", pin_value);
        }

        sleep(5);
    }
}

#if 0
static void test_input()
{
    int ret;

    ret = gpio_init();
    if (ret != 0) {
        LOGE(TAG, "gpio init fail\n");
        return ;
    }

    gpio_pin_value_t pin_value;
    for (int i = 0; i < 32; i++)
    {
        //gpio_set_drive_mode(i, GPIO_DM_INPUT);
	    //gpio_set_drive_mode(i, GPIO_DM_INPUT_PULL_UP);
	    gpio_set_drive_mode(i, GPIO_DM_INPUT_PULL_DOWN);
    }

    while(1) {
        for (int i = 0; i < 32; i++)
	    {
            pin_value = gpio_get_pin(i);
            LOGD(TAG, "get gpio %d: value:%d", i, pin_value);
        }
	    LOGD(TAG, "\n\n");
        sleep(5);
    }
}

static void test_input1()
{
    int gpio = 22; //30 22
    gpio_pin_value_t pin_value;
    int ret;

    ret = gpio_init();
    if (ret != 0) {
        LOGE(TAG, "gpio init fail\n");
        return ;
    }

    gpio_set_drive_mode(31, GPIO_DM_OUTPUT);
    gpio_set_pin(31, GPIO_PV_HIGH);
    sleep(5);


    gpio_set_drive_mode(gpio, GPIO_DM_INPUT);
    sleep(5);
    pin_value = gpio_get_pin(gpio);
    LOGD(TAG, "get gpio %d: value:%d", gpio, pin_value);
}
#endif

static void* thread1_entry(void* parameter)
{
    (void)parameter;
    test_output();
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

    LOGI(TAG, "AIVA GPIO EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
