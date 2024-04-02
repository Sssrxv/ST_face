#ifndef __FS_H__
#define __FS_H__

#include <stdint.h>
#include "flash.h"

#ifdef USE_RTOS
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/sys/types.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif // USE_RTOS


#ifdef __cplusplus
extern "C"
{
#endif

// file system coniguration
typedef struct _fs_cfg_t {
    flash_type_t flash_type;
    int          spi_num;
    int          spi_chip_sel;
    // int          baudr;
    uint32_t     clk_rate;
    int          start_offset; // reserved space
} fs_cfg_t;

// file system definition
typedef struct _fs_t {
    fs_cfg_t cfg;
    void    *fs_ptr;
} fs_t;

// mount the file system
int fs_mount(fs_cfg_t *cfg);

// format the file system
int fs_format(void);

// unmout the file system
int fs_unmount(void);

// get the current occupied size in bytes
int fs_size(void);

// get the current capacity in bytes
int fs_capacity(void);

// get the file system pointer
fs_t *get_curr_fs(void);

// lock file system
int fs_get_lock();

// unlock file system
int fs_release_lock();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __FS_H__k
