#ifndef XNN_MODEL_H
#define XNN_MODEL_H

#include <stdint.h>
#include "layer_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XNN_PARAM_FILE_MAGIC_NUMBER     (0xff081192)
#define XNN_PARAM_FILE_VERSION          (0x01)
#define XNN_PARAM_HW_VERSION            (0x01)

#define XNN_MODEL_FILE_MAGIC_NUMBER     (0xff081193)
#define XNN_MODEL_FILE_VERSION          (0x01)
#define XNN_MODEL_HW_VERSION            (0x01)

#define XNN_PACK_FILE_MAGIC_NUMBER     (0xff081194)
#define XNN_PACK_FILE_VERSION          (0x01)
#define XNN_PACK_HW_VERSION            (0x01)

#define XNN_PARAM_FLAG_LAYER_REORDED    (0x1)
typedef struct _xnn_param_file_header_t {
    uint32_t magic_number;      // 0xff, 0x08, 0x11, 0x92
    uint8_t  version;           // file format version
    uint8_t  hw_version;        // xnn hardware version
    uint8_t  padding[2];        // padding byte
    uint32_t flag;              // misc flag
    int32_t  layer_count;       // layer count
    int32_t  blob_count;        // blob count
    uint32_t weight_address;    // target weight load address
    int      weight_size;       // weight size in bytes
    int      workbuffer_size;   // workbuffer size in bytes
} __attribute__((packed, aligned(4))) xnn_param_file_header_t;


typedef struct _xnn_model_file_header_t {
    uint32_t magic_number;      // 0xff, 0x08, 0x11, 0x93
    uint8_t  version;           // file format version
    uint8_t  hw_version;        // xnn hardware version
    uint8_t  padding0[2];       // padding byte
    uint32_t flag;              // misc flag
    int16_t  core_count;        // used core count
    int16_t  core_idx;          // xnn core index if cour count is 1
    int      weight_offset;     // weight binary in the file offset
    int      weight_size;
} __attribute__((packed, aligned(4))) xnn_model_file_header_t;


typedef struct _xnn_file_header_t {
    uint32_t magic_number;      // 0xff, 0x08, 0x11, 0x94
    uint8_t  version;           // file format version
    uint8_t  padding[3];        // padding byte
    uint32_t xparam_size;       // xparam file size in bytes
    uint32_t xmodel_offset;     // xmodel file offset from file begin
    uint32_t xmodel_size;       // xmodel file size in bytes
    uint32_t padding0[3];       // kee xnn_file_header_t 16 bytes align
} __attribute__((packed, aligned(4))) xnn_file_header_t;


#ifdef __cplusplus
}
#endif

#endif
