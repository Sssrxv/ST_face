#ifndef _FPC1150_H_
#define _FPC1150_H_

#include <stdint.h>

/**
 * @brief       init fpc1150 fingerprint sensor
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int fpc1150_init(void);


/**
 * @brief       get image resolution of fingerprint sensor
 * @param[in,out] width width of fingerprint image
 * @param[in,out] height height of fingerprint image
 */
void fpc1150_get_img_resolution(int *width, int *height);

/**
 * @brief       read one fingerprint image from fpc1150 fingerprint sensor
 * @note        should call fpc1150_init before call this function
 * @param[in]   img_buf    buffer to store image, get resolution by #fpc1150_get_img_resolution
 * @param[in]   buf_len    length of given buffer, should bigger than 80*208 bytes
 * 
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int fpc1150_read_fingerprint(uint8_t *img_buf, uint16_t buf_len);

/**
 * @brief       set sensor to sleep mode
 * @note        should call fpc1150_init before call this function
 *
 * @return      Result
 *     - 0      Success
 *     - Other  Fail
 */
int fpc1150_sleep(void);

#endif
