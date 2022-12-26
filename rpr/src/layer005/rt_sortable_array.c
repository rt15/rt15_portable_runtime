#include "layer005/rt_sortable_array.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_binary_search.h"
#include "layer002/rt_quick_sort.h"

rt_s rt_sortable_array_create(void **sortable_array, rt_un size, rt_un item_size, rt_un header_size, struct rt_heap *heap, rt_comparison_callback_t callback, void *context)
{
	struct rt_sortable_array_header *header;
	rt_s ret;

	if (!header_size)
		header_size = sizeof(struct rt_sortable_array_header);

	if (!rt_array_create(sortable_array, size, item_size, header_size, heap))
		goto error;

	header = RT_SORTABLE_ARRAY_GET_HEADER(*sortable_array);
	header->callback = callback;
	header->context = context;
	if (size) {
		/* The array is not initialized, we do not consider it sorted. */
		header->sorted = RT_FALSE;
	} else {
		/* The array is empty, we consider it sorted. */
		header->sorted = RT_TRUE;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_sortable_array_add_item(void **sortable_array, const void *new_item, rt_un *item_index)
{
	struct rt_sortable_array_header *header;
	rt_un new_item_index;
	rt_un size;
	rt_un item_size;
	rt_comparison_callback_t callback;
	void *context;
	rt_un insertion_index;
	void *insertion_address;
	void *after_insertion_address;
	rt_s ret;

	header = RT_SORTABLE_ARRAY_GET_HEADER(*sortable_array);
	size = header->array_header.size;
	item_size = header->array_header.item_size;

	if (!size || !header->sorted) {
		/* Allocate a new item. */
		if (!rt_array_new_item_index(sortable_array, &new_item_index))
			goto error;

		/* Put the new item at the end of the array. */
		RT_MEMORY_COPY(new_item, (rt_char8*)*sortable_array + new_item_index * item_size, item_size);
		*item_index = new_item_index;
	} else {
		/* Search for the insertion index. */
		callback = header->callback;
		context = header->context;
		if (!rt_binary_search_insertion_index(*sortable_array, new_item, size, item_size, callback, context, &insertion_index))
			goto error;

		/* Allocate a new item at the end of the array. */
		if (!rt_array_new_item_index(sortable_array, &new_item_index))
			goto error;
		size++;

		/* Compute insertion address. */
		insertion_address = ((rt_uchar8*)*sortable_array) + item_size * insertion_index;

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
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_sortable_array_delete_item_index(void **sortable_array, rt_un item_index)
{
	struct rt_sortable_array_header *header;
	rt_un size;
	rt_un item_size;
	void *source;
	void *destination;
	rt_s ret;

	header = RT_SORTABLE_ARRAY_GET_HEADER(*sortable_array);

	if (header->sorted) {

		size = header->array_header.size;

		/* If it is not the last item of the array. */
		if (item_index < size - 1) {
			/* Overwrite the item to delete with the end of the array. */
			item_size = header->array_header.item_size;
			source = ((rt_uchar8*)*sortable_array) + item_size * (item_index + 1);
			destination = ((rt_uchar8*)*sortable_array) + item_size * item_index;
			RT_MEMORY_MOVE(source, destination, item_size * (size - (item_index + 1)));
		}

		if (!rt_array_set_size(sortable_array, size - 1))
			goto error;

	} else {

		/* Array is not sorted, simply remove it with the classical function. */
		if (!rt_array_delete_item_index(sortable_array, item_index))
			goto error;

	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_sortable_array_sort(void *sortable_array)
{
	struct rt_sortable_array_header *header;
	rt_s ret;

	header = RT_SORTABLE_ARRAY_GET_HEADER(sortable_array);

	if (!header->sorted) {
		if (!rt_quick_sort(sortable_array, header->array_header.size, header->array_header.item_size, header->callback, header->context))
			goto error;
		header->sorted = RT_TRUE;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_sortable_array_search_item_index(void *sortable_array, const void *item, rt_un *item_index)
{
	struct rt_sortable_array_header *header;
	rt_s ret;

	header = RT_SORTABLE_ARRAY_GET_HEADER(sortable_array);

	if (!header->sorted) {
		if (!rt_sortable_array_sort(sortable_array))
			goto error;
	}

	if (!rt_binary_search_index(sortable_array, item, header->array_header.size, header->array_header.item_size, header->callback, header->context, item_index))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
