#ifndef __CMDS_LOAD_CALLBACK_H__
#define __CMDS_LOAD_CALLBACK_H__

/**
 * @brief enum of load state
 */
typedef enum cmds_load_state
{
    CMDS_LOAD_STATE_TRANSFER = 0,          /** start to reive data from host */
    CMDS_LOAD_STATE_SYNC_TO_DEVICE,        /** start to write to target device, partition, file, etc.*/
    CMDS_LOAD_STATE_VERIFY,                /** start to do data verification*/
    CMDS_LOAD_STATE_SUCCESS,               /** load success */
    CMDS_LOAD_STATE_FAILED,                /** load failed */
} cmds_load_state_t;

/**
 * @brief  callback function called at different stage of load
 *
 * @param[in]  current_state  current state of load process
 *
 * @return void
 */
typedef void (*cmds_load_state_callback_t)(cmds_load_state_t current_state);

#endif // __CMDS_LOAD_CALLBACK_H__
