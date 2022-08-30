#ifndef RT_STATIC_HEAP_H
#define RT_STATIC_HEAP_H

#include "layer000/rt_types.h"

/**
 * @file
 * Use process heap under Windows and CRT under Linux.
 *
 * <p>
 * Thread safe if process heap/malloc is which should be the case.
 * </p>
 *
 * @param area RT_NULL in case of error.
 */
void *rt_static_heap_alloc(void **area, rt_un size);

/**
 *
 *
 * @param area Left unchanged in case of failure.
 * @return New area or RT_NULL in case of failure.
 */
void *rt_static_heap_realloc(void **area, rt_un size);

/**
 * <p>
 * Does nothing if <tt>*area</tt> is null.<br>
 * <tt>*area</tt> is always set to null by this function.
 * </p>
 */
rt_s rt_static_heap_free(void **area);

/**
 * Perform an heap allocation only if <tt>buffer</tt> is too small.
 *
 * @param heap_buffer_capacity If not RT_NULL, must be initialized with zero or the size of heap buffer.
 */
void *rt_static_heap_alloc_if_needed(void *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, void **area, rt_un size);

#endif /* RT_STATIC_HEAP_H */
