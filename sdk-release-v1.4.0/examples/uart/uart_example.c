#include "uart/uart.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

#include "uart_stream_buffer.h"

static const char* TAG = "uart_example";
static pthread_t tid1;
static pthread_t tid2;
static uart_handle_t handle;

static void uart_test()
{
    int i;
    char send_buf[16];
    char recv_buf[16];

    uart_init(UART_DEVICE_0);
    uart_config(UART_DEVICE_0, 460800, UART_BITWIDTH_8BIT, UART_STOP_1, UART_PARITY_NONE);

    while(1) {
        usleep(100*1000);

        uart_send_data(UART_DEVICE_0, &send_buf[0], 16);
        uart_receive_data(UART_DEVICE_0, &recv_buf[0], 16);

        LOGD(TAG, "uart recv data:\n");
        for (i=0; i<16; i++) {
            LOGD(TAG, "recv_buf[%d]=0x%02x\n", i, recv_buf[i]);
        }
    }
}

static void* thread1_entry(void* parameter)
{
    (void)parameter;

    uart_test();
    
    return NULL;
}

static void* uart_async_test(void* parameter)
{
    (void)parameter;

    // int count = 0;

    while(1) {
        usleep(100*1000);
        char data[64] = {0};

        int len = uart_read_async(&handle, data, 64);
        // loopback
        if (len > 0) {
            uart_write_async(&handle, data, len);
        }
        // LOGD(TAG, "%s, %s count %d\n", __func__, data, count++);
        // uart_write_async(&handle, "hello\n", 7);
    }

    return NULL;
}

static int my_app_entry(void)
{
    int result;

    result = pthread_create(&tid1, NULL, thread1_entry, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid1 fail!");
    }

    uart_async_open(&handle, UART_DEVICE_1);

    result = pthread_create(&tid2, NULL, uart_async_test, NULL);
    if (result < 0) {
        LOGE(__func__, "Create tid2 fail!");
    }

    return result;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    LOGI(TAG, "AIVA UART EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}
