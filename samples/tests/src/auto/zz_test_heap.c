#include <rpr.h>

static rt_s zz_test_heap_do(struct rt_heap *heap)
{
	rt_char8 *area = RT_NULL;
	void* result;
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

static rt_s zz_test_runtime_heap()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_heap_do(&runtime_heap.heap))
		goto error;

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

	if (!zz_test_heap_do(&page_heap.heap))
		goto error;

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
