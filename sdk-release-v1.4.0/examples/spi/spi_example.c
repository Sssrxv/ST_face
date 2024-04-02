#include "drivers/spi/spi.h"
#include "sys/syslog.h"

#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/mqueue.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/errno.h"
#include "os_startup.h"

static const char* TAG = "spi_example";
static pthread_t tid1;

#if 0
static void spi_send_test()
{
    uint8_t cmd_buff[4];
    cmd_buff[0] = 87;
    cmd_buff[1] = 99;
    cmd_buff[2] = 32;
    cmd_buff[3] = 67;

    uint8_t tx_buff[6]; 
    tx_buff[0] = 57;
    tx_buff[1] = 29;
    tx_buff[2] = 52;
    tx_buff[3] = 97;
    tx_buff[4] = 39;
    tx_buff[5] = 32;

    spi_init(SPI_DEVICE_1, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8);

    while(1)
    {
        spi_send_data_standard(SPI_DEVICE_1, SPI_CHIP_SELECT_0, cmd_buff, 4, tx_buff, 6);
        usleep(100*1000);
    }

    return;    
}
#endif

static void spi_recv_test()
{
    uint8_t cmd_buff[4];
    cmd_buff[0] = 87;
    cmd_buff[1] = 54;
    cmd_buff[2] = 32;
    cmd_buff[3] = 27;

    uint8_t rx_buff[6]; 

    spi_init(SPI_DEVICE_1, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8);

    while(1)
    {
        spi_receive_data_standard(SPI_DEVICE_1, SPI_CHIP_SELECT_0, cmd_buff, 4, rx_buff, 6);
        usleep(100*1000);
    }

    return;    
}

static void* thread1_entry(void* parameter)
{
    (void)parameter;

    spi_recv_test();
    
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

    LOGI(TAG, "AIVA SPI EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(my_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}

