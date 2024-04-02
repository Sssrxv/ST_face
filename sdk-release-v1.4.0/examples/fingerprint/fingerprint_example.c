#include "fpc1150.h"
#include "syslog.h"
#include "os_startup.h"
#include <stdint.h>
#include <stdlib.h>


static int fingerprint_app_entry()
{
    int ret = 0;
    uint8_t *fingerprint_buffer = NULL;
    int fingerprint_width = 0;
    int fingerprint_height = 0;
    const int test_cnt = 20;

    ret = fpc1150_init();
    if (ret != 0)
    {
        LOGE(__func__, "init fpc1150 failed");
        return -1;
    }
    //
    fpc1150_get_img_resolution(&fingerprint_width, &fingerprint_height);

    //
    int fingerprint_img_size = fingerprint_width*fingerprint_height;
    fingerprint_buffer = (uint8_t*)calloc(1, fingerprint_img_size);
    for (int i = 0; i < test_cnt; i++)
    {
        do
        {
            ret = fpc1150_read_fingerprint(fingerprint_buffer, fingerprint_img_size);
            if (ret == 0)
                LOGI(__func__, "fingerprint found");
        } while (ret != 0);
        // do something with fingerprint....
    }
    //
    ret = fpc1150_sleep();
    if (ret)
    {
        LOGI(__func__, "sleep failed");
    }

    if (fingerprint_buffer != NULL)
    {
        free(fingerprint_buffer);
        fingerprint_buffer = NULL;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    LOGI("", "AIVA FINGERPRINT EXAMPLE");

    /* Register custom entry */
    os_app_set_custom_entry(fingerprint_app_entry);

    /* Start scheduler, dead loop */
    os_app_main_entry();

    return 0;
}