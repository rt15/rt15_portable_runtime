#ifndef RT_ARRAY_H
#define RT_ARRAY_H

#include "layer000/rt_types.h"
#include "layer003/rt_heap.h"

/**
 * @file
 * Arrays.
 *
 * <p>
 * When using a custom header, it must ends with <tt>struct rt_array_header</tt> structure, and <tt>header_size</tt> must be given to the sizeof the new structure.
 * </p>
 */

struct rt_array_header {
	rt_un size;
	rt_un item_size;
	rt_un header_size;
	struct rt_heap *heap;
};

#define RT_ARRAY_GET_HEADER(ARRAY) (struct rt_array_header*)ARRAY - 1
#define RT_ARRAY_GET_CUSTOM_HEADER(ARRAY, ARRAY_HEADER) (void*)((rt_uchar8*)ARRAY - ARRAY_HEADER->header_size)

/**
 * Create a new array of specified <tt>size</tt>.
 *
 * <p>
 * <tt>header_size</tt> can be 0 to use default header.
 * </p>
 *
 * @param header_size The total size of the custom header ending with struct rt_array_header.
 * @return RT_NULL in case of error.
 */
RT_API rt_s rt_array_create(void **array, rt_un size, rt_un item_size, rt_un header_size, struct rt_heap *heap);

/**
 * Resize the array.
 *
 * <p>
 * If this function fails, <tt>array</tt> remains untouched.
 * </p>
 */
RT_API rt_s rt_array_set_size(void **array, rt_un size);

/**
 * Prepare an item a the end of the array and return it address.
 */
RT_API rt_s rt_array_new_item(void **array, void **item);

/**
 * Prepare an item a the end of the array and return its index.
 */
RT_API rt_s rt_array_new_item_index(void **array, rt_un *item_index);

/**
 * This function moves the last item in place of the item to delete.<br>
 * Then it reduces the size of the array.<br>
 * As a result, indexes referencing the array items might become invalid after this function call.
 */
RT_API rt_s rt_array_delete_item_index(void **array, rt_un item_index);

/**
 * Retrieve a pointer on the last item.
 *
 * <p>
 * Can be used before <tt>rt_array_delete_last_item</tt> to simulate a pop from a stack.
 * </p>
 */
RT_API rt_s rt_array_get_last_item(void *array, void **item);

RT_API rt_s rt_array_get_item(void *array, rt_un item_index, void **item);

/**
 * Delete the last item of the array.
 *
 * <p>
 * Can be used after <tt>rt_array_get_last_item</tt> to simulate a pop from a stack.
 * </p>
 */
RT_API rt_s rt_array_delete_last_item(void **array);

/**
 * Free provided <tt>array</tt>.
 *
 * <p>
 * <tt>*array</tt> can be RT_NULL.<br>
 * <tt>*array</tt> is set to RT_NULL in all cases.
 * </p>
 */
RT_API rt_s rt_array_free(void **array);

#endif /* RT_ARRAY_H */
