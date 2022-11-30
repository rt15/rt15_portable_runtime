#include <rpr.h>

static rt_s zz_test_heap_do(struct rt_heap *heap)
{
	rt_char8 *area = RT_NULL;
	void *result;
	rt_un i;
	rt_s ret;

	result = heap->alloc(heap, (void**)&area, 10000);
	if (!result)
		goto error;
	if (result != area)
		goto error;
	for (i = 0; i < 10000; i++)
		area[i] = 'a';

	/* Re-allocate, asking for same size. */
	result = heap->realloc(heap, (void**)&area, 10000);
	if (!result)
		goto error;
	if (result != area)
		goto error;
	for (i = 0; i < 10000; i++) {
		if (area[i] != 'a')
			goto error;
	}

	/* Re-allocate, asking for a bigger size. */
	result = heap->realloc(heap, (void**)&area, 1000000);
	if (!result)
		goto error;
	if (result != area)
		goto error;
	for (i = 0; i < 10000; i++) {
		if (area[i] != 'a')
			goto error;
	}
	for (i = 10000; i < 1000000; i++)
		area[i] = 'b';

	/* Re-allocate, asking for a smaller size. */
	result = heap->realloc(heap, (void**)&area, 100);
	if (!result)
		goto error;
	if (result != area)
		goto error;
	for (i = 0; i < 100; i++) {
		if (area[i] != 'a')
			goto error;
	}

	ret = RT_OK;
free:
	if (area) {
		if (!heap->free(heap, (void**)&area) && ret)
			goto error;
		/* Check that free does set the pointer to RT_NULL. */
		if (area)
			goto error;
	}

	return ret;
error:
	ret = RT_FAILED;
	goto free;
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
	rt_s ret;

	/* Nothing provided, so failure. */
	if (rt_heap_alloc_if_needed(RT_NULL, 0, RT_NULL, RT_NULL, (void**)&area, 1, RT_NULL))
		goto error;

	/* Buffer is too small. */
	if (rt_heap_alloc_if_needed(buffer, 50, RT_NULL, RT_NULL, (void**)&area, 51, RT_NULL))
		goto error;

	/* Buffer is good. */
	result = rt_heap_alloc_if_needed(buffer, 50, RT_NULL, RT_NULL, (void**)&area, 50, RT_NULL);
	if (!result) goto error;
	if (area != buffer) goto error;
	for (i = 0; i < 50; i++)
		area[i] = 'a';

	/* Allocate with heap. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 100, heap);
	if (!result) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer_capacity != 100) goto error;
	previous_heap_buffer = heap_buffer;
	for (i = 0; i < 100; i++)
		area[i] = 'a';

	/* Heap buffer is enough. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 75, heap);
	if (!result) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer_capacity != 100) goto error;
	if (heap_buffer != previous_heap_buffer) goto error;
	for (i = 0; i < 75; i++)
		area[i] = 'a';

	/* Heap buffer is not enough. */
	result = rt_heap_alloc_if_needed(buffer, 50, &heap_buffer, &heap_buffer_capacity, (void**)&area, 150, heap);
	if (!result) goto error;
	if (area != heap_buffer) goto error;
	if (heap_buffer_capacity != 150) goto error;
	for (i = 0; i < 150; i++)
		area[i] = 'a';

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!heap->free(heap, &heap_buffer) && ret)
			goto error;
	}

	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_runtime_heap()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_heap_do(&runtime_heap.heap)) goto error;
	if (!zz_test_alloc_if_needed(&runtime_heap.heap)) goto error;

	ret = RT_OK;
free:
	if (runtime_heap_created) {
		runtime_heap_created = RT_FALSE;
		if (!runtime_heap.heap.close(&runtime_heap.heap) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_page_heap()
{
	struct rt_page_heap page_heap;
	rt_b page_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_page_heap_create(&page_heap))
		goto error;
	page_heap_created = RT_TRUE;

	if (!zz_test_heap_do(&page_heap.heap)) goto error;
	if (!zz_test_alloc_if_needed(&page_heap.heap)) goto error;

	ret = RT_OK;
free:
	if (page_heap_created) {
		page_heap_created = RT_FALSE;
		if (!page_heap.heap.close(&page_heap.heap) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_heap()
{
	rt_s ret;

	if (!zz_test_runtime_heap()) goto error;
	if (!zz_test_page_heap()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
