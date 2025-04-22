#ifndef _LIBJPEG_WRAPPER_H_
#define _LIBJPEG_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    OUTPUT_FORMAT_RGB = 0,
    OUTPUT_FORMAT_GRAY,
    // OUTPUT_FORMAT_RGBA,
    // OUTPUT_FORMAT_BGRA,
} output_format_t;

/**
 * @brief decode jpeg
 * @param[in]   input_jpeg_buffer   buffer where jpeg store
 * @param[in]   buffer_len   length of jpeg buffer
 * @param[in]   output_format   configure format of decompressed image
 * @param[in,out]   output_buffer   decompressed image buffer
 * @param[in,out]   width   width of decompressed image
 * @param[in,out]   height   height of decompressed image
 * @return 0 for success, -1 for fail
 * @note if decode success, caller should free the output_buffer by call free_jpeg_buffer()
 */
int decode_jpeg(const uint8_t *input_jpeg_buffer, size_t buffer_len,
                output_format_t output_format,
                uint8_t **output_buffer, unsigned int *width, unsigned int *height);


void free_jpeg_buffer(uint8_t *buffer);


#ifdef __cplusplus
}
#endif

#endif
