#ifndef _SDCARD_LFS_PORT_H_
#define _SDCARD_LFS_PORT_H_

#include "sdcard.h"
#include "lfs.h"
#include "fs.h"


int sd_read_wrapper(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size);

int sd_prog_wrapper(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size);

int sd_erase_wrapper(const struct lfs_config *c, lfs_block_t block);

int sd_sync_wrapper(const struct lfs_config *c);


#endif
