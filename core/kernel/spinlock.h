#pragma once

typedef struct SpinLock {
    int locked;
} SpinLock;

void lock(SpinLock* lock);
void unlock(SpinLock* lock);
