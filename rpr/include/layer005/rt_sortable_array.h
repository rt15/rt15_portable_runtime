#ifndef RT_SORTABLE_ARRAY_H
#define RT_SORTABLE_ARRAY_H

#include "layer000/rt_types.h"
#include "layer001/rt_comparison.h"
#include "layer004/rt_array.h"

struct rt_sortable_array {
	rt_comparison_callback_t callback;
	void *context;
	rt_b sorted;
	struct rt_array array;
};

/**
 *
 * <p>
 * The array is marked as sorted only if it is empty.
 * </p>
 *
 * <p>
 * In case of failure, <tt>rt_sortable_array_free</tt> must not be called.
 * </p>
 */
RT_API rt_s rt_sortable_array_create(struct rt_sortable_array *sortable_array, rt_un size, rt_un item_size, struct rt_heap *heap, rt_comparison_callback_t callback, void *context);

/**
 * Add a new item to the sortable array, in its right place (if the array is currently sorted).<br>
 * The items located after the insertion point are moved forward.
 *
 * @param item_index Out parameter, the index of the inserted item.
 */
RT_API rt_s rt_sortable_array_add_item(struct rt_sortable_array *sortable_array, const void *new_item, rt_un *item_index);

/**
 * Delete an item of the array by overwriting it with the end of the array.<br>
 * The array remains sorted if it was.
 */
RT_API rt_s rt_sortable_array_delete_item_index(struct rt_sortable_array *sortable_array, rt_un item_index);

/**
 * Sort the array and mark it as sorted.
 *
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 */
RT_API rt_s rt_sortable_array_sort(struct rt_sortable_array *sortable_array);

/**
 * Search <tt>item</tt> in <tt>sortable_array</tt>.<br>
 * Sort the array if it is not already sorted.
 *
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 *
 * @param item_index RT_TYPE_MAX_UN if the item is not found, the item index otherwise.
 */
RT_API rt_s rt_sortable_array_search_item_index(struct rt_sortable_array *sortable_array, const void *item, rt_un *item_index);

/**
 * Same as <tt>rt_array_free</tt>.
 *
 * <p>
 * It is safe to call this function several times on the same sortable array.
 * </p>
 */
RT_API rt_s rt_sortable_array_free(struct rt_sortable_array *sortable_array);

#endif /* RT_SORTABLE_ARRAY_H */
