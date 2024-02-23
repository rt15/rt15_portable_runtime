#ifndef RT_SPINLOCK_H
#define RT_SPINLOCK_H

#include "layer000/rt_types.h"
#include "layer001/rt_atomic.h"
#include "layer002/rt_sleep.h"

#define RT_SPINLOCK_ACQUIRE(VAR) while (!RT_ATOMIC_TRY_TO_ACQUIRE(VAR)) { rt_sleep_yield(); };
#define RT_SPINLOCK_RELEASE(VAR) RT_ATOMIC_RELEASE(VAR);

#define RT_SPINLOCK_ACQUIRE32(VAR) while (!RT_ATOMIC_TRY_TO_ACQUIRE32(VAR)) { rt_sleep_yield(); };
#define RT_SPINLOCK_RELEASE32(VAR) RT_ATOMIC_RELEASE32(VAR);

#endif /* RT_SPINLOCK_H */
