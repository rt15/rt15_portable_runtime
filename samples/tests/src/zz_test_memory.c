#include <rpr.h>

/**
 * <tt>area1</tt> should be the same as <tt>area2</tt>.
 */
static rt_s zz_test_memory_compare_same(void *area1, void *area2, rt_un size)
{
	rt_s ret;

	if (rt_memory_compare(area1, area2, size)) goto error;
	if (rt_memory_compare(area2, area1, size)) goto error;

	if (RT_MEMORY_COMPARE(area1, area2, size)) goto error;
	if (RT_MEMORY_COMPARE(area2, area1, size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

/**
 * <tt>area1</tt> must be greater than <tt>area2</tt>.
 */
static rt_s zz_test_memory_compare_different(void *area1, void *area2, rt_un size)
{
	rt_s ret;

	if (rt_memory_compare(area1, area2, size) <= 0) goto error;
	if (rt_memory_compare(area2, area1, size) >= 0) goto error;

	if (RT_MEMORY_COMPARE(area1, area2, size) <= 0) goto error;
	if (RT_MEMORY_COMPARE(area2, area1, size) >= 0) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_compare_memory()
{
	rt_char8 area1[32];
	rt_char8 area2[32];
	rt_un i;
	rt_un j;
	rt_s ret;

	if (!zz_test_memory_compare_same("bar", "bar", 3)) goto error;
	if (!zz_test_memory_compare_same("bar!", "bar!", 4)) goto error;
	if (!zz_test_memory_compare_same("foobar", "foobar", 6)) goto error;

	if (!zz_test_memory_compare_different("foo", "bar", 3)) goto error;
	if (!zz_test_memory_compare_different("fooo", "foob", 4)) goto error;
	if (!zz_test_memory_compare_different("foobar", "fooba!", 6)) goto error;

	for (i = 1; i < 28; i++) {
		for (j = 0; j < i - 1; j++) {
			area1[j] = (rt_char8)('a' + j);
			area2[j] = area1[j];
		}
		area1[i - 1] = 'B';
		area2[i - 1] = 'A';
		area1[i] = 0;
		area2[i] = 0;
		if (!zz_test_memory_compare_different(area1, area2, i))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_memory_do(void *source, rt_un size)
{
	rt_uchar8 buffer1[128];
	rt_uchar8 buffer2[128];
	rt_uchar8 buffer3[128];
	rt_uchar8 buffer4[128];
	void *returned_value;
	rt_s ret;

	returned_value = rt_memory_copy(source, buffer1, size);
	if (returned_value != buffer1) goto error;
	if (RT_MEMORY_COMPARE(source, buffer1, size)) goto error;

	returned_value = RT_MEMORY_COPY(source, buffer2, size);
	if (returned_value != buffer2) goto error;
	if (RT_MEMORY_COMPARE(source, buffer2, size)) goto error;

	returned_value = rt_memory_move(source, buffer3, size);
	if (returned_value != buffer3) goto error;
	if (RT_MEMORY_COMPARE(source, buffer3, size)) goto error;

	returned_value = RT_MEMORY_MOVE(source, buffer4, size);
	if (returned_value != buffer4) goto error;
	if (RT_MEMORY_COMPARE(source, buffer4, size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_memory()
{
	rt_s ret;

	if (!zz_test_copy_memory_do("foo", 3)) goto error;
	if (!zz_test_copy_memory_do("foob", 4)) goto error;
	if (!zz_test_copy_memory_do("foobar", 6)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_move_memory_do(rt_char8 *input, rt_un source_size, rt_un destination_index, rt_char8 *expected, rt_un size)
{
	rt_char8 buffer[200];
	void *result;
	rt_un test;
	rt_s ret;

	for (test = 0; test < 2; test++) {
		/* Copy input into mutable memory. */
		if (size > 200) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto error;
		}
		RT_MEMORY_COPY(input, buffer, size);

		if (test)
			result = rt_memory_move(buffer, &buffer[destination_index], source_size);
		else
			result = RT_MEMORY_MOVE(buffer, &buffer[destination_index], source_size);

		if (result != &buffer[destination_index])
			goto error;

		/* Check result against expected. */
		if (RT_MEMORY_COMPARE(buffer, expected, size))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_move_memory()
{
	rt_s ret;

	if (!zz_test_move_memory_do("123", 2, 1, "112", 3)) goto error;
	if (!zz_test_move_memory_do("1234567890abcde", 10, 5, "123451234567890", 15)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_set_memory_check(rt_uchar8 *buffer, rt_un size, rt_uchar8 expected, rt_uchar8 trailing)
{
	rt_un i;
	rt_s ret;

	for (i = 0; i < size; i++) {
		if (buffer[i] != expected)
			goto error;
	}
	if (buffer[size] != trailing)
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_set_memory_do(rt_un size)
{
	rt_uchar8 buffer[32];
	void *result;
	rt_un i;
	rt_s ret;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = rt_memory_set(buffer, 170, size);
	if (result != buffer) goto error;
	if (!zz_test_set_memory_check(buffer, size, 170, 85)) goto error;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = RT_MEMORY_SET(buffer, 170, size);
	if (result != buffer) goto error;
	if (!zz_test_set_memory_check(buffer, size, 170, 85)) goto error;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = rt_memory_zero(buffer, size);
	if (result != buffer) goto error;
	if (!zz_test_set_memory_check(buffer, size, 0, 85)) goto error;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = RT_MEMORY_ZERO(buffer, size);
	if (result != buffer) goto error;
	if (!zz_test_set_memory_check(buffer, size, 0, 85)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_set_memory()
{
	rt_un i;
	rt_s ret;

	for (i = 1; i < 28; i++) {
		if (!zz_test_set_memory_do(i)) goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_memory_swap_do(rt_un size)
{
	rt_uchar8 area1[32];
	rt_uchar8 area2[32];
	rt_uchar8 i;
	rt_s ret;

	for (i = 0; i < size; i++) {
		area1[i] = 'a' + i;
		area2[i] = 'A' + i;
	}
	area1[size] = 1;
	area2[size] = 2;

	rt_memory_swap(area1, area2, size);
	for (i = 0; i < size; i++) {
		if (area1[i] != 'A' + i) goto error;
		if (area2[i] != 'a' + i) goto error;
	}
	if (area1[size] != 1) goto error;
	if (area2[size] != 2) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_memory_swap()
{
	rt_un i;
	rt_s ret;

	for (i = 0; i < 27; i++) {
		if (!zz_test_memory_swap_do(i))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_memory_get_chunks_count_values(rt_un size, rt_un chunk_size)
{
	rt_un expected;
	rt_s ret;

	expected = size / chunk_size;
	if (size % chunk_size)
		expected++;

	if (RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size) != expected)
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_memory_get_chunks_count()
{
	rt_un i;
	rt_s ret;

	for (i = 2; i < 80000; i++) {
		if (!zz_test_memory_get_chunks_count_values(i, 1))   goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 2))   goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 4))   goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 8))   goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 16))  goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 32))  goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 64))  goto error;
		if (!zz_test_memory_get_chunks_count_values(i, 128)) goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_memory_modulo()
{
	rt_un i;
	rt_s ret;

	for (i = 0; i < 80000; i++) {

		if (i % sizeof(rt_n) != RT_MEMORY_MODULO_RT_UN_SIZE(i)) goto error;

		if (i % 1   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 1))   goto error;
		if (i % 2   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 2))   goto error;
		if (i % 4   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 4))   goto error;
		if (i % 8   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 8))   goto error;
		if (i % 16  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 16))  goto error;
		if (i % 32  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 32))  goto error;
		if (i % 64  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 64))  goto error;
		if (i % 128 != RT_MEMORY_MODULO_POWER_OF_TWO(i, 128)) goto error;

		switch (i)
		{
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
			case 128:
			case 256:
			case 512:
			case 1024:
			case 2048:
			case 4096:
			case 8192:
			case 16384:
			case 32768:
			case 65536:
				if (!RT_MEMORY_IS_POWER_OF_TWO(i)) goto error;
				break;
			default:
				if (RT_MEMORY_IS_POWER_OF_TWO(i)) goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_memory()
{
	rt_s ret;

	if (!zz_test_compare_memory()) goto error;
	if (!zz_test_copy_memory()) goto error;
	if (!zz_test_move_memory()) goto error;
	if (!zz_test_set_memory()) goto error;
	if (!zz_test_memory_swap()) goto error;
	if (!zz_test_memory_get_chunks_count()) goto error;
	if (!zz_test_memory_modulo()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
