#ifndef RT_ARRAY_H
#define RT_ARRAY_H

#include "layer000/rt_types.h"
#include "layer003/rt_heap.h"

/**
 * @file
 * Arrays.
 *
 * <p>
 * The items are stored in a single heap area pointed by <tt>items</tt>.<br>
 * <tt>items</tt> may be moved by functions changing the size of the array, so pointers on items must not be kept across such calls.<br>
 * An empty array does not use any heap area: <tt>items</tt> is RT_NULL.
 * </p>
 *
 * <p>
 * To extend the array with additional data, embed <tt>struct rt_array</tt> into another structure.
 * </p>
 */

struct rt_array {
	void *items;
	rt_un size;
	rt_un item_size;
	struct rt_heap *heap;
};

/**
 * Create a new array of specified <tt>size</tt>.
 *
 * <p>
 * If <tt>size</tt> is zero, no allocation is performed.<br>
 * In case of failure, <tt>rt_array_free</tt> must not be called.
 * </p>
 */
RT_API rt_s rt_array_create(struct rt_array *array, rt_un size, rt_un item_size, struct rt_heap *heap);

/**
 * Resize the array.
 *
 * <p>
 * If this function fails, <tt>array</tt> remains untouched.<br>
 * If <tt>size</tt> is zero, the items area is freed and <tt>items</tt> is set to RT_NULL.
 * </p>
 */
RT_API rt_s rt_array_set_size(struct rt_array *array, rt_un size);

/**
 * Prepare an item a the end of the array and return it address.
 */
RT_API rt_s rt_array_new_item(struct rt_array *array, void **item);

/**
 * Prepare an item a the end of the array and return its index.
 */
RT_API rt_s rt_array_new_item_index(struct rt_array *array, rt_un *item_index);

/**
 * This function moves the last item in place of the item to delete.<br>
 * Then it reduces the size of the array.<br>
 * As a result, indexes referencing the array items might become invalid after this function call.
 */
RT_API rt_s rt_array_delete_item_index(struct rt_array *array, rt_un item_index);

/**
 * Retrieve a pointer on the last item.
 *
 * <p>
 * Can be used before <tt>rt_array_delete_last_item</tt> to simulate a pop from a stack.
 * </p>
 */
RT_API rt_s rt_array_get_last_item(struct rt_array *array, void **item);

RT_API rt_s rt_array_get_item(struct rt_array *array, rt_un item_index, void **item);

/**
 * Delete the last item of the array.
 *
 * <p>
 * Can be used after <tt>rt_array_get_last_item</tt> to simulate a pop from a stack.
 * </p>
 */
RT_API rt_s rt_array_delete_last_item(struct rt_array *array);

/**
 * Free the items of provided <tt>array</tt>.
 *
 * <p>
 * <tt>array->items</tt> can be RT_NULL.<br>
 * <tt>array->items</tt> is set to RT_NULL and <tt>array->size</tt> to zero in all cases.<br>
 * As a result, it is safe to call this function several times on the same array.
 * </p>
 */
RT_API rt_s rt_array_free(struct rt_array *array);

#endif /* RT_ARRAY_H */
