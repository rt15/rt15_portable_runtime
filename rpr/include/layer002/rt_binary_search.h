#ifndef RT_BINARY_SEARCH_H
#define RT_BINARY_SEARCH_H

#include "layer000/rt_types.h"
#include "layer001/rt_comparison.h"

/**
 * @file
 * Binary search implementation.
 */

/**
 * Search for <tt>item</tt> in area.<br>
 * <tt>area</tt> must be sorted.
 *
 * <p>
 * If <tt>area</tt> contains given <tt>item</tt> several times, the lower index is returned.
 * </p>
 *
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 *
 * @param item_index RT_TYPE_MAX_UN if the item is not found, the item index otherwise.
 */
rt_s rt_binary_search_index(const void *area, const void *item, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context, rt_un *item_index);

/**
 * Search the insertion index corresponding to <tt>item</tt> in <tt>area</tt>.<br>
 * <tt>area</tt> must be sorted.
 *
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 *
 * <p>
 * If <tt>area</tt> contains given <tt>item</tt> several times, the lower index is returned.
 * </p>
 *
 * <p>
 * If <tt>item</tt> is lower than all the values, then zero is returned.<br>
 * If <tt>item</tt> is greater than all the values, then last index plus one is returned.
 * </p>
 *
 * @param item_index The index of searched item or an index to which insert it.
 */
rt_s rt_binary_search_insertion_index(const void *area, const void *item, rt_un size, rt_un item_size, rt_comparison_callback_t callback, void *context, rt_un *item_index);

#endif /* RT_BINARY_SEARCH_H */
