#ifndef __ROTATE_H__
#define __ROTATE_H__

#include <stdint.h>
#include "frame_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROT_MOD_MIRROR              (1UL << 0)

/* rotate mode: 8 modes in total */
#define ROT_MOD_CLK_0               (1UL << 1)
#define ROT_MOD_CLK_90              (1UL << 2)
#define ROT_MOD_CLK_180             (1UL << 3)
#define ROT_MOD_CLK_270             (1UL << 4)
#define ROT_MOD_CLK_0_MIR           (ROT_MOD_CLK_0   | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_90_MIR          (ROT_MOD_CLK_90  | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_180_MIR         (ROT_MOD_CLK_180 | ROT_MOD_MIRROR)
#define ROT_MOD_CLK_270_MIR         (ROT_MOD_CLK_270 | ROT_MOD_MIRROR)

frame_buf_t *rotate_frame_start(frame_buf_t *buf_i, int rotate_mode);
int rotate_frame_finish(frame_buf_t *buf_o);

/* --------------------------------------------------------------------------*/
/** 
 * @brief  rotate_frame 
 * 
 * @param buf_in       frame buffer 
 * @param rotate_mode  rotate mode, 8 modes in total
 *                     ROT_MOD_CLK_0      : clockwise rotation   0 degrees
 *                     ROT_MOD_CLK_90     : clockwise rotation  90 degrees
 *                     ROT_MOD_CLK_180    : clockwise rotation 180 degrees
 *                     ROT_MOD_CLK_270    : clockwise rotation 270 degrees
 *                     ROT_MOD_CLK_0_MIR  : clockwise rotation   0 degrees and left-right mirror
 *                     ROT_MOD_CLK_90_MIR : clockwise rotation  90 degrees and left-right mirror
 *                     ROT_MOD_CLK_180_MIR: clockwise rotation 180 degrees and left-right mirror
 *                     ROT_MOD_CLK_270_MIR: clockwise rotation 270 degrees and left-right mirror
 * 
 * @return             frame buffer out, NULL if failed
 */
/* --------------------------------------------------------------------------*/
frame_buf_t *rotate_frame(frame_buf_t *buf_in, int rotate_mode, int flush_input);

#ifdef __cplusplus
}
#endif

#endif // __ROTATE_H__
