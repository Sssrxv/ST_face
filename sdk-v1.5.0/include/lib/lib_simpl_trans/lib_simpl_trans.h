#ifndef __LIB_SIMPL_TRANS_H__
#define __LIB_SIMPL_TRANS_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

//typedef void * simpl_handle_t;

typedef struct _simpl_trans_op_t {
    void *(*init)(void *param);
    int (*send)(const void *devh, void *buf, int buf_len, int timeout_ms);    /* send data function */
    int (*recv)(const void *devh, void *buf, int buf_len, int timeout_ms);    /* recv data function */
    int (*uninit)(const void *devh);
} simpl_trans_op_t;

typedef struct _simpl_handle_t {
    void *devh;
    simpl_trans_op_t op;
} simpl_handle_t;

enum {
    SIMPL_OK            = 0,
    SIMPL_ERR_TIMEOUT   = 1,
    SIMPL_ERR_INV_PARAM = 2,
    SIMPL_ERR_HEADER    = 3,
    SIMPL_ERR_CHECKSUM  = 4,
    SIMPL_ERR_XFER      = 5,
};

/* --------------------------------------------------------------------------*/
/** 
 * @brief  init simple transfer handler
 * 
 * @param op            send/recv operation pointer
 * @param param         param pointer
 * 
 * @return simple transfer handler, NULL if failed
 */
/* --------------------------------------------------------------------------*/
simpl_handle_t *simpl_trans_init(simpl_trans_op_t *op, void *param);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  simpl_trans_uninit 
 * 
 * @param handle
 */
/* --------------------------------------------------------------------------*/
void  simpl_trans_uninit(simpl_handle_t *handle);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  simpl_trans_send 
 * 
 * @param handle        simple transfer handler
 * @param buf           data buffer to be sent
 * @param buf_len       buffer length in byte
 * 
 * @return if return value < 0, fail, otherwise return sent bytes count
 */
/* --------------------------------------------------------------------------*/
int   simpl_trans_send(simpl_handle_t *handle, void *buf, int buf_len);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  simpl_trans_recv 
 * 
 * @param handle        simple transfer handler
 * @param buf           data buffer to receive
 * @param buf_len       max recv bytes
 * 
 * @return if return value < 0, fail, otherwise return received bytes count
 */
/* --------------------------------------------------------------------------*/
int   simpl_trans_recv(simpl_handle_t *handle, void *buf, int buf_len);

#ifdef __cplusplus
}
#endif

#endif  // __LIB_SIMPL_TRANS_H__
