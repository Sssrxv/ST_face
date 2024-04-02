#ifndef XNN_ALLOCATOR_H
#define XNN_ALLOCATOR_H

#include <stdlib.h>
#include <list>
#include <vector>
#include "platform.h"
#include "xnn_log.h"

namespace xnn {

// the alignment of all the allocated buffers
#define MALLOC_ALIGN    32

// Aligns a pointer to the specified number of bytes
// ptr Aligned pointer
// n Alignment size that must be a power of two
template<typename _Tp> static inline _Tp* alignPtr(_Tp* ptr, int n=(int)sizeof(_Tp))
{
    return (_Tp*)(((size_t)ptr + n-1) & -n);
}

// Aligns a buffer size to the specified number of bytes
// The function returns the minimum number that is greater or equal to sz and is divisible by n
// sz Buffer size to align
// n Alignment size that must be a power of two
static inline size_t alignSize(size_t sz, int n)
{
    return (sz + n-1) & -n;
}

static inline void* fastMalloc(size_t size)
{
#ifdef XNN_NONE_EABI
    unsigned char* udata = (unsigned char*)malloc(size + sizeof(void*) + MALLOC_ALIGN);
    //XNN_LOGI("fastMalloc udata=0x%x\n", udata);

    if (!udata)
        return 0;
    unsigned char** adata = alignPtr((unsigned char**)udata + 1, MALLOC_ALIGN);
    //XNN_LOGI("fastMalloc adata=0x%x\n", adata);
    adata[-1] = udata;
    return adata;
#else
    unsigned char* udata = (unsigned char*)malloc(size + sizeof(void*) + MALLOC_ALIGN);
    //XNN_LOGI("fastMalloc udata=0x%x\n", udata);

    if (!udata)
        return 0;
    unsigned char** adata = alignPtr((unsigned char**)udata + 1, MALLOC_ALIGN);
    //XNN_LOGI("fastMalloc adata=0x%x\n", adata);
    adata[-1] = udata;
    return adata;
#endif
}

static inline void fastFree(void* ptr)
{
    if (ptr)
    {
#ifdef XNN_NONE_EABI
        //XNN_LOGI("fastFree ptr=0x%x\n", ptr);
        unsigned char* udata = ((unsigned char**)ptr)[-1];
        //XNN_LOGI("fastFree udata=0x%x\n", udata);
        free(udata);
#else
        //XNN_LOGI("fastFree ptr=0x%x\n", ptr);
        unsigned char* udata = ((unsigned char**)ptr)[-1];
        //XNN_LOGI("fastFree udata=0x%x\n", udata);
        free(udata);
#endif
    }
}

// thread-unsafe branch
static inline int XNN_XADD(int* addr, int delta) { int tmp = *addr; *addr += delta; return tmp; }

class Allocator
{
public:
    virtual ~Allocator() = 0;
    virtual void* fastMalloc(size_t size) = 0;
    virtual void fastFree(void* ptr) = 0;
};

class PoolAllocator : public Allocator
{
public:
    PoolAllocator();
    ~PoolAllocator();

    // ratio range 0 ~ 1
    // default cr = 0.75
    void set_size_compare_ratio(float scr);

    // release all budgets immediately
    void clear();

    virtual void* fastMalloc(size_t size);
    virtual void fastFree(void* ptr);

private:
    Mutex budgets_lock;
    Mutex payouts_lock;
    unsigned int size_compare_ratio;// 0~256
    std::list< std::pair<size_t, void*> > budgets;
    std::list< std::pair<size_t, void*> > payouts;
};

class UnlockedPoolAllocator : public Allocator
{
public:
    UnlockedPoolAllocator();
    ~UnlockedPoolAllocator();

    // ratio range 0 ~ 1
    // default cr = 0.75
    void set_size_compare_ratio(float scr);

    // release all budgets immediately
    void clear();

    virtual void* fastMalloc(size_t size);
    virtual void fastFree(void* ptr);

private:
    unsigned int size_compare_ratio;// 0~256
    std::list< std::pair<size_t, void*> > budgets;
    std::list< std::pair<size_t, void*> > payouts;
};

} // namespace xnn

#endif // XNN_ALLOCATOR_H
