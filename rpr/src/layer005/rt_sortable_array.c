#include "layer005/rt_sortable_array.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_binary_search.h"
#include "layer002/rt_quick_sort.h"

rt_s rt_sortable_array_create(struct rt_sortable_array *sortable_array, rt_un size, rt_un item_size, struct rt_heap *heap, rt_comparison_callback_t callback, void *context)
{
	rt_s ret = RT_FAILED;

	sortable_array->callback = callback;
	sortable_array->context = context;
	if (size) {
		/* The array is not initialized, we do not consider it sorted. */
		sortable_array->sorted = RT_FALSE;
	} else {
		/* The array is empty, we consider it sorted. */
		sortable_array->sorted = RT_TRUE;
	}

	if (RT_UNLIKELY(!rt_array_create(&sortable_array->array, size, item_size, heap)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_sortable_array_add_item(struct rt_sortable_array *sortable_array, const void *new_item, rt_un *item_index)
{
	rt_un new_item_index;
	rt_un size = sortable_array->array.size;
	rt_un item_size = sortable_array->array.item_size;
	rt_un insertion_index;
	void *insertion_address;
	void *after_insertion_address;
	rt_s ret = RT_FAILED;

	if (!size || !sortable_array->sorted) {
		/* Allocate a new item. */
		if (RT_UNLIKELY(!rt_array_new_item_index(&sortable_array->array, &new_item_index)))
			goto end;

		/* Put the new item at the end of the array. */
		RT_MEMORY_COPY(new_item, (rt_char8*)sortable_array->array.items + new_item_index * item_size, item_size);
		*item_index = new_item_index;
	} else {
		/* Search for the insertion index. */
		if (RT_UNLIKELY(!rt_binary_search_insertion_index(sortable_array->array.items, new_item, size, item_size, sortable_array->callback, sortable_array->context, &insertion_index)))
			goto end;

		/* Allocate a new item at the end of the array. */
		if (RT_UNLIKELY(!rt_array_new_item_index(&sortable_array->array, &new_item_index)))
			goto end;
		size++;

		/* Compute insertion address. */
		insertion_address = ((rt_uchar8*)sortable_array->array.items) + item_size * insertion_index;

		/* If an hole is necessary. */
		if (insertion_index < size - 1) {
			/* Create an hole at insertion point. */
			after_insertion_address = (rt_uchar8*)insertion_address + item_size;
			RT_MEMORY_MOVE(insertion_address, after_insertion_address, (size - (insertion_index + 1)) * item_size);
		}
		/* Fill the hole. */
		RT_MEMORY_COPY(new_item, insertion_address, item_size);

		*item_index = insertion_index;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_sortable_array_delete_item_index(struct rt_sortable_array *sortable_array, rt_un item_index)
{
	rt_un size;
	rt_un item_size;
	void *source;
	void *destination;
	rt_s ret = RT_FAILED;

	if (sortable_array->sorted) {

		size = sortable_array->array.size;

		/* If it is not the last item of the array. */
		if (item_index < size - 1) {
			/* Overwrite the item to delete with the end of the array. */
			item_size = sortable_array->array.item_size;
			source = ((rt_uchar8*)sortable_array->array.items) + item_size * (item_index + 1);
			destination = ((rt_uchar8*)sortable_array->array.items) + item_size * item_index;
			RT_MEMORY_MOVE(source, destination, item_size * (size - (item_index + 1)));
		}

		if (RT_UNLIKELY(!rt_array_set_size(&sortable_array->array, size - 1)))
			goto end;

	} else {

		/* Array is not sorted, simply delete it with the classical function. */
		if (RT_UNLIKELY(!rt_array_delete_item_index(&sortable_array->array, item_index)))
			goto end;

	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_sortable_array_sort(struct rt_sortable_array *sortable_array)
{
	rt_s ret = RT_FAILED;

	if (!sortable_array->sorted) {
		if (RT_UNLIKELY(!rt_quick_sort(sortable_array->array.items, sortable_array->array.size, sortable_array->array.item_size, sortable_array->callback, sortable_array->context)))
			goto end;
		sortable_array->sorted = RT_TRUE;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_sortable_array_search_item_index(struct rt_sortable_array *sortable_array, const void *item, rt_un *item_index)
{
	rt_s ret = RT_FAILED;

	if (!sortable_array->sorted) {
		if (RT_UNLIKELY(!rt_sortable_array_sort(sortable_array)))
			goto end;
	}

	if (RT_UNLIKELY(!rt_binary_search_index(sortable_array->array.items, item, sortable_array->array.size, sortable_array->array.item_size, sortable_array->callback, sortable_array->context, item_index)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_sortable_array_free(struct rt_sortable_array *sortable_array)
{
	return rt_array_free(&sortable_array->array);
}
