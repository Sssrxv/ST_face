#ifndef __MJPEG_H__
#define __MJPEG_H__

#include <stdint.h>
#include "frame_mgr.h"
#include "xvic.h"
#include "libjpeg_hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/** 
 * @brief  mjpeg_compress, compress one frame with jpeg
 * 
 * @param frame         input frame
 * @param level         compressing level
 * 
 * @return   NULL: error
 *
 */
/* --------------------------------------------------------------------------*/
frame_buf_t *mjpeg_compress(frame_buf_t *frame, mjpeg_comp_level_t level);

#ifdef __cplusplus
}
#endif

#endif // __MJPEG_H__
