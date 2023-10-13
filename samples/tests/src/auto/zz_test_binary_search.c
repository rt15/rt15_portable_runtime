#include <rpr.h>

#include "zz_utils.h"

#define ZZ_TEST_BINARY_SEARCH_ARRAY_SIZE 9

static const rt_un zz_test_binary_search_array[ZZ_TEST_BINARY_SEARCH_ARRAY_SIZE] = { 2, 2, 4, 6, 6, 6, 8, 10, 10 };

rt_s zz_test_binary_search_index_do(rt_un item, rt_un *item_index)
{
	rt_un context_value = 42;
	rt_s ret;

	if (RT_UNLIKELY(!rt_binary_search_index(zz_test_binary_search_array, &item, ZZ_TEST_BINARY_SEARCH_ARRAY_SIZE, sizeof(rt_un), &zz_comparison_callback, &context_value, item_index)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_binary_search_index()
{
	rt_un item_index;
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(1, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(2, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != 0)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(3, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(4, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != 2)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(5, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(6, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != 3)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(7, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(8, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != 6)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(9, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(10, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != 7)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_index_do(11, &item_index))) goto error;
	if (RT_UNLIKELY(item_index != RT_TYPE_MAX_UN)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_binary_search_insertion_index_do(rt_un item, rt_un *insertion_index)
{
	rt_un context_value = 42;
	rt_s ret;

	if (RT_UNLIKELY(!rt_binary_search_insertion_index(zz_test_binary_search_array, &item, ZZ_TEST_BINARY_SEARCH_ARRAY_SIZE, sizeof(rt_un), &zz_comparison_callback, &context_value, insertion_index)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_binary_search_insertion_index()
{
	rt_un insertion_index;
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(1, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 0)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(2, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 0)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(3, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 2)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(4, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 2)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(5, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 3)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(6, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 3)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(7, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 6)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(8, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 6)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(9, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 7)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(10, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 7)) goto error;

	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index_do(11, &insertion_index))) goto error;
	if (RT_UNLIKELY(insertion_index != 9)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_binary_search()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_binary_search_index())) goto error;
	if (RT_UNLIKELY(!zz_test_binary_search_insertion_index())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
