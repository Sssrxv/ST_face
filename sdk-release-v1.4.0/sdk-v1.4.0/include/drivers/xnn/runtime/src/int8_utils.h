#ifndef XNN_INT8_UTILS_H
#define XNN_INT8_UTILS_H

static inline signed char float2int8(float v)
{
    int int32 = round(v);
    if (int32 > 127) return 127;
    if (int32 < -127) return -127;
    return (signed char)int32;
}

static inline signed char float2int8ceil(float v)
{
    int int32 = ceil(v);
    if (int32 > 127) return 127;
    if (int32 < -127) return -127;
    return (signed char)int32;
}

#endif
