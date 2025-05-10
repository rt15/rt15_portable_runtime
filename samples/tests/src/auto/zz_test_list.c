#include <rpr.h>

struct zz_test_list_item {
	rt_un32 value;
	rt_char filler[32];
};

struct zz_test_list_header {
	rt_un value;
	struct rt_list_header list_header;
};

static rt_s zz_test_list_check(void *list, rt_un size, rt_un chunks_count, struct rt_heap *heap)
{
	rt_un *array;
	rt_s ret = RT_FAILED;

	array = (rt_un*)list;
	if (RT_UNLIKELY(array[-1] != (rt_un)heap)) goto end;
	if (RT_UNLIKELY(array[-2] != sizeof(struct zz_test_list_header))) goto end;
	if (RT_UNLIKELY(array[-3] != sizeof(void*))) goto end;
	if (RT_UNLIKELY(array[-4] != chunks_count)) goto end;
	if (RT_UNLIKELY(array[-5] != 16)) goto end;
	if (RT_UNLIKELY(array[-6] != sizeof(struct zz_test_list_item))) goto end;
	if (RT_UNLIKELY(array[-7] != size)) goto end;
	if (RT_UNLIKELY(array[-8] != 12)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_list_do(struct rt_heap *heap)
{
	void *list = RT_NULL;
	struct rt_list_header *header;
	struct zz_test_list_header *test_header;
	struct zz_test_list_item *item;
	rt_un item_index;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_list_create(&list, 35, sizeof(struct zz_test_list_item), 16, sizeof(struct zz_test_list_header), heap))) goto end;

	header = RT_LIST_GET_HEADER(list);
	test_header = RT_LIST_GET_CUSTOM_HEADER(list, header);
	test_header->value = 12;

	for (i = 0; i < 35; i++) {
		if (RT_UNLIKELY(!rt_list_get_item(list, i, (void**)&item)))
			goto end;
		item->value = (rt_un32)i;
	}

	/* Reduce the size. */
	if (RT_UNLIKELY(!zz_test_list_check(list, 35, 3, heap))) goto end;
	if (RT_UNLIKELY(!rt_list_set_size(&list, 33))) goto end;
	if (RT_UNLIKELY(!zz_test_list_check(list, 33, 3, heap))) goto end;
	if (RT_UNLIKELY(!rt_list_set_size(&list, 29))) goto end;
	if (RT_UNLIKELY(!zz_test_list_check(list, 29, 2, heap))) goto end;

	/* Increase the size. */
	if (RT_UNLIKELY(!rt_list_set_size(&list, 51))) goto end;
	for (i = 0; i < 51; i++) {
		if (RT_UNLIKELY(!rt_list_get_item(list, i, (void**)&item)))
			goto end;
		item->value = (rt_un32)i;
	}
	if (RT_UNLIKELY(!zz_test_list_check(list, 51, 4, heap))) goto end;

	/* New item. */
	if (RT_UNLIKELY(!rt_list_new_item(&list, (void**)&item))) goto end;
	item->value = 51;
	if (RT_UNLIKELY(!zz_test_list_check(list, 52, 4, heap))) goto end;

	/* New item index. */
	if (RT_UNLIKELY(!rt_list_new_item_index(&list, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 52)) goto end;
	if (RT_UNLIKELY(!rt_list_get_item(list, 52, (void**)&item))) goto end;
	item->value = 52;
	if (RT_UNLIKELY(!zz_test_list_check(list, 53, 4, heap))) goto end;

	/* Prepare to delete in the middle of the list. */
	if (RT_UNLIKELY(!rt_list_get_item(list, 12, (void**)&item))) goto end;
	item->value = 99;
	if (RT_UNLIKELY(!rt_list_get_item(list, 52, (void**)&item))) goto end;
	item->value = 12;

	/* Delete in the middle of the list. */
	if (RT_UNLIKELY(!rt_list_delete_item_index(&list, 12))) goto end;
	if (RT_UNLIKELY(!zz_test_list_check(list, 52, 4, heap))) goto end;

	/* Delete the last item in the list. */
	if (RT_UNLIKELY(!rt_list_delete_item_index(&list, 51))) goto end;
	if (RT_UNLIKELY(!zz_test_list_check(list, 51, 4, heap))) goto end;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_list_free((void**)&list)))
		ret = RT_FAILED;

	return ret;
}

rt_s zz_test_list(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_list_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
