#include "layer005/rt_ring.h"

#include "layer002/rt_error.h"

rt_s rt_ring_create(struct rt_ring *ring, rt_un capacity, rt_un item_size, struct rt_heap *heap)
{
	struct rt_array *array = &ring->array;
	rt_b array_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!capacity)) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	if (RT_UNLIKELY(!rt_array_create(array, capacity, item_size, heap)))
		goto end;
	array_created = RT_TRUE;

	ring->head_index = 0;
	ring->tail_index = 0;
	ring->size = 0;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret && array_created))
		rt_array_free(array);

	return ret;
}

rt_s rt_ring_new_item(struct rt_ring *ring, void **item)
{
	rt_s ret = RT_FAILED;

	if (ring->size == ring->array.size) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto end;
	}

	*item = ((rt_uchar8*)ring->array.items) + ring->head_index * ring->array.item_size;
	ring->head_index++;
	if (ring->head_index >= ring->array.size)
		ring->head_index = 0;

	ring->size++;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_ring_pop_item(struct rt_ring *ring, void **item)
{
	rt_s ret = RT_FAILED;

	if (!ring->size) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	*item = ((rt_uchar8*)ring->array.items) + ring->tail_index * ring->array.item_size;
	ring->tail_index++;
	if (ring->tail_index >= ring->array.size)
		ring->tail_index = 0;

	ring->size--;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_ring_free(struct rt_ring *ring)
{
	rt_s ret = RT_OK;

	if (RT_UNLIKELY(!rt_array_free(&ring->array)))
		ret = RT_FAILED;

	return ret;
}
