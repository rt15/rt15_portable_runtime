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

/**
 * Perform an heap allocation only if <tt>buffer</tt> and <tt>heap_buffer</tt> are too small.<br>
 * <tt>heap_buffer</tt> can be reused if already allocated.
 *
 * <p>
 * If <tt>heap_buffer</tt> is bigger than the requested size, then it is kept (a smaller area is not allocated).<br>
 * Also, the bigger size is kept in <tt>heap_buffer_capacity</tt>.
 * </p>
 *
 * @param buffer Can be RT_NULL if <tt>buffer_capacity</tt> is zero. If so, <tt>heap_buffer</tt> will be used.
 * @param heap_buffer Can be RT_NULL if <tt>heap_buffer_capacity</tt> is RT_NULL or zero and <tt>heap</tt> is RT_NULL. *heap_buffer can be RT_NULL or point on an already allocated heap memory.
 * @param heap_buffer_capacity Can be RT_NULL as <tt>buffer</tt> can be enough. Otherwise point on the size of *heap_buffer. Updated with the size of *heap_buffer if it is allocated/reallocated.
 * @param area The resulting area. <tt>buffer</tt> if it is enough, an area allocated using <tt>heap</tt> otherwise (Same as heap_buffer).
 * @param heap Can be <tt>RT_NULL</tt>. In that case we rely only on <tt>buffer</tt>
 * @return <tt>*area</tt>.
 */
void *rt_heap_alloc_if_needed(void *buffer, rt_un buffer_capacity, void **heap_buffer, rt_un *heap_buffer_capacity, void **area, rt_un size, struct rt_heap *heap);

#endif /* RT_HEAP_H */
