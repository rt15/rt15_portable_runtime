#ifndef RT_VIRTUAL_MEMORY_H
#define RT_VIRTUAL_MEMORY_H

#include "layer000/rt_types.h"

/**
 * @file
 * Operating systems expose memory as pages.<br>
 * Heaps implementation (malloc...) are usually requesting pages internally.
 */

/**
 * Retrieve the size of a page, in bytes.<br>
 * Usually 4096 on Windows.
 */
rt_un rt_virtual_memory_get_page_size();

/**
 * The resulting memory is page aligned.
 *
 * @param area RT_NULL in case of error.
 */
void *rt_virtual_memory_alloc(void **area, rt_un size);

/**
 * @param area The pointer is set to RT_NULL by this function.
 * @param size The <tt>size</tt> used in <tt>rt_virtual_memory_alloc</tt>.
 */
rt_s rt_virtual_memory_free(void **area, rt_un size);

#endif /* RT_VIRTUAL_MEMORY_H */
