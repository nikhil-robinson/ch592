#include "ch_spinlock.h"

// Initialize the spinlock
void spinlock_init(spinlock_t *lock) {
    atomic_flag_clear(&lock->flag);
}

// Acquire the spinlock
void spinlock_lock(spinlock_t *lock) {
    while (atomic_flag_test_and_set(&lock->flag)) {
        // Busy-wait (spin) until the lock is released
    }
}

// Release the spinlock
void spinlock_unlock(spinlock_t *lock) {
    atomic_flag_clear(&lock->flag);
}
