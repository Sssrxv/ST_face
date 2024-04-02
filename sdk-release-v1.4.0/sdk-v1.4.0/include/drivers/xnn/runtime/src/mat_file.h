#ifndef XNN_MAT_FILE_H
#define XNN_MAT_FILE_H

#include "mat.h"
#include <string>


namespace xnn {

class MatFile
{
public:
    enum ElementType {
        ELEMENT_TYPE_INT8,
        ELEMENT_TYPE_UINT8,
        ELEMENT_TYPE_INT32,
        ELEMENT_TYPE_FP16,
        ELEMENT_TYPE_FP32
    };
    static bool is_enabled();
    static void enable_write();
    static void disable_write();
    static void set_directory(const std::string &dir);
    static void set_file_prefix(const std::string &prefix);
    static int write_to_file(const std::string &basename, const Mat & blob, ElementType element_type);
    static int write_dequantize_scales_to_file(const std::string &basename, const std::vector<float> &dequantize_scales);
};




} // namespace xnn

#endif //XNN_MAT_FILE_H
