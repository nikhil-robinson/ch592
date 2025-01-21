#pragma once

#include "CH59x_common.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "ch_log.h"
#include "ch_err.h"
#include "ch_check.h"

#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define the spinlock structure
typedef struct {
    atomic_flag flag;
} spinlock_t;

// Function prototypes
void spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#define mutexENTER_CRITICAL(mux)  spinlock_lock(mux)
#define mutexEXIT_CRITICAL(mux)   spinlock_unlock(mux)

#ifdef __cplusplus
}
#endif
