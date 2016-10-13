#include "spinlock.h"

void lock(SpinLock* lock) {
    asm(
        "lock.loop:\n"
        "mov eax, 1\n"
        "xchg eax, [%0]\n"
        "test eax, eax\n"
        "jnz lock.loop\n"
        :: "rm" (lock)
        : "eax"
    );
}

void unlock(SpinLock* lock) {
    lock->locked = 0;
}
