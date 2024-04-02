#ifndef XNN_PARAMDICT_H
#define XNN_PARAMDICT_H

#include <stdio.h>
#include <stdint.h>
#include "mat.h"
#include "platform.h"

#define XNN_EXT_PARAM_START (20)
#define XNN_EXT_PARAM_COUNT (12)

// at most 20+XNN_AIVA_PARAM_COUNT parameters
#define XNN_MAX_PARAM_COUNT (20+XNN_EXT_PARAM_COUNT)

namespace xnn {

class Net;
class ParamDict
{
public:
    // empty
    ParamDict();

    // get int
    int get(int id, int def) const;
    // get float
    float get(int id, float def) const;
    // get array
    Mat get(int id, const Mat& def) const;

    // set int
    void set(int id, int i);
    // set float
    void set(int id, float f);
    // set array
    void set(int id, const Mat& v);

protected:
    friend class Net;

    void clear();

#if XNN_STDIO
    int load_param(FILE* fp);
#endif // XNN_STDIO
    int load_param(const unsigned char*& mem);

protected:
    struct
    {
        int loaded;
        union { int i; float f; };
        Mat v;
    } params[XNN_MAX_PARAM_COUNT];
};

} // namespace xnn

#endif // XNN_PARAMDICT_H
