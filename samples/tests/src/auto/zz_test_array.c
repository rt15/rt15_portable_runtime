#include <rpr.h>

struct zz_test_array_header {
	rt_un value;
	struct rt_array_header array_header;
};

static rt_s zz_test_array_check(rt_un *array, rt_un size, struct rt_heap *heap)
{
	struct rt_array_header *header;
	struct zz_test_array_header *test_header;
	rt_un i;
	rt_s ret = RT_FAILED;

	header = RT_ARRAY_GET_HEADER(array);
	if (RT_UNLIKELY(header->size != size)) goto end;
	if (RT_UNLIKELY(header->item_size != sizeof(rt_un))) goto end;
	if (RT_UNLIKELY(header->header_size != sizeof(struct zz_test_array_header))) goto end;
	if (RT_UNLIKELY(header->heap != heap)) goto end;

	test_header = RT_ARRAY_GET_CUSTOM_HEADER(array, header);
	if (RT_UNLIKELY(test_header->value != 12)) goto end;

	if (RT_UNLIKELY(array[-1] != (rt_un)heap)) goto end;
	if (RT_UNLIKELY(array[-2] != sizeof(struct zz_test_array_header))) goto end;
	if (RT_UNLIKELY(array[-3] != sizeof(rt_un))) goto end;
	if (RT_UNLIKELY(array[-4] != size)) goto end;
	if (RT_UNLIKELY(array[-5] != 12)) goto end;

	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(array[i] != i))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_array_do(struct rt_heap *heap)
{
	rt_un *array = RT_NULL;
	struct rt_array_header *header;
	struct zz_test_array_header *test_header;
	rt_un *item;
	rt_un item_index;
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Test create. */
	if (RT_UNLIKELY(!rt_array_create((void**)&array, 10, sizeof(rt_un), sizeof(struct zz_test_array_header), heap))) goto end;

	header = RT_ARRAY_GET_HEADER(array);
	test_header = RT_ARRAY_GET_CUSTOM_HEADER(array, header);
	test_header->value = 12;

	for (i = 0; i < 10; i++)
		array[i] = i;

	if (RT_UNLIKELY(!zz_test_array_check(array, 10, heap))) goto end;

	/* Test set_size. */
	if (RT_UNLIKELY(!rt_array_set_size((void**)&array, 100))) goto end;

	for (i = 10; i < 100; i++)
		array[i] = i;

	if (RT_UNLIKELY(!zz_test_array_check(array, 100, heap))) goto end;

	/* Test new_item. */
	if (RT_UNLIKELY(!rt_array_new_item((void**)&array, (void**)&item))) goto end;
	*item = 100;

	if (RT_UNLIKELY(!zz_test_array_check(array, 101, heap))) goto end;

	/* Test new_item_index. */
	if (RT_UNLIKELY(!rt_array_new_item_index((void**)&array, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 101)) goto end;
	array[item_index] = 101;

	if (RT_UNLIKELY(!zz_test_array_check(array, 102, heap))) goto end;

	/* Test delete_item_index. */
	if (RT_UNLIKELY(!rt_array_delete_item_index((void**)&array, 100))) goto end;
	if (RT_UNLIKELY(array[100] != 101)) goto end;
	array[100] = 100;

	if (RT_UNLIKELY(!zz_test_array_check(array, 101, heap))) goto end;

	/* Test delete_last_item. */
	if (RT_UNLIKELY(!rt_array_delete_last_item((void**)&array))) goto end;
	if (RT_UNLIKELY(!zz_test_array_check(array, 100, heap))) goto end;

	/* Test get_last_item. */
	if (RT_UNLIKELY(!rt_array_get_last_item(array, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 99)) goto end;

	/* Test get_item. */
	if (RT_UNLIKELY(!rt_array_get_item(array, 99, (void**)&item))) goto end;
	if (RT_UNLIKELY(*item != 99)) goto end;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_array_free((void**)&array)))
		ret = RT_FAILED;

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
