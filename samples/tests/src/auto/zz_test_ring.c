#include <rpr.h>

static rt_s zz_test_ring_check_ring(struct rt_ring *ring, rt_un head_index, rt_un tail_index, rt_un size)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(ring->array.size != 3)) goto end;
	if (RT_UNLIKELY(ring->array.item_size != 4)) goto end;

	if (RT_UNLIKELY(ring->head_index != head_index)) goto end;
	if (RT_UNLIKELY(ring->tail_index != tail_index)) goto end;
	if (RT_UNLIKELY(ring->size != size)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_ring_do(struct rt_heap *heap)
{
	struct rt_ring ring;
	rt_b ring_created = RT_FALSE;
	rt_un32 *item;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_ring_create(&ring, 3, sizeof(rt_un32), heap)))
		goto end;
	ring_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 0, 0, 0)))
		goto end;

	if (RT_UNLIKELY(!rt_ring_new_item(&ring, (void**)&item))) goto end;
	*item = 1;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 1, 0, 1))) goto end;

	if (RT_UNLIKELY(!rt_ring_new_item(&ring, (void**)&item))) goto end;
	*item = 2;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 2, 0, 2))) goto end;

	if (RT_UNLIKELY(!rt_ring_new_item(&ring, (void**)&item))) goto end;
	*item = 3;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 0, 0, 3))) goto end;

	/* New item on a full ring. */
	if (RT_UNLIKELY(rt_ring_new_item(&ring, (void**)&item))) goto end;

	if (RT_UNLIKELY(!rt_ring_pop_item(&ring, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 1)) goto end;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 0, 1, 2))) goto end;

	if (RT_UNLIKELY(!rt_ring_pop_item(&ring, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 2)) goto end;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 0, 2, 1))) goto end;

	if (RT_UNLIKELY(!rt_ring_new_item(&ring, (void**)&item))) goto end;
	*item = 4;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 1, 2, 2))) goto end;

	if (RT_UNLIKELY(!rt_ring_pop_item(&ring, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 3)) goto end;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 1, 0, 1))) goto end;

	if (RT_UNLIKELY(!rt_ring_pop_item(&ring, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 4)) goto end;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 1, 1, 0))) goto end;

	/* Pop on an empty ring. */
	if (RT_UNLIKELY(rt_ring_pop_item(&ring, (void**)&item))) goto end;

	if (RT_UNLIKELY(!rt_ring_new_item(&ring, (void**)&item))) goto end;
	*item = 5;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 2, 1, 1))) goto end;

	if (RT_UNLIKELY(!rt_ring_pop_item(&ring, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 5)) goto end;
	if (RT_UNLIKELY(!zz_test_ring_check_ring(&ring, 2, 2, 0))) goto end;

	ret = RT_OK;
end:
	if (ring_created) {
		if (RT_UNLIKELY(!rt_ring_free(&ring)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_test_ring(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_ring_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
