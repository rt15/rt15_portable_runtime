#ifndef RT_PAGE_HEAP_H
#define RT_PAGE_HEAP_H

#include "layer000/rt_types.h"
#include "layer001/rt_heap.h"

/**
 * @file
 * Provide a heap based on <tt>rt_virtual_memory</tt>.
 *
 * <p>
 * Beware that the allocated areas are not aligned with pages boundaries.<br>
 * To obtain aligned pages, use directly <tt>rt_virtual_memory</tt>.
 * </p>
 *
 * <p>
 * Also provided static functions.
 * </p>
 */

/**
 * Heap working with pages.
 */
struct rt_page_heap {
	struct rt_heap heap;
};

rt_s rt_page_heap_create(struct rt_page_heap *page_heap);

void *rt_page_heap_alloc(void **area, rt_un size);

/**
 * Do nothing if the old area has enough pages to store given <tt>size</tt>.
 *
 * @return Allocated area of RT_NULL in case of error.
 */
void *rt_page_heap_realloc(void **area, rt_un size);


rt_s rt_page_heap_free(void **area);

#endif /* RT_PAGE_HEAP_H */
