#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_quick_sort_simple_callback(const void *item1, const void *item2, RT_UNUSED void *context, rt_n *comparison_result)
{
	rt_char8 *char1 = (rt_char8*)item1;
	rt_char8 *char2 = (rt_char8*)item2;

	*comparison_result = *char1 - *char2;

	return RT_OK;
}

static rt_s zz_test_quick_sort_simple()
{
	rt_char8 data[20];
	rt_un i;
	rt_s ret = RT_FAILED;

	data[0] = 15;
	data[1] = 11;
	data[2] = 3;
	data[3] = 5;
	data[4] = 19;
	data[5] = 2;
	data[6] = 1;
	data[7] = 18;
	data[8] = 10;
	data[9] = 17;
	data[10] = 20;
	data[11] = 9;
	data[12] = 4;
	data[13] = 12;
	data[14] = 14;
	data[15] = 8;
	data[16] = 6;
	data[17] = 7;
	data[18] = 13;
	data[19] = 16;

	if (RT_UNLIKELY(!rt_quick_sort(data, 20, 1, &zz_test_quick_sort_simple_callback, RT_NULL)))
		goto end;
	
	for (i = 0; i < 20; i++) {
		if (data[i] != (rt_char8)i + 1)
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_quick_sort_random(struct rt_heap *heap)
{
	rt_un *array = RT_NULL;
	rt_un size;
	rt_un item;
	rt_un check_sum = 0;
	rt_un context_value = 42;
	rt_un previous_item = 0;
	rt_un new_sum = 0;
	rt_un i;
	rt_s ret = RT_FAILED;

	/* Create an array of a random size. */
	if (RT_UNLIKELY(!rt_random_get_unsigned_integer_with_boundaries(2, 1000, &size))) goto end;
	if (RT_UNLIKELY(!rt_array_create((void**)&array, size, sizeof(rt_un), 0, heap))) goto end;

	/* Fill the array with some random values. */
	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(!rt_random_get_unsigned_integer_with_boundaries(0, 1000, &item)))
			goto end;
		array[i] = item;
		check_sum += item;
	}

	if (RT_UNLIKELY(!rt_quick_sort(array, size, sizeof(rt_un), &zz_comparison_callback, &context_value)))
		goto end;

	for (i = 0; i < size; i++) {
		item = array[i];
		if (RT_UNLIKELY(item < previous_item))
			goto end;
		new_sum += item;
		previous_item = item;
	}
	if (RT_UNLIKELY(new_sum != check_sum))
		goto end;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!rt_array_free((void**)&array)))
		ret = RT_FAILED;

	return ret;
}

rt_s zz_test_quick_sort(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_quick_sort_simple())) goto end;
	if (RT_UNLIKELY(!zz_test_quick_sort_random(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
