#ifndef __CMDS_LOAD_H__
#define __CMDS_LOAD_H__

#include "cmds_load_callback.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief  register state callback of load
 * 
 * @param[in] cb  callback function called at different stage of load
 * 
 * @note Never call any syslog functions in this callback since load use the 
 *       same interface of syslog
 * 
 * @return void
 */
void cmds_load_register_state_callback(cmds_load_state_callback_t cb);

/** 
 * @brief  unregister state callback
 * 
 * @return void
 * 
 */
void cmds_load_unregister_state_callback();


#ifdef __cplusplus
}
#endif

#endif // __CMDS_LOAD_H__