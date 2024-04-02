#include "drivers/i2c/i2c.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "i2c_example";
static pthread_t tid1;

static int i2c_irq_IRQn_callback(void * ctx)
{
    i2c_irq_context_t* context = (i2c_irq_context_t*) ctx;
    i2c_device_number_t i2c_num = context->i2c_num;
    LOGD(__func__, "i2c number %d", i2c_num);

    return 0;
}

static int i2c_iter_test(void)
{
    int i2c_num;
    uint8_t reg_addr = 0x12;
#if 1
    for (i2c_num = I2C_DEVICE_0; i2c_num < I2C_DEVICE_5; i2c_num++) {
        LOGD(__func__, "i2c_num is %d", i2c_num);
        i2c_init(i2c_num, 0x78 >> 1, 7, 50*1000);
        i2c_send_data(i2c_num, 0x78 >> 1, &reg_addr, 1);
    }
#endif
    /*sysctl_set_io_switch(IO_SWITCH_SPIM_CS1, 1);*/
    /*sysctl_set_io_switch(IO_SWITCH_SPIM_CS2, 1);*/

    return 0;
}

static int i2c_irq_send_test(void)
{
    int i2c_num = 2;
    uint8_t reg_addr[100];
    for (int i = 0; i < 100; i++)
        reg_addr[i] = 0x12;

    i2c_init(i2c_num, 0x78 >> 1, 7, 50*1000);    
    i2c_irq_init(i2c_num, i2c_irq_IRQn_callback, NULL, 18<<3);

    while(1)
        i2c_send_data(i2c_num, 0x78 >> 1, reg_addr, 100);

    return 0;
}

#if 0
static int i2c_irq_recv_test(void)
{
    int i2c_num = 2;
    uint8_t reg_addr[100];
    uint8_t recv_reg_addr[100];    

    for (int i = 0; i < 100; i++){
        reg_addr[i] = 0x12;
	}

    i2c_init(i2c_num, 0x78 >> 1, 7, 50*1000);
    i2c_irq_init(i2c_num, i2c_irq_IRQn_callback, NULL, 18<<3);

    while(1)
    {
        i2c_recv_data(i2c_num, 0x78 >> 1, reg_addr, 100, recv_reg_addr, 100);
    }

    return 0;
}
#endif

static void* thread1_entry(void* parameter)
{
    (void)parameter;

    i2c_iter_test();
    i2c_irq_send_test();
    
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

    LOGI(TAG, "AIVA I2C EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

