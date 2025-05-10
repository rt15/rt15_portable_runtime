#include <rpr.h>

static rt_s zz_test_heap_do(struct rt_heap *heap)
{
	rt_char8 *area = RT_NULL;
	void *result;
	rt_un i;
	rt_s ret = RT_FAILED;

	result = heap->alloc(heap, (void**)&area, 10000);
	if (RT_UNLIKELY(!result))
		goto end;
	if (RT_UNLIKELY(result != area))
		goto end;
	for (i = 0; i < 10000; i++)
		area[i] = 'a';

	/* Re-allocate, asking for same size. */
	result = heap->realloc(heap, (void**)&area, 10000);
	if (RT_UNLIKELY(!result))
		goto end;
	if (RT_UNLIKELY(result != area))
		goto end;
	for (i = 0; i < 10000; i++) {
		if (RT_UNLIKELY(area[i] != 'a'))
			goto end;
	}

	/* Re-allocate, asking for a bigger size. */
	result = heap->realloc(heap, (void**)&area, 1000000);
	if (RT_UNLIKELY(!result))
		goto end;
	if (RT_UNLIKELY(result != area))
		goto end;
	for (i = 0; i < 10000; i++) {
		if (RT_UNLIKELY(area[i] != 'a'))
			goto end;
	}
	for (i = 10000; i < 1000000; i++)
		area[i] = 'b';

	/* Re-allocate, asking for a smaller size. */
	result = heap->realloc(heap, (void**)&area, 100);
	if (RT_UNLIKELY(!result))
		goto end;
	if (RT_UNLIKELY(result != area))
		goto end;
	for (i = 0; i < 100; i++) {
		if (RT_UNLIKELY(area[i] != 'a'))
			goto end;
	}

	ret = RT_OK;
end:
	if (area) {
		if (RT_UNLIKELY(!heap->free(heap, (void**)&area)))
			ret = RT_FAILED;
		/* Check that free does set the pointer to RT_NULL. */
		if (RT_UNLIKELY(area))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_alloc_if_needed(struct rt_heap *heap)
{
	rt_char8 buffer[50];
	rt_char8 *area;
	void *heap_buffer = RT_NULL;
	void *previous_heap_buffer;
	rt_un heap_buffer_capacity = 0;
	rt_un i;
	void *result;
	rt_s ret = RT_FAILED;

	/* Nothing provided, so failure. */
	if (RT_UNLIKELY(rt_heap_alloc_if_needed(RT_NULL, 0, RT_NULL, RT_NULL, (void**)&area, 1, RT_NULL)))
		goto end;

	/* Buffer is too small. */
	if (RT_UNLIKELY(rt_heap_alloc_if_needed(buffer, 50, RT_NULL, RT_NULL, (void**)&area, 51, RT_NULL)))
		goto end;

	/* Buffer is good. */
	result = rt_heap_alloc_if_needed(buffer, 50, RT_NULL, RT_NULL, (void**)&area, 50, RT_NULL);
	if (RT_UNLIKELY(!result)) goto end;
	if (RT_UNLIKELY(area != buffer)) goto end;
	for (i = 0; i < 50; i++)
		area[i] = 'a';

	/* Allocate with heap. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 100, heap);
	if (RT_UNLIKELY(!result)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 100)) goto end;
	previous_heap_buffer = heap_buffer;
	for (i = 0; i < 100; i++)
		area[i] = 'a';

	/* Heap buffer is enough. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 75, heap);
	if (RT_UNLIKELY(!result)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 100)) goto end;
	if (RT_UNLIKELY(heap_buffer != previous_heap_buffer)) goto end;
	for (i = 0; i < 75; i++)
		area[i] = 'a';

	/* Heap buffer is not enough. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 150, heap);
	if (RT_UNLIKELY(!result)) goto end;
	if (RT_UNLIKELY(area != heap_buffer)) goto end;
	if (RT_UNLIKELY(heap_buffer_capacity != 150)) goto end;
	for (i = 0; i < 150; i++)
		area[i] = 'a';

	ret = RT_OK;
end:
	if (heap_buffer) {
		if (RT_UNLIKELY(!heap->free(heap, &heap_buffer)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_runtime_heap(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_heap_do(&runtime_heap.heap))) goto end;
	if (RT_UNLIKELY(!zz_test_alloc_if_needed(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_page_heap(void)
{
	struct rt_page_heap page_heap;
	rt_b page_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_page_heap_create(&page_heap)))
		goto end;
	page_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_heap_do(&page_heap.heap))) goto end;
	if (RT_UNLIKELY(!zz_test_alloc_if_needed(&page_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (page_heap_created) {
		if (RT_UNLIKELY(!page_heap.heap.close(&page_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_heap(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_runtime_heap())) goto end;
	if (RT_UNLIKELY(!zz_test_page_heap())) goto end;

	ret = RT_OK;
end:
	return ret;
}
