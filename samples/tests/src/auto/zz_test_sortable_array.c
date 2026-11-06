#include <rpr.h>

#include "zz_utils.h"

static const rt_un zz_test_sortable_array_expected1[] =  { 10 };
static const rt_un zz_test_sortable_array_expected2[] =  { 10, 8 };
static const rt_un zz_test_sortable_array_expected3[] =  { 10, 8, 6 };
static const rt_un zz_test_sortable_array_expected4[] =  { 10, 6 };
static const rt_un zz_test_sortable_array_expected5[] =  { 10, 6, 8 };
static const rt_un zz_test_sortable_array_expected6[] =  { 6, 8, 10 };
static const rt_un zz_test_sortable_array_expected7[] =  { 6, 8, 10, 12 };
static const rt_un zz_test_sortable_array_expected8[] =  { 6, 7, 8, 10, 12 };
static const rt_un zz_test_sortable_array_expected9[] =  { 6, 7, 10, 12 };
static const rt_un zz_test_sortable_array_expected10[] = { 6, 7, 10 };

static rt_s zz_test_sortable_array_check(const struct rt_sortable_array *sortable_array, const rt_un *expected, rt_un expected_size, rt_b expected_sorted)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Checking array. */
	if (RT_UNLIKELY(sortable_array->array.size != expected_size)) goto end;
	if (RT_UNLIKELY(sortable_array->array.item_size != sizeof(rt_un))) goto end;

	/* Checking sortable array. */
	if (RT_UNLIKELY(sortable_array->callback != &zz_comparison_callback)) goto end;
	if (RT_UNLIKELY(*(rt_un*)sortable_array->context != 42)) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(sortable_array->sorted, expected_sorted))) goto end;

	for (i = 0; i < expected_size; i++) {
		if (RT_UNLIKELY(((rt_un*)sortable_array->array.items)[i] != expected[i]))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_sortable_array_do(struct rt_heap *heap)
{
	struct rt_sortable_array sortable_array;
	rt_b sortable_array_created = RT_FALSE;
	rt_un context_value = 42;
	rt_un item;
	rt_un item_index;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_sortable_array_create(&sortable_array, 0, sizeof(rt_un), heap, &zz_comparison_callback, &context_value))) goto end;
	sortable_array_created = RT_TRUE;

	if (RT_UNLIKELY(!sortable_array.sorted)) goto end;
	sortable_array.sorted = RT_FALSE;

	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, RT_NULL, 0, RT_FALSE))) goto end;

	item = 10;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 0)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected1, 1, RT_FALSE))) goto end;

	item = 8;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 1)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected2, 2, RT_FALSE))) goto end;

	item = 6;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 2)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected3, 3, RT_FALSE))) goto end;

	if (RT_UNLIKELY(!rt_sortable_array_delete_item_index(&sortable_array, 1))) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected4, 2, RT_FALSE))) goto end;

	item = 8;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 2)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected5, 3, RT_FALSE))) goto end;

	item = 8;
	if (RT_UNLIKELY(!rt_sortable_array_search_item_index(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 1)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected6, 3, RT_TRUE))) goto end;

	item = 12;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 3)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected7, 4, RT_TRUE))) goto end;

	item = 7;
	if (RT_UNLIKELY(!rt_sortable_array_add_item(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != 1)) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected8, 5, RT_TRUE))) goto end;

	if (RT_UNLIKELY(!rt_sortable_array_delete_item_index(&sortable_array, 2))) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected9, 4, RT_TRUE))) goto end;

	if (RT_UNLIKELY(!rt_sortable_array_delete_item_index(&sortable_array, 3))) goto end;
	if (RT_UNLIKELY(!zz_test_sortable_array_check(&sortable_array, zz_test_sortable_array_expected10, 3, RT_TRUE))) goto end;

	item = 404;
	if (RT_UNLIKELY(!rt_sortable_array_search_item_index(&sortable_array, &item, &item_index))) goto end;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto end;

	ret = RT_OK;
end:
	if (sortable_array_created) {
		if (RT_UNLIKELY(!rt_sortable_array_free(&sortable_array)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_sortable_array(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_sortable_array_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
