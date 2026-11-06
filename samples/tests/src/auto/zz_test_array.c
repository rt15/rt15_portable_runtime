#include <rpr.h>

static rt_s zz_test_array_check(struct rt_array *array, rt_un size, struct rt_heap *heap)
{
	rt_un *items;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(array->size != size)) goto end;
	if (RT_UNLIKELY(array->item_size != sizeof(rt_un))) goto end;
	if (RT_UNLIKELY(array->heap != heap)) goto end;

	items = (rt_un*)array->items;
	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(items[i] != i))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_array_do(struct rt_heap *heap)
{
	struct rt_array array;
	rt_b array_created = RT_FALSE;
	rt_un *items;
	rt_un *item;
	rt_un item_index;
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Test create. */
	if (RT_UNLIKELY(!rt_array_create(&array, 10, sizeof(rt_un), heap))) goto end;
	array_created = RT_TRUE;

	items = (rt_un*)array.items;
	for (i = 0; i < 10; i++)
		items[i] = i;

	if (RT_UNLIKELY(!zz_test_array_check(&array, 10, heap))) goto end;

	/* Test set_size. */
	if (RT_UNLIKELY(!rt_array_set_size(&array, 100))) goto end;

	items = (rt_un*)array.items;
	for (i = 10; i < 100; i++)
		items[i] = i;

	if (RT_UNLIKELY(!zz_test_array_check(&array, 100, heap))) goto end;

	/* Test new_item. */
	if (RT_UNLIKELY(!rt_array_new_item(&array, (void**)&item))) goto end;
	*item = 100;

	if (RT_UNLIKELY(!zz_test_array_check(&array, 101, heap))) goto end;

	/* Test new_item_index. */
	if (RT_UNLIKELY(!rt_array_new_item_index(&array, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 101)) goto end;
	items = (rt_un*)array.items;
	items[item_index] = 101;

	if (RT_UNLIKELY(!zz_test_array_check(&array, 102, heap))) goto end;

	/* Test delete_item_index. */
	if (RT_UNLIKELY(!rt_array_delete_item_index(&array, 100))) goto end;
	items = (rt_un*)array.items;
	if (RT_UNLIKELY(items[100] != 101)) goto end;
	items[100] = 100;

	if (RT_UNLIKELY(!zz_test_array_check(&array, 101, heap))) goto end;

	/* Test delete_last_item. */
	if (RT_UNLIKELY(!rt_array_delete_last_item(&array))) goto end;
	if (RT_UNLIKELY(!zz_test_array_check(&array, 100, heap))) goto end;

	/* Test get_last_item. */
	if (RT_UNLIKELY(!rt_array_get_last_item(&array, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 99)) goto end;

	/* Test get_item. */
	if (RT_UNLIKELY(!rt_array_get_item(&array, 99, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 99)) goto end;

	/* Test resizing to zero. */
	if (RT_UNLIKELY(!rt_array_set_size(&array, 0))) goto end;
	if (RT_UNLIKELY(array.items)) goto end;
	if (RT_UNLIKELY(!zz_test_array_check(&array, 0, heap))) goto end;

	/* Test growing from an empty array. */
	if (RT_UNLIKELY(!rt_array_new_item(&array, (void**)&item))) goto end;
	*item = 0;
	if (RT_UNLIKELY(!zz_test_array_check(&array, 1, heap))) goto end;

	ret = RT_OK;
end:
	if (array_created) {
		if (RT_UNLIKELY(!rt_array_free(&array)))
			ret = RT_FAILED;

		/* Freeing twice must be harmless. */
		if (RT_UNLIKELY(!rt_array_free(&array)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_array(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_array_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
