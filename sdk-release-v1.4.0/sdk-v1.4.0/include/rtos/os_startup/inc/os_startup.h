#ifndef __OS_APP_MAIN__
#define __OS_APP_MAIN__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*os_app_entry_func_t)(void);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_app_set_custom_entry  set custom entry function
 * 
 * @param func_entry                the custom entry to be registered
 * 
 * @return 0 : OK, -1 : FAIL
 */
/* --------------------------------------------------------------------------*/
int os_app_set_custom_entry(os_app_entry_func_t func_entry);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  os_app_main_entry        os main entry to start scheduler
 * 
 * @return 0 : OK, -1 : FAIL
 */
/* --------------------------------------------------------------------------*/
int os_app_main_entry(void);

#ifdef __cplusplus
}
#endif 

#endif // __OS_APP_MAIN__
