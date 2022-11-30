#include <rpr.h>

struct zz_test_array_header {
	rt_un value;
	struct rt_array_header array_header;
};

static rt_s zz_test_array_check(rt_un *array, rt_un size, struct rt_heap *heap)
{
	struct rt_array_header *array_header;
	struct zz_test_array_header *test_array_header;
	rt_un i;
	rt_s ret;

	array_header = RT_ARRAY_GET_HEADER(array);
	if (array_header->size != size) goto error;
	if (array_header->item_size != sizeof(rt_un)) goto error;
	if (array_header->header_size != sizeof(struct zz_test_array_header)) goto error;
	if (array_header->heap != heap) goto error;

	test_array_header = RT_ARRAY_GET_CUSTOM_HEADER(array, array_header);
	if (test_array_header->value != 42) goto error;

	if (array[-1] != (rt_un)heap) goto error;
	if (array[-2] != sizeof(struct zz_test_array_header)) goto error;
	if (array[-3] != sizeof(rt_un)) goto error;
	if (array[-4] != size) goto error;
	if (array[-5] != 42) goto error;

	for (i = 0; i < size; i++) {
		if (array[i] != i)
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_array_do(struct rt_heap *heap)
{
	rt_un *array = RT_NULL;
	struct rt_array_header *array_header;
	struct zz_test_array_header *test_array_header;
	rt_un *item;
	rt_un item_index;
	rt_un i;
	rt_s ret;

	/* Test create. */
	if (!rt_array_create((void**)&array, 10, sizeof(rt_un), sizeof(struct zz_test_array_header), heap)) goto error;

	array_header = RT_ARRAY_GET_HEADER(array);
	test_array_header = RT_ARRAY_GET_CUSTOM_HEADER(array, array_header);
	test_array_header->value = 42;

	for (i = 0; i < 10; i++)
		array[i] = i;

	if (!zz_test_array_check(array, 10, heap)) goto error;

	/* Test set_size. */
	if (!rt_array_set_size((void**)&array, 100)) goto error;

	for (i = 10; i < 100; i++)
		array[i] = i;

	if (!zz_test_array_check(array, 100, heap)) goto error;

	/* Test new_item. */
	if (!rt_array_new_item((void**)&array, (void**)&item)) goto error;
	*item = 100;

	if (!zz_test_array_check(array, 101, heap)) goto error;

	/* Test new_item_index. */
	if (!rt_array_new_item_index((void**)&array, &item_index)) goto error;
	if (item_index != 101) goto error;
	array[item_index] = 101;

	if (!zz_test_array_check(array, 102, heap)) goto error;

	/* Test delete_item_index. */
	if (!rt_array_delete_item_index((void**)&array, 100)) goto error;
	if (array[100] != 101) goto error;
	array[100] = 100;

	if (!zz_test_array_check(array, 101, heap)) goto error;

	/* Test delete_last_item. */
	if (!rt_array_delete_last_item((void**)&array)) goto error;
	if (!zz_test_array_check(array, 100, heap)) goto error;

	/* Test get_last_item. */
	if (!rt_array_get_last_item(array, (void**)&item)) goto error;
	if (*item != 99) goto error;

	/* Test get_item. */
	if (!rt_array_get_item(array, 99, (void**)&item)) goto error;
	if (*item != 99) goto error;

	ret = RT_OK;
free:
	if (!rt_array_free((void**)&array) && ret)
		goto error;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_array()
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret;

	if (!rt_runtime_heap_create(&runtime_heap))
		goto error;
	runtime_heap_created = RT_TRUE;

	if (!zz_test_array_do(&runtime_heap.heap)) goto error;

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
