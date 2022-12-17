#ifndef RT_SORTABLE_ARRAY_H
#define RT_SORTABLE_ARRAY_H

#include "layer000/rt_types.h"
#include "layer002/rt_comparison.h"
#include "layer004/rt_array.h"

struct rt_sortable_array_header {
	rt_comparison_callback_t callback;
	void *context;
	rt_b sorted;
	struct rt_array_header array_header;
};

#define RT_SORTABLE_ARRAY_GET_HEADER(SORTABLE_ARRAY) (struct rt_sortable_array_header*)SORTABLE_ARRAY - 1
#define RT_SORTABLE_ARRAY_GET_CUSTOM_HEADER(SORTABLE_ARRAY, SORTABLE_ARRAY_HEADER) (void*)((rt_uchar8*)SORTABLE_ARRAY - SORTABLE_ARRAY_HEADER->array_header.header_size)

/**
 *
 * <p>
 * The array is marked as sorted only if it is empty.
 * </p>
 */
rt_s rt_sortable_array_create(void **sortable_array, rt_un size, rt_un item_size, rt_un header_size, struct rt_heap *heap, rt_comparison_callback_t callback, void *context);

/**
 * Add a new item to the sortable array, in its right place (if the array is currently sorted).<br>
 * The items located after the insertion point are moved forward.
 *
 * @param item_index Out parameter, the index of the inserted item.
 */
rt_s rt_sortable_array_add_item(void **sortable_array, const void *new_item, rt_un *item_index);

/**
 * Delete an item of the array by overwriting it with the end of the array.<br>
 * The array remains sorted if it was.
 */
rt_s rt_sortable_array_delete_item_index(void **sortable_array, rt_un item_index);

/**
 * Sort the array and mark it as sorted.
 *
 * <p>
 * Can fail only if the comparison callback fails.
 * </p>
 */
rt_s rt_sortable_array_sort(void *sortable_array);

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
rt_s rt_sortable_array_search_item_index(void *sortable_array, const void *item, rt_un *item_index);

#endif /* RT_SORTABLE_ARRAY_H */
