#ifndef XNN_MAT_H
#define XNN_MAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if __ARM_NEON
#include <arm_neon.h>
#endif
#include "platform.h"
#include "allocator.h"
#if !XNN_SIM
#include "system_AI3100.h"
#endif

namespace xnn {

typedef struct _MatParam_t
{
    int dims;
    int w;
    int h;
    int c;
    int cn;
    int elemsize;
}MatParam_t;



// the three dimension matrix
class Mat
{
public:
    enum DataFormat
    {
        DATA_FORMAT_CHW,
        DATA_FORMAT_HWC_BGR,
        DATA_FORMAT_HWC_RGB,
        DATA_FORMAT_HWC_RGBA,
        DATA_FORMAT_HWC_C16N,
        DATA_FORMAT_YUV422SP,
    };

    enum DataType
    {
        DATA_TYPE_FP32 = 0,
        DATA_TYPE_INT8,
        DATA_TYPE_UINT8,
        DATA_TYPE_FP16,
    };

    // empty
    Mat();
    // vec
    Mat(int w, size_t elemsize = 4u, Allocator* allocator = 0);
    // image
    Mat(int w, int h, size_t elemsize = 4u, Allocator* allocator = 0);
    // dim
    Mat(int w, int h, int c, size_t elemsize = 4u, Allocator* allocator = 0);
    // packed vec
    Mat(int w, size_t elemsize, int packing, Allocator* allocator = 0);
    // packed image
    Mat(int w, int h, size_t elemsize, int packing, Allocator* allocator = 0);
    // packed dim
    Mat(int w, int h, int c, size_t elemsize, int packing, Allocator* allocator = 0);
    // chw, bgr, rgb, rgba, hwc16n
    Mat(int w, int h, int c, size_t elemsize, int packing, DataFormat dataformat, Allocator* allocator = 0);
    // copy
    Mat(const Mat& m);
    // external vec
    Mat(int w, void* data, size_t elemsize = 4u, Allocator* allocator = 0);
    // external image
    Mat(int w, int h, void* data, size_t elemsize = 4u, Allocator* allocator = 0);
    // external dim
    Mat(int w, int h, int c, void* data, size_t elemsize = 4u, Allocator* allocator = 0);
    // external packed vec
    Mat(int w, void* data, size_t elemsize, int packing, Allocator* allocator = 0);
    // external packed image
    Mat(int w, int h, void* data, size_t elemsize, int packing, Allocator* allocator = 0);
    // external packed dim
    Mat(int w, int h, int c, void* data, size_t elemsize, int packing, Allocator* allocator = 0);
    // external chw, bgr, rgb, rgba, hwc16n
    Mat(int w, int h, int c, void* data, size_t elemsize, int packing, DataFormat dataformat, Allocator* allocator = 0);
    // release
    ~Mat();
    // assign
    Mat& operator=(const Mat& m);
    // set all
    void fill(float v);
    void fill(int v);
    template <typename T> void fill(T v);
    // deep copy
    Mat clone(Allocator* allocator = 0) const;
    // reshape vec
    Mat reshape(int w, Allocator* allocator = 0) const;
    // reshape image
    Mat reshape(int w, int h, Allocator* allocator = 0) const;
    // reshape dim
    Mat reshape(int w, int h, int c, Allocator* allocator = 0) const;
    // allocate vec
    void create(int w, size_t elemsize = 4u, Allocator* allocator = 0);
    // allocate image
    void create(int w, int h, size_t elemsize = 4u, Allocator* allocator = 0);
    // allocate dim
    void create(int w, int h, int c, size_t elemsize = 4u, Allocator* allocator = 0);
    // allocate packed vec
    void create(int w, size_t elemsize, int packing, Allocator* allocator = 0);
    // allocate packed image
    void create(int w, int h, size_t elemsize, int packing, Allocator* allocator = 0);
    // allocate packed dim
    void create(int w, int h, int c, size_t elemsize, int packing, Allocator* allocator = 0);
    // chw, bgr, rgb, rgba, hwc16n
    void create(int w, int h, int c, size_t elemsize, int packing, DataFormat dataformat, Allocator* allocator = 0);
    // allocate like
    void create_like(const Mat& m, Allocator* allocator = 0);
    // refcount++
    void addref();
    // refcount--
    void release();

    bool empty() const;
    size_t total() const;

    // shape only
    Mat shape() const;

    // data reference
    Mat channel(int c);
    const Mat channel(int c) const;
    float* row(int y);
    const float* row(int y) const;
    template<typename T> T* row(int y);
    template<typename T> const T* row(int y) const;

    // range reference
    Mat channel_range(int c, int channels);
    const Mat channel_range(int c, int channels) const;
    Mat row_range(int y, int rows);
    const Mat row_range(int y, int rows) const;
    Mat range(int x, int n);
    const Mat range(int x, int n) const;

    // access raw data
    template<typename T> operator T*();
    template<typename T> operator const T*() const;

    // convenient access float vec element
    float& operator[](int i);
    const float& operator[](int i) const;

#if XNN_PIXEL
    enum
    {
        PIXEL_CONVERT_SHIFT = 16,
        PIXEL_FORMAT_MASK = 0x0000ffff,
        PIXEL_CONVERT_MASK = 0xffff0000,

        PIXEL_RGB       = 1,
        PIXEL_BGR       = (1 << 1),
        PIXEL_GRAY      = (1 << 2),
        PIXEL_RGBA      = (1 << 3),

        PIXEL_RGB2BGR   = PIXEL_RGB | (PIXEL_BGR << PIXEL_CONVERT_SHIFT),
        PIXEL_RGB2GRAY  = PIXEL_RGB | (PIXEL_GRAY << PIXEL_CONVERT_SHIFT),

        PIXEL_BGR2RGB   = PIXEL_BGR | (PIXEL_RGB << PIXEL_CONVERT_SHIFT),
        PIXEL_BGR2GRAY  = PIXEL_BGR | (PIXEL_GRAY << PIXEL_CONVERT_SHIFT),

        PIXEL_GRAY2RGB  = PIXEL_GRAY | (PIXEL_RGB << PIXEL_CONVERT_SHIFT),
        PIXEL_GRAY2BGR  = PIXEL_GRAY | (PIXEL_BGR << PIXEL_CONVERT_SHIFT),

        PIXEL_RGBA2RGB  = PIXEL_RGBA | (PIXEL_RGB << PIXEL_CONVERT_SHIFT),
        PIXEL_RGBA2BGR  = PIXEL_RGBA | (PIXEL_BGR << PIXEL_CONVERT_SHIFT),
        PIXEL_RGBA2GRAY = PIXEL_RGBA | (PIXEL_GRAY << PIXEL_CONVERT_SHIFT),
    };
    // convenient construct from pixel data
    static Mat from_pixels(const unsigned char* pixels, int type, int w, int h, Allocator* allocator = 0);
    // convenient construct from pixel data and resize to specific size
    static Mat from_pixels_resize(const unsigned char* pixels, int type, int w, int h, int target_width, int target_height, Allocator* allocator = 0);

    // convenient export to pixel data
    void to_pixels(unsigned char* pixels, int type) const;
    // convenient export to pixel data and resize to specific size
    void to_pixels_resize(unsigned char* pixels, int type, int target_width, int target_height) const;
#endif // XNN_PIXEL

    // substract channel-wise mean values, then multiply by normalize values, pass 0 to skip
    void substract_mean_normalize(const float* mean_vals, const float* norm_vals);

    // convenient construct from half precisoin floating point data
    static Mat from_float16(const unsigned short* data, int size);

    DataFormat dataformat;

    // pointer to the data
    void* data;

    // pointer to the reference counter
    // when points to user-allocated data, the pointer is NULL
    int* refcount;

    // element size in bytes
    // 4 = float32/int32
    // 2 = float16
    // 1 = int8/uint8
    // 0 = empty
    size_t elemsize;

    // packed count inside element
    // c/1-h-w-1  h/1-w-1  w/1-1  scalar
    // c/4-h-w-4  h/4-w-4  w/4-4  sse/neon
    // c/8-h-w-8  h/8-w-8  w/8-8  avx/fp16
    int packing;

    // the allocator
    Allocator* allocator;

    // the dimensionality
    int dims;

    int w;
    int h;
    int c;
    int cn;

    size_t cstep;

    // scale for change between int8 and float32
    float scale_to_fp32; // multiplied scale_to_fp32 to convert int8 mat to fp32 mat.
    float scale_to_int8; // multiplied scale_to_int8 to convert fp32 mat to int8 mat.
};


// misc function
#if XNN_PIXEL
// convert yuv420sp(nv21) to rgb, the fast approximate version
void yuv420sp2rgb(const unsigned char* yuv420sp, int w, int h, unsigned char* rgb);
// image pixel bilinear resize
void resize_bilinear_c1(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
void resize_bilinear_c2(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
void resize_bilinear_c3(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
void resize_bilinear_c4(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
// image pixel bilinear resize, convenient wrapper for yuv420sp(nv21)
void resize_bilinear_yuv420sp(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
#endif // XNN_PIXEL

// mat process
enum
{
    BORDER_CONSTANT = 0,
    BORDER_REPLICATE = 1,
};
void copy_make_border(const Mat& src, Mat& dst, int top, int bottom, int left, int right, int type, float v, Allocator* allocator = 0, int num_threads = 1);
void copy_cut_border(const Mat& src, Mat& dst, int top, int bottom, int left, int right, Allocator* allocator = 0, int num_threads = 1);
void resize_bilinear(const Mat& src, Mat& dst, int w, int h, Allocator* allocator = 0, int num_threads = 1);
void resize_bicubic(const Mat& src, Mat& dst, int w, int h, Allocator* allocator = 0, int num_threads = 1);
void convert_packing(const Mat& src, Mat& dst, int packing, Allocator* allocator = 0, int num_threads = 1);
void cast_float32_to_float16(const Mat& src, Mat& dst, Allocator* allocator = 0, int num_threads = 1);
void cast_float16_to_float32(const Mat& src, Mat& dst, Allocator* allocator = 0, int num_threads = 1);
int convert_hwc_align_to_chw(const xnn::Mat &src, xnn::Mat &dst, int c, Allocator *allocator = 0, int num_threads = 1);
int convert_chw_to_hwc_align(const xnn::Mat &src, xnn::Mat &dst, int cn, Allocator *allocator = 0, int num_threads = 1);
float dot_product(const xnn::Mat &f0, const xnn::Mat &f1, Allocator* allocator = 0, int num_threads = 1);
int normalize(const xnn::Mat &in, xnn::Mat &out, Allocator* allocator = 0, int num_threads = 1);

int chw2hwc_int8(const xnn::Mat &src, void* dst_data, int cn);
int chw2hwc_float16(const xnn::Mat &src, void* dst_data, int cn);
int chw2hwc_float32_to_int8(const xnn::Mat &src, void* dst_data, int cn, float scale);
int chw2hwc_float32_to_int8_with_float16_multiply(const xnn::Mat &src, void* dst_data, int cn, float scale);
int chw2hwc_uint8_to_int8(const xnn::Mat &src, void* dst_data, int cn, float scale);
int hwc2chw_int8(void *src_data, xnn::Mat &dst, int cn, int dims, int w, int h, int c, bool reshape_1x1 = false, Allocator* allocator = 0);
int hwc2chw_float16(void *src_data, xnn::Mat &dst, int cn, int dims, int w, int h, int c, bool reshape_1x1 = false, Allocator* allocator = 0);
int hwc2chw_uint8_to_float32(void *src_data, xnn::Mat &dst, int cn, int dims, int w, int h, int c, float scale, Allocator* allocator = 0);
int hwc2chw_int8_to_float32(void *src_data, xnn::Mat &dst, int cn, int dims, int w, int h, int c, float scale, bool reshape_1x1 = false, Allocator* allocator = 0);
int hwc2chw_float16_to_float32(void *src_data, xnn::Mat &dst, int cn, int dims, int w, int h, int c, bool reshape_1x1 = false, Allocator* allocator = 0);
int chw2chw_int8_to_float32(const xnn::Mat &src, xnn::Mat &dst, Allocator* allocator = 0);
int chw2chw_float32_to_int8(const xnn::Mat &src, xnn::Mat &dst, Allocator* allocator = 0);
int hwc2hwc_channel_align_int8(const xnn::Mat &blob_in, xnn::Mat &blob_out, Allocator* allocator = 0);
int convert_to_uint8_rgba(const xnn::Mat &src, xnn::Mat &dst);
int chw2hwc_float32_to_uint8(const xnn::Mat &src, void* dst_data, int cn, float scale);
int rgba_float32_to_rgba_uint8(const xnn::Mat &blob_in, xnn::Mat &blob_out, Allocator* allocator = 0);
int rgba2hwc_float32_to_int8(const xnn::Mat &blob_in, xnn::Mat &blob_out, float scale_to_int8, Allocator* allocator = 0);
int rgba2hwc_uint8_to_int8(const xnn::Mat &blob_in, xnn::Mat &blob_out, float scale_to_int8, Allocator* allocator = 0);

class WeightCnvParam
{
public:
    int kw;
    int kh;
    int ic;
    int oc;
    int in;
    int on;
};


class WeightCnvDw32Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightCnvDw64Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightCnvGc4x4Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightCnvGc8x8Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightCnvGc16x16Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightCnvGc32x32Param
{
public:
    int kw;
    int kh;
    int ic;
    int in;
};


class WeightFcParam
{
public:
    int ic;
    int oc;
    int in;
    int on;
};

class WeightPerChnlParam
{
public:
    int oc;
    int on;
};

int convert_cnv_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvParam *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvdw32_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvDw32Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvdw64_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvDw64Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvgc4x4_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvGc4x4Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvgc8x8_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvGc8x8Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvgc16x16_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvGc16x16Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_cnvgc32x32_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightCnvGc32x32Param *param, Allocator *allocator = 0, int num_threads = 1);
int convert_fc_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightFcParam *param, int bdims, int bc, int bw, int bh, Allocator *allocator = 0, int num_threads = 1);
int convert_perchannel_weight_to_hw(const xnn::Mat &src, xnn::Mat &dst, const WeightPerChnlParam *param, Allocator *allocator = 0, int num_threads = 1);



inline Mat::Mat()
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
}

inline Mat::Mat(int _w, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _elemsize, _allocator);
}

inline Mat::Mat(int _w, int _h, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _h, _elemsize, _allocator);
}

inline Mat::Mat(int _w, int _h, int _c, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _h, _c, _elemsize, _allocator);
}

inline Mat::Mat(int _w, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _elemsize, _packing, _allocator);
}

inline Mat::Mat(int _w, int _h, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _h, _elemsize, _packing, _allocator);
}

inline Mat::Mat(int _w, int _h, int _c, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _h, _c, _elemsize, _packing, _allocator);
}


inline Mat::Mat(int _w, int _h, int _c, size_t _elemsize, int _packing, DataFormat _dataformat, Allocator* _allocator)
    : dataformat(_dataformat), data(0), refcount(0), elemsize(0), packing(0), allocator(0), dims(0), w(0), h(0), c(0), cn(0), cstep(0), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    create(_w, _h, _c, _elemsize, _packing, _dataformat, _allocator);
}


inline Mat::Mat(const Mat& m)
    : dataformat(m.dataformat), data(m.data), refcount(m.refcount), elemsize(m.elemsize), packing(m.packing), allocator(m.allocator), dims(m.dims), w(m.w), h(m.h), c(m.c), cn(m.cn), cstep(m.cstep), scale_to_fp32(m.scale_to_fp32), scale_to_int8(m.scale_to_int8)
{
    if (refcount)
        XNN_XADD(refcount, 1);
}

inline Mat::Mat(int _w, void* _data, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(1), allocator(_allocator), dims(1), w(_w), h(1), c(1), cn(1), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = w;
}

inline Mat::Mat(int _w, int _h, void* _data, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(1), allocator(_allocator), dims(2), w(_w), h(_h), c(1), cn(1), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = w * h;
}

inline Mat::Mat(int _w, int _h, int _c, void* _data, size_t _elemsize, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(1), allocator(_allocator), dims(3), w(_w), h(_h), c(_c), cn(_c), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = alignSize(w * h * elemsize, 16) / elemsize;
}

inline Mat::Mat(int _w, void* _data, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(_packing), allocator(_allocator), dims(1), w(_w), h(1), c(1), cn(1), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = w;
}

inline Mat::Mat(int _w, int _h, void* _data, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(_packing), allocator(_allocator), dims(2), w(_w), h(_h), c(1), cn(1), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = w * h;
}

inline Mat::Mat(int _w, int _h, int _c, void* _data, size_t _elemsize, int _packing, Allocator* _allocator)
    : dataformat(DATA_FORMAT_CHW), data(_data), refcount(0), elemsize(_elemsize), packing(_packing), allocator(_allocator), dims(3), w(_w), h(_h), c(_c), cn(_c), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = alignSize(w * h * elemsize, 16) / elemsize;
}


inline Mat::Mat(int _w, int _h, int _c, void* _data, size_t _elemsize, int _packing, DataFormat _dataformat, Allocator* _allocator)
    : dataformat(_dataformat), data(_data), refcount(0), elemsize(_elemsize), packing(_packing), allocator(_allocator), dims(3), w(_w), h(_h), c(_c), cn(_c), scale_to_fp32(1.0), scale_to_int8(1.0)
{
    cstep = alignSize(w * h * elemsize, 16) / elemsize;
}

inline Mat::~Mat()
{
    release();
}

inline Mat& Mat::operator=(const Mat& m)
{
    if (this == &m)
        return *this;

    if (m.refcount)
        XNN_XADD(m.refcount, 1);

    release();

    data = m.data;
    refcount = m.refcount;
    elemsize = m.elemsize;
    packing = m.packing;
    allocator = m.allocator;

    dataformat = m.dataformat;

    dims = m.dims;
    w = m.w;
    h = m.h;
    c = m.c;
    cn = m.cn;

    cstep = m.cstep;
    scale_to_fp32 = m.scale_to_fp32;
    scale_to_int8 = m.scale_to_int8;

    return *this;
}

inline void Mat::fill(float _v)
{
    int size = (int)total();
    float* ptr = (float*)data;

#if __ARM_NEON
    int nn = size >> 2;
    int remain = size - (nn << 2);
#else
    int remain = size;
#endif // __ARM_NEON

#if __ARM_NEON
    float32x4_t _c = vdupq_n_f32(_v);
#if __aarch64__
    if (nn > 0)
    {
    asm volatile (
        "0:                             \n"
        "subs       %w0, %w0, #1        \n"
        "st1        {%4.4s}, [%1], #16  \n"
        "bne        0b                  \n"
        : "=r"(nn),     // %0
          "=r"(ptr)     // %1
        : "0"(nn),
          "1"(ptr),
          "w"(_c)       // %4
        : "cc", "memory"
    );
    }
#else
    if (nn > 0)
    {
    asm volatile(
        "0:                             \n"
        "subs       %0, #1              \n"
        "vst1.f32   {%e4-%f4}, [%1 :128]!\n"
        "bne        0b                  \n"
        : "=r"(nn),     // %0
          "=r"(ptr)     // %1
        : "0"(nn),
          "1"(ptr),
          "w"(_c)       // %4
        : "cc", "memory"
    );
    }
#endif // __aarch64__
#endif // __ARM_NEON
    for (; remain>0; remain--)
    {
        *ptr++ = _v;
    }
}

inline void Mat::fill(int _v)
{
    int size = (int)total();
    int* ptr = (int*)data;

#if __ARM_NEON
    int nn = size >> 2;
    int remain = size - (nn << 2);
#else
    int remain = size;
#endif // __ARM_NEON

#if __ARM_NEON
    int32x4_t _c = vdupq_n_s32(_v);
#if __aarch64__
    if (nn > 0)
    {
    asm volatile (
        "0:                             \n"
        "subs       %w0, %w0, #1        \n"
        "st1        {%4.4s}, [%1], #16  \n"
        "bne        0b                  \n"
        : "=r"(nn),     // %0
          "=r"(ptr)     // %1
        : "0"(nn),
          "1"(ptr),
          "w"(_c)       // %4
        : "cc", "memory"
    );
    }
#else
    if (nn > 0)
    {
    asm volatile(
        "0:                             \n"
        "subs       %0, #1              \n"
        "vst1.s32   {%e4-%f4}, [%1 :128]!\n"
        "bne        0b                  \n"
        : "=r"(nn),     // %0
          "=r"(ptr)     // %1
        : "0"(nn),
          "1"(ptr),
          "w"(_c)       // %4
        : "cc", "memory"
    );
    }
#endif // __aarch64__
#endif // __ARM_NEON
    for (; remain>0; remain--)
    {
        *ptr++ = _v;
    }
}

template <typename T>
inline void Mat::fill(T _v)
{
    int size = total();
    T* ptr = (T*)data;
    for (int i=0; i<size; i++)
    {
        ptr[i] = _v;
    }
}

inline Mat Mat::clone(Allocator* allocator) const
{
    if (empty())
        return Mat();

    Mat m;
    if (dims == 1)
    {
        m.create(w, elemsize, packing, allocator);
    }
    else if (dims == 2)
    {
        m.create(w, h, elemsize, packing, allocator);
    }
    else if (dims == 3)
    {
        if ((dataformat == DATA_FORMAT_HWC_BGR) ||
            (dataformat == DATA_FORMAT_HWC_RGBA) ||
            (dataformat == DATA_FORMAT_HWC_C16N))
        {
            m.create(w, h, c, elemsize, packing, dataformat, allocator);
        }
        else
        {
            m.create(w, h, c, elemsize, packing, allocator);
        }
    }

    if (total() > 0)
    {
        memcpy(m.data, data, total() * elemsize);
    }

    return m;
}

inline Mat Mat::reshape(int _w, Allocator* _allocator) const
{
    if (w * h * c != _w)
        return Mat();

    if (dims == 3 && cstep != (size_t)w * h)
    {
        Mat m;
        m.create(_w, elemsize, packing, _allocator);

        // flatten
        for (int i=0; i<c; i++)
        {
            const void* ptr = (unsigned char*)data + i * cstep * elemsize;
            void* mptr = (unsigned char*)m.data + i * w * h * elemsize;
            memcpy(mptr, ptr, w * h * elemsize);
        }

        return m;
    }

    Mat m = *this;

    m.dims = 1;
    m.w = _w;
    m.h = 1;
    m.c = 1;
    m.cn = 1;

    m.cstep = _w;

    return m;
}

inline Mat Mat::reshape(int _w, int _h, Allocator* _allocator) const
{
    if (w * h * c != _w * _h)
        return Mat();

    if (dims == 3 && cstep != (size_t)w * h)
    {
        Mat m;
        m.create(_w, _h, elemsize, packing, _allocator);

        // flatten
        for (int i=0; i<c; i++)
        {
            const void* ptr = (unsigned char*)data + i * cstep * elemsize;
            void* mptr = (unsigned char*)m.data + i * w * h * elemsize;
            memcpy(mptr, ptr, w * h * elemsize);
        }

        return m;
    }

    Mat m = *this;

    m.dims = 2;
    m.w = _w;
    m.h = _h;
    m.c = 1;
    m.cn = 1;

    m.cstep = _w * _h;

    return m;
}

inline Mat Mat::reshape(int _w, int _h, int _c, Allocator* _allocator) const
{
    if (w * h * c != _w * _h * _c)
        return Mat();

    if (dims < 3)
    {
        if ((size_t)_w * _h != alignSize(_w * _h * elemsize, 16) / elemsize)
        {
            Mat m;
            m.create(_w, _h, _c, elemsize, packing, _allocator);

            // align channel
            for (int i=0; i<_c; i++)
            {
                const void* ptr = (unsigned char*)data + i * _w * _h * elemsize;
                void* mptr = (unsigned char*)m.data + i * m.cstep * m.elemsize;
                memcpy(mptr, ptr, _w * _h * elemsize);
            }

            return m;
        }
    }
    else if (c != _c)
    {
        // flatten and then align
        Mat tmp = reshape(_w * _h * _c, _allocator);
        return tmp.reshape(_w, _h, _c, _allocator);
    }

    Mat m = *this;

    m.dims = 3;
    m.w = _w;
    m.h = _h;
    m.c = _c;
    m.cn = _c;

    m.cstep = alignSize(_w * _h * elemsize, 16) / elemsize;

    return m;
}

inline void Mat::create(int _w, size_t _elemsize, Allocator* _allocator)
{
    if (dims == 1 && w == _w && elemsize == _elemsize && packing == 1 && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = 1;
    allocator = _allocator;

    dims = 1;
    w = _w;
    h = 1;
    c = 1;
    cn = 1;

    cstep = w;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}

inline void Mat::create(int _w, int _h, size_t _elemsize, Allocator* _allocator)
{
    if (dims == 2 && w == _w && h == _h && elemsize == _elemsize && packing == 1 && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = 1;
    allocator = _allocator;

    dims = 2;
    w = _w;
    h = _h;
    c = 1;
    cn = 1;

    cstep = w * h;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}

inline void Mat::create(int _w, int _h, int _c, size_t _elemsize, Allocator* _allocator)
{
    if (dims == 3 && w == _w && h == _h && c == _c && elemsize == _elemsize && packing == 1 && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = 1;
    allocator = _allocator;

    dims = 3;
    w = _w;
    h = _h;
    c = _c;
    cn = _c;

    cstep = alignSize(w * h * elemsize, 16) / elemsize;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}

inline void Mat::create(int _w, size_t _elemsize, int _packing, Allocator* _allocator)
{
    if (dims == 1 && w == _w && elemsize == _elemsize && packing == _packing && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = _packing;
    allocator = _allocator;

    dims = 1;
    w = _w;
    h = 1;
    c = 1;
    cn = 1;

    cstep = w;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}

inline void Mat::create(int _w, int _h, size_t _elemsize, int _packing, Allocator* _allocator)
{
    if (dims == 2 && w == _w && h == _h && elemsize == _elemsize && packing == _packing && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = _packing;
    allocator = _allocator;

    dims = 2;
    w = _w;
    h = _h;
    c = 1;
    cn = 1;

    cstep = w * h;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}

inline void Mat::create(int _w, int _h, int _c, size_t _elemsize, int _packing, Allocator* _allocator)
{
    if (dims == 3 && w == _w && h == _h && c == _c && elemsize == _elemsize && packing == _packing && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = _packing;
    allocator = _allocator;

    dims = 3;
    w = _w;
    h = _h;
    c = _c;
    cn = _c;

    cstep = alignSize(w * h * elemsize, 16) / elemsize;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}


inline void Mat::create(int _w, int _h, int _c, size_t _elemsize, int _packing, DataFormat _dataformat, Allocator* _allocator)
{
    if (dims == 3 && w == _w && h == _h && c == _c && elemsize == _elemsize && packing == _packing && dataformat == _dataformat && allocator == _allocator)
        return;

    release();

    elemsize = _elemsize;
    packing = _packing;
    allocator = _allocator;

    dims = 3;
    w = _w;
    h = _h;
    c = _c;
    cn = _c;
    dataformat = _dataformat;

    cstep = alignSize(w * h * elemsize, 16) / elemsize;

    if (total() > 0)
    {
        size_t totalsize = alignSize(total() * elemsize, 4);
        if (allocator)
            data = allocator->fastMalloc(totalsize + (int)sizeof(*refcount));
        else
            data = fastMalloc(totalsize + (int)sizeof(*refcount));
        refcount = (int*)(((unsigned char*)data) + totalsize);
        *refcount = 1;
    }
}


inline void Mat::create_like(const Mat& m, Allocator* _allocator)
{
    if (m.dims == 1)
        create(m.w, m.elemsize, m.packing, _allocator);
    else if (m.dims == 2)
        create(m.w, m.h, m.elemsize, m.packing, _allocator);
    else if (m.dims == 3)
        create(m.w, m.h, m.c, m.elemsize, m.packing, _allocator);
}

inline void Mat::addref()
{
    if (refcount)
        XNN_XADD(refcount, 1);
}

inline void Mat::release()
{
    if (refcount && XNN_XADD(refcount, -1) == 1)
    {
        if (allocator)
            allocator->fastFree(data);
        else
            fastFree(data);
    }

    data = 0;

    elemsize = 0;
    packing = 0;

    dims = 0;
    w = 0;
    h = 0;
    c = 0;
    cn = 0;

    cstep = 0;
    scale_to_fp32 = 1.0;
    scale_to_int8 = 1.0;

    refcount = 0;
}

inline bool Mat::empty() const
{
    return data == 0 || total() == 0;
}

inline size_t Mat::total() const
{
    return cstep * c;
}

inline Mat Mat::shape() const
{
    if (dims == 1)
        return Mat(w * packing, (void*)0);
    if (dims == 2)
        return Mat(w, h * packing, (void*)0);
    if (dims == 3)
        return Mat(w, h, c * packing, (void*)0);

    return Mat();
}

inline Mat Mat::channel(int _c)
{
    return Mat(w, h, (unsigned char*)data + cstep * _c * elemsize, elemsize, packing, allocator);
}

inline const Mat Mat::channel(int _c) const
{
    return Mat(w, h, (unsigned char*)data + cstep * _c * elemsize, elemsize, packing, allocator);
}

inline float* Mat::row(int y)
{
    return (float*)((unsigned char*)data + w * y * elemsize);
}

inline const float* Mat::row(int y) const
{
    return (const float*)((unsigned char*)data + w * y * elemsize);
}

template <typename T>
inline T* Mat::row(int y)
{
    return (T*)((unsigned char*)data + w * y * elemsize);
}

template <typename T>
inline const T* Mat::row(int y) const
{
    return (const T*)((unsigned char*)data + w * y * elemsize);
}

inline Mat Mat::channel_range(int _c, int channels)
{
    return Mat(w, h, channels, (unsigned char*)data + cstep * _c * elemsize, elemsize, packing, allocator);
}

inline const Mat Mat::channel_range(int _c, int channels) const
{
    return Mat(w, h, channels, (unsigned char*)data + cstep * _c * elemsize, elemsize, packing, allocator);
}

inline Mat Mat::row_range(int y, int rows)
{
    return Mat(w, rows, (unsigned char*)data + w * y * elemsize, elemsize, packing, allocator);
}

inline const Mat Mat::row_range(int y, int rows) const
{
    return Mat(w, rows, (unsigned char*)data + w * y * elemsize, elemsize, packing, allocator);
}

inline Mat Mat::range(int x, int n)
{
    return Mat(n, (unsigned char*)data + x * elemsize, elemsize, packing, allocator);
}

inline const Mat Mat::range(int x, int n) const
{
    return Mat(n, (unsigned char*)data + x * elemsize, elemsize, packing, allocator);
}

template <typename T>
inline Mat::operator T*()
{
    return (T*)data;
}

template <typename T>
inline Mat::operator const T*() const
{
    return (const T*)data;
}

inline float& Mat::operator[](int i)
{
    return ((float*)data)[i];
}

inline const float& Mat::operator[](int i) const
{
    return ((const float*)data)[i];
}

} // namespace xnn

#endif // XNN_MAT_H
