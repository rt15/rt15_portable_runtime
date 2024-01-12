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

#define RT_LIST_GET_HEADER(LIST) (struct rt_list_header*)LIST - 1
#define RT_LIST_GET_CUSTOM_HEADER(LIST, LIST_HEADER) (void*)((rt_uchar8*)LIST - LIST_HEADER->array_header.header_size)

struct rt_list_header {
	rt_un size;       /* Total number of items. The array header contains the number of chunks. */
	rt_un item_size;  /* Size of the list items. The array contains pointers to chunks. */
	rt_un chunk_size;
	struct rt_array_header array_header;
};

/**
 * Create a new list using given parameters.
 *
 * @param size Total items count in the list.
 * @param item_size Size of an item.
 * @param chunk_size Maximum items count in a chunk. Must be a power of two.
 * @param header_size Can be zero to use the default header size.
 */
RT_API rt_s rt_list_create(void **list, rt_un size, rt_un item_size, rt_un chunk_size, rt_un header_size, struct rt_heap *heap);

RT_API rt_s rt_list_get_item(void *list, rt_un item_index, void **item);

/**
 * <p>
 * In case of failure, some chunks pointers can be RT_NULL or pointing on allocated data.<br>
 * Chunks not affected by the resizing should be OK.<br>
 * In any case, it is safe to call <tt>rt_list_free</tt> on the resulting list.
 * </p>
 */
RT_API rt_s rt_list_set_size(void **list, rt_un size);

/**
 * This function moves the last item in place of the one to delete.
 * As a result, indexes on the list items might become invalid.
 */
RT_API rt_s rt_list_delete_item_index(void **list, rt_un item_index);

RT_API rt_s rt_list_new_item(void **list, void **item);

RT_API rt_s rt_list_new_item_index(void **list, rt_un *item_index);

/**
 *
 * <p>
 * Set <tt>*list</tt> to RT_NULL.<br>
 * Does nothing if <tt>*list</tt> is RT_NULL.
 * </p>
 */
RT_API rt_s rt_list_free(void **list);

#endif /* RT_LIST_H */
