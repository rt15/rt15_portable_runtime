#ifndef RT_ATOMIC_H
#define RT_ATOMIC_H

#include "layer000/rt_types.h"

/**
 * @file
 * Atomic memory access operations.
 *
 * <p>
 * In a multithreaded context, memory content can be seen differently between threads.<br>
 * A thread can have written at an address but others threads could still read the old value.<br>
 * This can be due to aggressive optimizations (Instructions re-ordering) or the presence of several processors caches.<br>
 * To prevent this issue memory barriers must be used. There are two types or memory barriers:<br>
 * </p>
 * <ul>
 * <li>Software memory barriers for compiler re-ordering.</li>
 * <li>Hardware memory barriers for processor (Fence).</li>
 * </ul>
 *
 * <p>
 * Beware that variables for these functions must be aligned!
 * </p>
 *
 * <p>
 * Compilers provide built-in functions that implements barriers.<br>
 * For atomic operations, barriers that are both hardware and software must be used.
 * </p>
 *
 * <p>
 * In case of undefined reference to __syncXXXX error, add -march=i486 to compiler option.
 * </p>
 *
 * <p>
 * While compiling for 32 bits processor, __syncXXXX obviously does not support atomic operations on 64 bits.<br>
 * As a consequence RT_ATOMIC_LOAD64 and so on cannot exist.
 * </p>
 *
 * <p>
 * This header has no associated .c file.
 * </p>
 */

#ifdef RT_DEFINE_GCC

/* Ensure that value just written by another thread using RT_ATOMIC_STORE will be read. */
#define RT_ATOMIC_LOAD(VAR) __sync_fetch_and_or(&VAR, 0)

/* Ensure that other threads will read written value if they use RT_ATOMIC_LOAD. */
/* We must use __sync_synchronize as __sync_lock_test_and_set doesn't implement a full memory barrier. */
#define RT_ATOMIC_STORE(VAR, VALUE) __sync_synchronize(); __sync_lock_test_and_set(&VAR, VALUE)

/* Return 1 to first thread and zero to others. */
#define RT_ATOMIC_TRY_TO_ACQUIRE(VAR) !__sync_val_compare_and_swap(&VAR, 0, 1)

/* Restore a token acquired with RT_ATOMIC_TRY_TO_ACQUIRE to zero. */

/* 32 bits versions. The gcc built-in functions are type size independent. */
#define RT_ATOMIC_LOAD32 RT_ATOMIC_LOAD
#define RT_ATOMIC_STORE32 RT_ATOMIC_STORE
#define RT_ATOMIC_TRY_TO_ACQUIRE32 RT_ATOMIC_TRY_TO_ACQUIRE

#else

/* Define 32 bits version. */
#define RT_ATOMIC_LOAD32(VAR) _InterlockedOr((rt_n32*)&VAR, 0)
#define RT_ATOMIC_STORE32(VAR, VALUE) _InterlockedExchange((rt_n32*)&VAR, VALUE)
#define RT_ATOMIC_TRY_TO_ACQUIRE32(VAR) !_InterlockedCompareExchange((rt_n32*)&VAR, 1, 0)

#ifdef RT_DEFINE_64

/* Use 64 bits implementation. */
#define RT_ATOMIC_LOAD(VAR) _InterlockedOr64(&VAR, 0)
#define RT_ATOMIC_STORE(VAR, VALUE) _InterlockedExchange64(&VAR, VALUE)
#define RT_ATOMIC_TRY_TO_ACQUIRE(VAR) !_InterlockedCompareExchange64(&VAR, 1, 0)

#else /* NOT RT_DEFINE_64 */

/* Use 32 bits version as normal version. */
#define RT_ATOMIC_LOAD RT_ATOMIC_LOAD32
#define RT_ATOMIC_STORE RT_ATOMIC_STORE32
#define RT_ATOMIC_TRY_TO_ACQUIRE RT_ATOMIC_TRY_TO_ACQUIRE32

#endif

#endif

#define RT_ATOMIC_RELEASE(VAR) RT_ATOMIC_STORE(VAR, 0);
#define RT_ATOMIC_RELEASE32(VAR) RT_ATOMIC_STORE32(VAR, 0);

#endif /* RT_ATOMIC_H */
