#include "layer005/rt_list.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_list_create(struct rt_list *list, rt_un size, rt_un item_size, rt_un chunk_size, struct rt_heap *heap)
{
	rt_un chunks_count;
	void **chunks;
	rt_un chunk_allocation_size = chunk_size * item_size;
	rt_un i;
	rt_b chunks_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!RT_MEMORY_IS_POWER_OF_TWO(chunk_size))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	list->size = 0;
	list->item_size = item_size;
	list->chunk_size = chunk_size;

	chunks_count = RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size);

	if (RT_UNLIKELY(!rt_array_create(&list->chunks, chunks_count, sizeof(void*), heap)))
		goto end;
	chunks_created = RT_TRUE;

	chunks = (void**)list->chunks.items;

	/* Makes sure that the complete array is clean to ensure avoiding to try to free a garbage pointer in cleanup code. */
	for (i = 0; i < chunks_count; i++)
		chunks[i] = RT_NULL;

	/* Allocate the chunks. */
	for (i = 0; i < chunks_count; i++) {
		if (RT_UNLIKELY(!heap->alloc(heap, &chunks[i], chunk_allocation_size)))
			goto end;
	}

	list->size = size;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret && chunks_created))
		rt_list_free(list);

	return ret;
}

rt_s rt_list_get_item(struct rt_list *list, rt_un item_index, void **item)
{
	rt_un chunk_size = list->chunk_size;
	rt_un chunk_index;
	void **chunks = (void**)list->chunks.items;
	rt_uchar8 *chunk;
	rt_un item_index_in_chunk;

	chunk_index = item_index / chunk_size;
	chunk = (rt_uchar8*)chunks[chunk_index];

	item_index_in_chunk = RT_MEMORY_MODULO_POWER_OF_TWO(item_index, chunk_size);

	*item = chunk + item_index_in_chunk * list->item_size;

	return RT_OK;
}

rt_s rt_list_set_size(struct rt_list *list, rt_un size)
{
	rt_un chunk_size = list->chunk_size;
	rt_un chunks_count = list->chunks.size;
	struct rt_heap *heap = list->chunks.heap;
	rt_un new_chunks_count;
	void **chunks;
	rt_un chunk_allocation_size;
	rt_un i;
	rt_s ret = RT_FAILED;

	new_chunks_count = RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size);
	if (new_chunks_count < chunks_count) {

		chunks = (void**)list->chunks.items;

		/* Free the excess chunks at the end of the array. */
		/* If it fails, there will be some null pointers in the array. */
		for (i = new_chunks_count; i < chunks_count; i++) {
			if (RT_UNLIKELY(!heap->free(heap, &chunks[i])))
				goto end;
		}

		/* Resize the array. If it fails, there will be some null pointers at the end of the array. */
		if (RT_UNLIKELY(!rt_array_set_size(&list->chunks, new_chunks_count)))
			goto end;

	} else if (new_chunks_count > chunks_count) {

		/* Attempt to allocate some space at the end of the array. */
		/* If it fails, the array remains the same. */
		if (RT_UNLIKELY(!rt_array_set_size(&list->chunks, new_chunks_count)))
			goto end;

		chunks = (void**)list->chunks.items;

		/* Make sure to have null pointers in the array if one of the further allocations fails. */
		for (i = chunks_count; i < new_chunks_count; i++)
			chunks[i] = RT_NULL;

		chunk_allocation_size = chunk_size * list->item_size;

		/* Try to allocate the chunks. */
		/* If an allocation fails, there will be some null pointers at the end of the array. */
		for (i = chunks_count; i < new_chunks_count; i++) {
			if (RT_UNLIKELY(!heap->alloc(heap, &chunks[i], chunk_allocation_size)))
				goto end;
		}
	}

	/* Put the new size. */
	list->size = size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_list_delete_item_index(struct rt_list *list, rt_un item_index)
{
	rt_un size = list->size;
	rt_un last_item_index;
	void *item;
	void *last_item;
	rt_s ret = RT_FAILED;

	last_item_index = size - 1;

	if (item_index != last_item_index) {
		if (RT_UNLIKELY(!rt_list_get_item(list, item_index, &item)))
			goto end;
		if (RT_UNLIKELY(!rt_list_get_item(list, last_item_index, &last_item)))
			goto end;

		/* Copy the last item into the item to delete. */
		RT_MEMORY_COPY(last_item, item, list->item_size);
	}

	if (RT_UNLIKELY(!rt_list_set_size(list, size - 1)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_list_new_item(struct rt_list *list, void **item)
{
	rt_un size = list->size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_list_set_size(list, size + 1)))
		goto end;

	if (RT_UNLIKELY(!rt_list_get_item(list, size, item)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_list_new_item_index(struct rt_list *list, rt_un *item_index)
{
	rt_un size = list->size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_list_set_size(list, size + 1)))
		goto end;

	*item_index = size;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_list_free(struct rt_list *list)
{
	rt_un chunks_count = list->chunks.size;
	struct rt_heap *heap = list->chunks.heap;
	void **chunks = (void**)list->chunks.items;
	rt_un i;
	rt_s ret = RT_OK;

	/* Free all chunks. */
	for (i = 0; i < chunks_count; i++) {
		if (!heap->free(heap, &chunks[i]))
			ret = RT_FAILED;
	}

	/* Free the array of pointers. */
	if (!rt_array_free(&list->chunks))
		ret = RT_FAILED;

	list->size = 0;

	return ret;
}
