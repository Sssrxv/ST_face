#ifndef __FS_FILE_H__
#define __FS_FILE_H__

#include <stdint.h>
#include "fs.h"
#include "lfs.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PATH_LEN            256

// file descriptor definition
typedef struct _fs_file_t {
    fs_t            *fs;
    lfs_file_t      *file;
//#ifdef USE_RTOS
    //pthread_mutex_t  lock;
//#endif
} fs_file_t;

// direction descriptor definition
typedef struct _fs_dir_t {
    fs_t            *fs;
    lfs_dir_t      *dir;
//#ifdef USE_RTOS
    //pthread_mutex_t  lock;
//#endif
} fs_dir_t;

// flags for fs_file_open
#define FS_O_RDONLY         LFS_O_RDONLY    // Open a file as read only
#define FS_O_WRONLY         LFS_O_WRONLY    // Open a file as write only
#define FS_O_RDWR           LFS_O_RDWR      // Open a file as read and write
#define FS_O_CREAT          LFS_O_CREAT     // Create a file if it does not exist
#define FS_O_EXCL           LFS_O_EXCL      // Fail if a file already exists
#define FS_O_TRUNC          LFS_O_TRUNC     // Truncate the existing file to zero size
#define FS_O_APPEND         LFS_O_APPEND    // MOve to end of file on every write

// File seek flags
#define FS_SEEK_SET         LFS_SEEK_SET
#define FS_SEEK_CUR         LFS_SEEK_CUR
#define FS_SEEK_END         LFS_SEEK_END

fs_file_t * fs_file_open(const char *filename, int flags);

int fs_file_close(fs_file_t *fp);

int fs_file_sync(fs_file_t *fp);

int fs_file_read(fs_file_t *fp, void *buffer, int size);

int fs_file_write(fs_file_t *fp, const void *buffer, int size);

int fs_file_seek(fs_file_t *fp, int off, int whence);

int fs_file_size(fs_file_t *fp);

int fs_file_rewind(fs_file_t *fp);

fs_dir_t * fs_dir_open(const char * dirname, int * dirstat);

int fs_dir_read(fs_dir_t * dirp, struct lfs_info * info);

int fs_dir_close(fs_dir_t * dirp);
// remove file or dir
int fs_remove_file(const char *path, int len);

int fs_mkdir(const char *path);

int fs_rename(const char *oldpath, const char *newpath);

char *fs_get_curr_gpath(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __FS_H__k
