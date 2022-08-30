#ifndef RT_HEAP_H
#define RT_HEAP_H

#include "layer000/rt_types.h"

struct rt_heap;

/**
 * @file
 * Declares the interface of heaps.
 */

/**
 * @param area RT_NULL in case of error. Out parameter.
 * @return *area.
 */
typedef void *(*rt_heap_alloc_t)(struct rt_heap *heap, void **area, rt_un size);

/**
 * <p>
 * If <tt>realloc</tt> fails, area is not deallocated and its content remains unchanged.
 * </p>
 *
 * @param area Not modified in case of error.
 * @return Address of new area (*area) or RT_NULL in case of failure.
 */
typedef void *(*rt_heap_realloc_t)(struct rt_heap *heap, void **area, rt_un size);

/**
 *
 * <p>
 * area is set to RT_NULL in all cases.
 * </p>
 *
 */
typedef rt_s (*rt_heap_free_t)(struct rt_heap *heap, void **area);

typedef rt_s (*rt_heap_close_t)(struct rt_heap *heap);

struct rt_heap {
	rt_heap_alloc_t alloc;
	rt_heap_realloc_t realloc;
	rt_heap_free_t free;
	rt_heap_close_t close;
};

#endif /* RT_HEAP_H */
