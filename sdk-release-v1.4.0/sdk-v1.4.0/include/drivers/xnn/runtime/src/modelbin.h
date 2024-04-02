#ifndef XNN_MODELBIN_H
#define XNN_MODELBIN_H

#include <string>
#include <stdio.h>
#include <stdint.h>
#include "mat.h"
#include "platform.h"

namespace xnn {

class Net;
class ModelBin
{
public:
    enum MbElemType {
        AUTO    = 0,
        FLOAT32 = 1,
        FLOAT16 = 2,
        INT8    = 3,
        UINT8   = 4,
        INT16   = 5,
        UINT16  = 6,
        INT32   = 7,
        UINT32  = 8
    };
    // element type
    // 0 = auto
    // 1 = float32
    // 2 = float16
    // 3 = int8
    // 4 = uint8
    // 5 = int16
    // 6 = uint16
    // 7 = int32
    // 8 = uint32
    // load vec
    virtual Mat load(int w, int type) const = 0;
    // load image
    virtual Mat load(int w, int h, int type) const;
    // load dim
    virtual Mat load(int w, int h, int c, int type) const;

    virtual int8_t   load_int8(void) const;
    virtual int16_t  load_int16(void) const;
    virtual int32_t  load_int32(void) const;

    virtual uint8_t  load_uint8(void) const;
    virtual uint16_t load_uint16(void) const;
    virtual uint32_t load_uint32(void) const;

    virtual float    load_float32(void) const;

    virtual std::string load_string(void) const;
};

#if XNN_STDIO
class ModelBinFromStdio : public ModelBin
{
public:
    // construct from file
    ModelBinFromStdio(FILE* binfp);

    virtual Mat load(int w, int type) const;

    virtual int8_t   load_int8(void) const;
    virtual int16_t  load_int16(void) const;
    virtual int32_t  load_int32(void) const;

    virtual uint8_t  load_uint8(void) const;
    virtual uint16_t load_uint16(void) const;
    virtual uint32_t load_uint32(void) const;

    virtual float    load_float32(void) const;

    virtual std::string load_string(void) const;
protected:
    FILE* binfp;
};
#endif // XNN_STDIO


class ModelBinFromMemory : public ModelBin
{
public:
    // construct from external memory
    ModelBinFromMemory(const unsigned char*& mem);

    virtual Mat load(int w, int type) const;

    virtual int8_t   load_int8(void) const;
    virtual int16_t  load_int16(void) const;
    virtual int32_t  load_int32(void) const;

    virtual uint8_t  load_uint8(void) const;
    virtual uint16_t load_uint16(void) const;
    virtual uint32_t load_uint32(void) const;

    virtual float    load_float32(void) const;

    virtual std::string load_string(void) const;
protected:
    const unsigned char*& mem;
};

class ModelBinFromMatArray : public ModelBin
{
public:
    // construct from weight blob array
    ModelBinFromMatArray(const Mat* weights);

    virtual Mat load(int w, int type) const;

protected:
    mutable const Mat* weights;
};


} // namespace xnn

#endif // XNN_MODELBIN_H
