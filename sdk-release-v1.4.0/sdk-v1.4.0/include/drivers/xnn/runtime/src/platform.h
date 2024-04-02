#ifndef XNN_PLATFORM_H
#define XNN_PLATFORM_H

#define XNN_STDIO 1
#define XNN_PIXEL 1
#define XNN_PIXEL_ROTATE 0
#ifdef USE_FACELOCK_V2 // joey: temp disable xnn log for facelock_v2
#define XNN_LOG 0
#else
#define XNN_LOG 1
#endif
#if !XNN_SIM
#define XNN_OMP 0
#else
#define XNN_OMP 1
#endif

namespace xnn {

class Mutex
{
public:
    Mutex() {}
    ~Mutex() {}
    void lock() {}
    void unlock() {}
};

class MutexLockGuard
{
public:
    MutexLockGuard(Mutex& _mutex) : mutex(_mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }
private:
    Mutex& mutex;
};

} // namespace xnn

#endif // XNN_PLATFORM_H
