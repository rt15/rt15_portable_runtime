#ifndef RT_LIST_H
#define RT_LIST_H

#include "layer000/rt_types.h"
#include "layer003/rt_heap.h"
#include "layer004/rt_array.h"

/**
 * @file
 * A list is an array of pointers on chunks.<br>
 * Instead of storing items in a single huge array, they are stored in many "small" chunks.<br>
 * It permits to add new items quickly as there is no need to copy all existing items in a new array if
 * the current one is full.
 */

struct rt_list {
	rt_un size;            /* Total number of items. The chunks array contains the number of chunks. */
	rt_un item_size;       /* Size of the list items. The chunks array contains pointers to chunks. */
	rt_un chunk_size;
	struct rt_array chunks;
};

/**
 * Create a new list using given parameters.
 *
 * <p>
 * In case of failure, everything is cleaned up and <tt>rt_list_free</tt> must not be called.
 * </p>
 *
 * @param size Total items count in the list.
 * @param item_size Size of an item.
 * @param chunk_size Maximum items count in a chunk. Must be a power of two.
 */
RT_API rt_s rt_list_create(struct rt_list *list, rt_un size, rt_un item_size, rt_un chunk_size, struct rt_heap *heap);

RT_API rt_s rt_list_get_item(struct rt_list *list, rt_un item_index, void **item);

/**
 * <p>
 * In case of failure, some chunks pointers can be RT_NULL or pointing on allocated data.<br>
 * Chunks not affected by the resizing should be OK.<br>
 * In any case, it is safe to call <tt>rt_list_free</tt> on the resulting list.
 * </p>
 */
RT_API rt_s rt_list_set_size(struct rt_list *list, rt_un size);

/**
 * This function moves the last item in place of the one to delete.
 * As a result, indexes on the list items might become invalid.
 */
RT_API rt_s rt_list_delete_item_index(struct rt_list *list, rt_un item_index);

RT_API rt_s rt_list_new_item(struct rt_list *list, void **item);

RT_API rt_s rt_list_new_item_index(struct rt_list *list, rt_un *item_index);

/**
 *
 * <p>
 * Free the chunks and the chunks array.<br>
 * It is safe to call this function several times on the same list.
 * </p>
 */
RT_API rt_s rt_list_free(struct rt_list *list);

#endif /* RT_LIST_H */
