#ifndef _I80_CTL_H_
#define _I80_CTL_H_

#include <stdint.h>
#include "i80_lcd_driver.h"
#include "mi_rd.h"
#include "frame_mgr.h"

#define I80_CTL_POLLING_MODE

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief       Enable i80 ctl device
 */
int i80_ctl_enable(void);

/**
 * @brief       Disable i80 ctl device
 */
int i80_ctl_disable(void);


/**
 * @brief       Config i80 ctl device
 */
int i80_ctl_config(display_i80_params_t *params);


/**
 * @brief       Reset i80 lcd device
 */
int i80_ctl_reset_device(void);

/**
 * @brief       Send i80 write commands
 */
int i80_ctl_send_wr_cmd(uint32_t cmd, const uint16_t *cmd_param, int cmd_param_len);

/**
 * @brief       Send i80 read commands
 */
int i80_ctl_send_rd_cmd(uint32_t cmd, uint16_t *cmd_result, int cmd_result_len);


int i80_ctl_write_ram_prepare(void);

/**
 * @brief       Display a new frame
 */
int i80_ctl_send_frame(const frame_buf_t *frame_buf);



#ifdef __cplusplus
}
#endif

#endif /* _I80_CTL_H_ */
