#include "layer005/rt_list.h"

#include "layer001/rt_memory.h"
#include "layer002/rt_error.h"

rt_s rt_list_create(void **list, rt_un size, rt_un item_size, rt_un chunk_size, rt_un header_size, struct rt_heap *heap)
{
	rt_un chunks_count;
	struct rt_list_header *header;
	void **chunks = RT_NULL;
	rt_un chunk_allocation_size = chunk_size * item_size;
	rt_un i;
	rt_s ret;

	*list = RT_NULL;

	if (RT_UNLIKELY(!RT_MEMORY_IS_POWER_OF_TWO(chunk_size))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	chunks_count = RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size);

	if (header_size == 0)
		header_size = sizeof(struct rt_list_header);

	if (RT_UNLIKELY(!rt_array_create(list, chunks_count, sizeof(void*), header_size, heap)))
		goto error;

	chunks = (void**)*list;

	header = RT_LIST_GET_HEADER(*list);
	header->size = size;
	header->item_size = item_size;
	header->chunk_size = chunk_size;

	/* Makes sure that the complete array is clean to ensure avoiding to try to free a garbage pointer in cleanup code. */
	for (i = 0; i < chunks_count; i++)
		chunks[i] = RT_NULL;

	/* Allocate the chunks. */
	for (i = 0; i < chunks_count; i++) {
		if (RT_UNLIKELY(!heap->alloc(heap, &chunks[i], chunk_allocation_size)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	rt_list_free(list);
	ret = RT_FAILED;
	goto free;
}

rt_s rt_list_get_item(void *list, rt_un item_index, void **item)
{
	struct rt_list_header *header;
	rt_un chunk_size;
	rt_un item_size;
	rt_un chunk_index;
	void **chunks = (void**)list;
	rt_uchar8 *chunk;
	rt_un item_index_in_chunk;

	header = RT_LIST_GET_HEADER(list);
	chunk_size = header->chunk_size;
	item_size = header->item_size;

	chunk_index = item_index / chunk_size;
	chunk = (rt_uchar8*)chunks[chunk_index];

	item_index_in_chunk = RT_MEMORY_MODULO_POWER_OF_TWO(item_index, chunk_size);

	*item = chunk + item_index_in_chunk * item_size;

	return RT_OK;
}

rt_s rt_list_set_size(void **list, rt_un size)
{
	struct rt_list_header *header;
	rt_un chunk_size;
	rt_un chunks_count;
	rt_un item_size;
	struct rt_heap *heap;
	rt_un new_chunks_count;
	void **chunks;
	rt_un chunk_allocation_size;
	rt_un i;
	rt_s ret;

	header = RT_LIST_GET_HEADER(*list);
	chunk_size = header->chunk_size;
	chunks_count = header->array_header.size;
	item_size = header->item_size;
	heap = header->array_header.heap;

	new_chunks_count = RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size);
	if (new_chunks_count < chunks_count) {

		chunks = (void**)*list;

		/* Free the excess chunks at the end of the array. */
		/* If it fails, there will be some null pointers in the array. */
		for (i = new_chunks_count; i < chunks_count; i++) {
			if (RT_UNLIKELY(!heap->free(heap, &chunks[i])))
				goto error;
		}

		/* Resize the array. If it fails, there will be some null pointers at the end of the array. */
		if (RT_UNLIKELY(!rt_array_set_size(list, new_chunks_count)))
			goto error;

	} else if (new_chunks_count > chunks_count) {

		/* Attempt to allocate some space at the end of the array. */
		/* If it fails, the array remains the same. */
		if (RT_UNLIKELY(!rt_array_set_size(list, new_chunks_count)))
			goto error;

		chunks = (void**)*list;

		/* Make sure to have null pointers in the array if one of the further allocations fails. */
		for (i = chunks_count; i < new_chunks_count; i++)
			chunks[i] = RT_NULL;

		chunk_allocation_size = chunk_size * item_size;

		/* Try to allocate the chunks. */
		/* If an allocation fails, there will be some null pointers at the end of the array. */
		for (i = chunks_count; i < new_chunks_count; i++) {
			if (RT_UNLIKELY(!heap->alloc(heap, &chunks[i], chunk_allocation_size)))
				goto error;
		}
	}

	/* Put the new size. */
	header = RT_LIST_GET_HEADER(*list);
	header->size = size;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_list_delete_item_index(void **list, rt_un item_index)
{
	struct rt_list_header *header;
	rt_un size;
	rt_un item_size;
	rt_un last_item_index;
	void *item;
	void *last_item;
	rt_s ret;

	header = RT_LIST_GET_HEADER(*list);
	size = header->size;
	item_size = header->item_size;

	last_item_index = size - 1;

	if (item_index != last_item_index) {
		if (RT_UNLIKELY(!rt_list_get_item(*list, item_index, &item)))
			goto error;
		if (RT_UNLIKELY(!rt_list_get_item(*list, last_item_index, &last_item)))
			goto error;

		/* Copy the last item into the item to delete. */
		RT_MEMORY_COPY(last_item, item, item_size);
	}

	if (RT_UNLIKELY(!rt_list_set_size(list, size - 1)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_list_new_item(void **list, void **item)
{
	struct rt_list_header *header;
	rt_un size;
	rt_s ret;

	header = RT_LIST_GET_HEADER(*list);
	size = header->size;

	if (RT_UNLIKELY(!rt_list_set_size(list, size + 1)))
		goto error;

	if (RT_UNLIKELY(!rt_list_get_item(*list, size, item)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_list_new_item_index(void **list, rt_un *item_index)
{
	struct rt_list_header *header;
	rt_un size;
	rt_s ret;

	header = RT_LIST_GET_HEADER(*list);
	size = header->size;

	if (RT_UNLIKELY(!rt_list_set_size(list, size + 1)))
		goto error;

	*item_index = size;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_list_free(void **list)
{
	struct rt_list_header *header;
	rt_un chunks_count;
	struct rt_heap *heap;
	void **chunks;
	rt_un i;
	rt_s ret = RT_OK;

	if (*list) {
		header = RT_LIST_GET_HEADER(*list);
		chunks_count = header->array_header.size;
		heap = header->array_header.heap;

		chunks = (void**)*list;

		/* Free all chunks. */
		for (i = 0; i < chunks_count; i++) {
			if (!heap->free(heap, &chunks[i]))
				ret = RT_FAILED;
		}

		/* Free the array of pointers. */
		if (!rt_array_free(list))
			ret = RT_FAILED;
	}

	return ret;
}
