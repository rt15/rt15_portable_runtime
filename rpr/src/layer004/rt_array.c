#include "layer004/rt_array.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_array_create(struct rt_array *array, rt_un size, rt_un item_size, struct rt_heap *heap)
{
	rt_s ret = RT_FAILED;

	/* Start with an empty array then let rt_array_set_size allocate the items. */
	array->items = RT_NULL;
	array->size = 0;
	array->item_size = item_size;
	array->heap = heap;

	if (RT_UNLIKELY(!rt_array_set_size(array, size)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_set_size(struct rt_array *array, rt_un size)
{
	struct rt_heap *heap = array->heap;
	rt_s ret = RT_FAILED;

	if (!size) {
		/* An empty array does not use any heap area. */
		if (array->items) {
			if (RT_UNLIKELY(!heap->free(heap, &array->items)))
				goto end;
		}
	} else if (array->items) {
		/* If it fails, items remain untouched. */
		if (RT_UNLIKELY(!heap->realloc(heap, &array->items, size * array->item_size)))
			goto end;
	} else {
		if (RT_UNLIKELY(!heap->alloc(heap, &array->items, size * array->item_size)))
			goto end;
	}

	array->size = size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_new_item(struct rt_array *array, void **item)
{
	rt_un initial_size = array->size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_array_set_size(array, initial_size + 1)))
		goto end;

	*item = ((rt_uchar8*)array->items) + initial_size * array->item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_new_item_index(struct rt_array *array, rt_un *item_index)
{
	rt_un initial_size = array->size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_array_set_size(array, initial_size + 1)))
		goto end;

	*item_index = initial_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_delete_item_index(struct rt_array *array, rt_un item_index)
{
	rt_un last_item_index;
	rt_un item_size;
	void *source;
	void *destination;
	rt_s ret = RT_FAILED;

	/* Check that the item index is within range. */
	if (RT_UNLIKELY(item_index >= array->size)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	last_item_index = array->size - 1;
	item_size = array->item_size;

	/* Copy the last item into the item to delete. */
	if (item_index != last_item_index) {
		source = (rt_uchar8*)array->items + last_item_index * item_size;
		destination = (rt_uchar8*)array->items + item_index * item_size;
		RT_MEMORY_COPY(source, destination, item_size);
	}

	if (RT_UNLIKELY(!rt_array_set_size(array, last_item_index)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_get_last_item(struct rt_array *array, void **item)
{
	rt_s ret = RT_FAILED;

	/* Makes sure there is something to return. */
	if (RT_UNLIKELY(!array->size)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	*item = ((rt_uchar8*)array->items) + (array->size - 1) * array->item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_get_item(struct rt_array *array, rt_un item_index, void **item)
{
	rt_s ret = RT_FAILED;

	/* Check that the item index is within range. */
	if (RT_UNLIKELY(item_index >= array->size)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	*item = ((rt_uchar8*)array->items) + item_index * array->item_size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_delete_last_item(struct rt_array *array)
{
	rt_s ret = RT_FAILED;

	/* Makes sure there is something to delete. */
	if (RT_UNLIKELY(!array->size)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	if (RT_UNLIKELY(!rt_array_set_size(array, array->size - 1)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_array_free(struct rt_array *array)
{
	struct rt_heap *heap;
	rt_s ret = RT_FAILED;

	array->size = 0;

	if (array->items) {
		/* Set items to RT_NULL in all cases. */
		heap = array->heap;
		if (RT_UNLIKELY(!heap->free(heap, &array->items)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}
