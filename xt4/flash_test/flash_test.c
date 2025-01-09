#include "aiva_malloc.h"
#include "nor_flash.h"
#include "sysctl.h"
#include "syslog.h"
#include "aiva_sleep.h"
#include "spi.h"
#include "aiva_time_profile.h"
#include "aiva_pmu.h"
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN         (256)
#define SPI_TEST_DEV    (SPI_DEVICE_0)
#define SPI_TEST_CS     (SPI_CHIP_SELECT_0)
#define SPL_LEN         (256 * 1024)

static int test_case_nor_flash_full_chip_test(uint32_t spi_rate_div)
{
    int ret = 0;
    uint32_t buf_len;
    uint8_t *rd_buf;
    uint8_t *wr_buf;
    uint32_t i;
    uint32_t seed;
    nor_flash_param_t param;
    uint32_t pre_cycle;
    float speed;
    uint32_t elapse_cost= 0;
    long time_counter = 0;
    uint32_t sector;
    uint32_t addr = 4 * 1024 * 1024; // flash test start addr
    // float cpu_freq = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
    uint32_t sys_clk = sysctl_clock_get_freq(SYSCTL_CLOCK_SPI0);

    ret = nor_flash_init(SPI_TEST_DEV, SPI_TEST_CS, sys_clk / spi_rate_div, FLASH_QUAD_EN);
    if (0 != ret) { ret = 1; goto END; }

    ret = nor_flash_get_param(&param);
    if (0 != ret) { ret = 2; goto END; }
    LOGI(__func__, "flash param name:%s, chip_size:%dMB spi clk:%dMHz.", param.dev_name, \
                            param.chip_size/1024/1024, sys_clk / spi_rate_div/ 1000/ 1000);
    /*buf_len = param.sector_size;*/
    buf_len = 256 * 1024;

    rd_buf = aiva_malloc(buf_len);
    if (NULL == rd_buf) { ret = 3; goto END; }

    wr_buf = aiva_malloc(buf_len);
    if (NULL == wr_buf) { ret = 4; goto END; }

    seed = read_cycle();

    for (i = 0; i < buf_len; i++) {
        wr_buf[i] = (seed + rand()) & 0xff;
        // wr_buf[i] = (i) & 0xff;
    }

    LOGI("test flash", "start addr: %dMB, stop addr: %dMB.", addr/1024/1024, param.chip_size/1024/1024);

    for (; addr < param.chip_size; addr += buf_len) {
        aiva_time_elapsed_ms(&time_counter, NULL);
        ret = nor_flash_write(addr, wr_buf, buf_len);
        elapse_cost = aiva_time_elapsed_ms(&time_counter, "write data over, elapse");
        speed = 1000. * buf_len / 1024. / 1024. / elapse_cost;
        LOGI(__func__, "write %dKB, time %d ms, speed %f MB/s",
                buf_len/1024, elapse_cost, speed);
        if (0 != ret) {
            LOGE("", "nor_flash write data failed.");
            ret = -1;
            goto END;
        }

        /*memset(rd_buf, 0, buf_len);*/
        aiva_time_elapsed_ms(&time_counter, NULL);
        ret = nor_flash_read(addr, rd_buf, buf_len);
        elapse_cost = aiva_time_elapsed_ms(&time_counter, "read data over, elapse");
        speed = 1000. * buf_len / 1024. / 1024. / elapse_cost;
        LOGI(__func__, "read %dKB, time %d ms, speed %f MB/s",
                buf_len/1024, elapse_cost, speed);

        for (i = 0; i < buf_len; i++) {
            if (rd_buf[i] != wr_buf[i]) { ret = -2; goto END; }
        }

        sector = addr / buf_len;
        if ((sector + 1) % 4 == 0) {
            LOGI(__func__, "sector (%d ~ %d) check pass!", sector-3, sector);
        }
    }

    ret = 0;
END:
    if (rd_buf) { aiva_free(rd_buf); }
    if (wr_buf) { aiva_free(wr_buf); }
    if(ret == 0) {
        LOGI("nor_flash", "test flash sys_clk/%u, clk:%dMHz, PASS", spi_rate_div, sys_clk / spi_rate_div/ 1000/ 1000);
    }
    else {
        LOGE("nor_flash", "test flash sys_clk/%u, clk:%d, FAIL", spi_rate_div, sys_clk / spi_rate_div/ 1000/ 1000);
    }
    return ret;
}



void flash_test_entry()
{
    int ret = 0;

    LOGI("nor_flash", "test only keep fw0 fw1 part.");

    uint8_t spi_rate_div[] = {2, 4, 8, 16};
    for(int i = 0; i < sizeof(spi_rate_div)/ sizeof(spi_rate_div[0]); i++)
    {
        test_case_nor_flash_full_chip_test(spi_rate_div[i]);
    }

    while(1)
    {
        LOGI("nor_flash", "test done");
        sleep(1);
    }
}


