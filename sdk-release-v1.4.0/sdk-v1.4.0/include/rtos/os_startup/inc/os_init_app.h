#ifndef __OS_INIT_APP_H__
#define __OS_INIT_APP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int os_init_app_entry(void *param);
int os_init_app_complete(void);
int os_get_flash_type(void);
int os_mount_fs(void);

#ifdef __cplusplus
}
#endif 

#endif // __OS_INIT_APP_H__
