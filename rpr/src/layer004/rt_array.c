#include "layer004/rt_array.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_array_create(void **array, rt_un size, rt_un item_size, rt_un header_size, struct rt_heap *heap)
{
	struct rt_array_header *header;
	void *area;

	if (!header_size)
		header_size = sizeof(struct rt_array_header);

	if (heap->alloc(heap, &area, header_size + size * item_size)) {

		/* Make the array point after the custom header. */
		*array = (rt_uchar8*)area + header_size;

		header = RT_ARRAY_GET_HEADER(*array);
		header->size = size;
		header->item_size = item_size;
		header->header_size = header_size;
		header->heap = heap;
	} else {
		*array = RT_NULL;
	}
	return (*array) ? RT_OK : RT_FAILED;
}

rt_s rt_array_set_size(void **array, rt_un size)
{
	struct rt_array_header *header;
	void *area;
	rt_un header_size;
	struct rt_heap *heap;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(*array);
	area = RT_ARRAY_GET_CUSTOM_HEADER(*array, header);

	header_size = header->header_size;
	heap = header->heap;

	if (RT_UNLIKELY(!heap->realloc(heap, &area, header_size + size * header->item_size)))
		goto end;

	/* Make the array point after the custom header. */
	*array = (rt_uchar8*)area + header_size;

	header = RT_ARRAY_GET_HEADER(*array);
	header->size = size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_new_item(void **array, void **item)
{
	struct rt_array_header *header;
	rt_un initial_size;
	rt_un item_size;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(*array);
	initial_size = header->size;
	item_size = header->item_size;

	if (RT_UNLIKELY(!rt_array_set_size(array, initial_size + 1)))
		goto end;

	*item = ((rt_uchar8*)*array) + initial_size * item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_new_item_index(void **array, rt_un *item_index)
{
	struct rt_array_header *header;
	rt_un initial_size;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(*array);
	initial_size = header->size;

	if (RT_UNLIKELY(!rt_array_set_size(array, initial_size + 1)))
		goto end;

	*item_index = initial_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_delete_item_index(void **array, rt_un item_index)
{
	struct rt_array_header *header;
	rt_un last_item_index;
	rt_un item_size;
	void *source;
	void *destination;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(*array);
	last_item_index = header->size - 1;
	item_size = header->item_size;

	/* Check that the item index is within range. */
	if (RT_UNLIKELY(item_index >= header->size)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	/* Copy the last item into the item to delete. */
	if (item_index != last_item_index) {
		source = (rt_uchar8*)*array + last_item_index * item_size;
		destination = (rt_uchar8*)*array + item_index * item_size;
		RT_MEMORY_COPY(source, destination, item_size);
	}

	if (RT_UNLIKELY(!rt_array_set_size(array, last_item_index)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_get_last_item(void *array, void **item)
{
	struct rt_array_header *header;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(array);

	/* Makes sure there is something to return. */
	if (RT_UNLIKELY(!header->size)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	*item = ((rt_uchar8*)array) + (header->size - 1) * header->item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_get_item(void *array, rt_un item_index, void **item)
{
	struct rt_array_header *header;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(array);

	/* Check that the item index is within range. */
	if (RT_UNLIKELY(item_index >= header->size)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	*item = ((rt_uchar8*)array) + item_index * header->item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_delete_last_item(void **array)
{
	struct rt_array_header *header;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(*array);

	/* Makes sure there is something to delete. */
	if (RT_UNLIKELY(!header->size)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	if (RT_UNLIKELY(!rt_array_set_size(array, header->size - 1)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_free(void **array)
{
	struct rt_array_header *header;
	void *area;
	struct rt_heap *heap;
	rt_s ret = RT_FAILED;

	if (*array) {
		header = RT_ARRAY_GET_HEADER(*array);
		area = RT_ARRAY_GET_CUSTOM_HEADER(*array, header);

		*array = RT_NULL;

		heap = header->heap;
		if (RT_UNLIKELY(!heap->free(heap, &area)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}
