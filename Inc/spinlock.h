
#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include <cstdint>

namespace OS
{
    class SpinLock
    {
    public:
        SpinLock();
        void Lock();
        void Unlock();
    private:
        std::uint8_t available;
    };
}

#endif