#include <rpr.h>

/**
 * <tt>area1</tt> should be the same as <tt>area2</tt>.
 */
static rt_s zz_test_memory_compare_same(void *area1, void *area2, rt_un size)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(rt_memory_compare(area1, area2, size))) goto end;
	if (RT_UNLIKELY(rt_memory_compare(area2, area1, size))) goto end;

	if (RT_UNLIKELY(RT_MEMORY_COMPARE(area1, area2, size))) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(area2, area1, size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

/**
 * <tt>area1</tt> must be greater than <tt>area2</tt>.
 */
static rt_s zz_test_memory_compare_different(void *area1, void *area2, rt_un size)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(rt_memory_compare(area1, area2, size) <= 0)) goto end;
	if (RT_UNLIKELY(rt_memory_compare(area2, area1, size) >= 0)) goto end;

	if (RT_UNLIKELY(RT_MEMORY_COMPARE(area1, area2, size) <= 0)) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(area2, area1, size) >= 0)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_compare_memory(void)
{
	rt_char8 area1[32];
	rt_char8 area2[32];
	rt_un i;
	rt_un j;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_memory_compare_same("bar", "bar", 3))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_compare_same("bar!", "bar!", 4))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_compare_same("foobar", "foobar", 6))) goto end;

	if (RT_UNLIKELY(!zz_test_memory_compare_different("foo", "bar", 3))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_compare_different("fooo", "foob", 4))) goto end;
	if (RT_UNLIKELY(!zz_test_memory_compare_different("foobar", "fooba!", 6))) goto end;

	for (i = 1; i < 28; i++) {
		for (j = 0; j < i - 1; j++) {
			area1[j] = (rt_char8)('a' + j);
			area2[j] = area1[j];
		}
		area1[i - 1] = 'B';
		area2[i - 1] = 'A';
		area1[i] = 0;
		area2[i] = 0;
		if (RT_UNLIKELY(!zz_test_memory_compare_different(area1, area2, i)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_copy_memory_do(void *source, rt_un size)
{
	rt_uchar8 buffer1[128];
	rt_uchar8 buffer2[128];
	rt_uchar8 buffer3[128];
	rt_uchar8 buffer4[128];
	void *returned_value;
	rt_s ret = RT_FAILED;

	returned_value = rt_memory_copy(source, buffer1, size);
	if (RT_UNLIKELY(returned_value != buffer1)) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(source, buffer1, size))) goto end;

	returned_value = RT_MEMORY_COPY(source, buffer2, size);
	if (RT_UNLIKELY(returned_value != buffer2)) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(source, buffer2, size))) goto end;

	returned_value = rt_memory_move(source, buffer3, size);
	if (RT_UNLIKELY(returned_value != buffer3)) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(source, buffer3, size))) goto end;

	returned_value = RT_MEMORY_MOVE(source, buffer4, size);
	if (RT_UNLIKELY(returned_value != buffer4)) goto end;
	if (RT_UNLIKELY(RT_MEMORY_COMPARE(source, buffer4, size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_copy_memory(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_copy_memory_do("foo", 3))) goto end;
	if (RT_UNLIKELY(!zz_test_copy_memory_do("foob", 4))) goto end;
	if (RT_UNLIKELY(!zz_test_copy_memory_do("foobar", 6))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_move_memory_do(const rt_char8 *input, rt_un source_size, rt_un destination_index, const rt_char8 *expected, rt_un size)
{
	rt_char8 buffer[200];
	void *result;
	rt_un test;
	rt_s ret = RT_FAILED;

	for (test = 0; test < 2; test++) {
		/* Copy input into mutable memory. */
		if (RT_UNLIKELY(size > 200)) {
			rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			goto end;
		}
		RT_MEMORY_COPY(input, buffer, size);

		if (test)
			result = rt_memory_move(buffer, &buffer[destination_index], source_size);
		else
			result = RT_MEMORY_MOVE(buffer, &buffer[destination_index], source_size);

		if (RT_UNLIKELY(result != &buffer[destination_index]))
			goto end;

		/* Check result against expected. */
		if (RT_UNLIKELY(RT_MEMORY_COMPARE(buffer, expected, size)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_move_memory(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_move_memory_do("123", 2, 1, "112", 3))) goto end;
	if (RT_UNLIKELY(!zz_test_move_memory_do("1234567890abcde", 10, 5, "123451234567890", 15))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_set_memory_check(rt_uchar8 *buffer, rt_un size, rt_uchar8 expected, rt_uchar8 trailing)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(buffer[i] != expected))
			goto end;
	}
	if (RT_UNLIKELY(buffer[size] != trailing))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_set_memory_do(rt_un size)
{
	rt_uchar8 buffer[32];
	void *result;
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = rt_memory_set(buffer, 170, size);
	if (RT_UNLIKELY(result != buffer)) goto end;
	if (RT_UNLIKELY(!zz_test_set_memory_check(buffer, size, 170, 85))) goto end;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = RT_MEMORY_SET(buffer, 170, size);
	if (RT_UNLIKELY(result != buffer)) goto end;
	if (RT_UNLIKELY(!zz_test_set_memory_check(buffer, size, 170, 85))) goto end;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = rt_memory_zero(buffer, size);
	if (RT_UNLIKELY(result != buffer)) goto end;
	if (RT_UNLIKELY(!zz_test_set_memory_check(buffer, size, 0, 85))) goto end;

	for (i = 0; i <= size; i++) buffer[i] = 85;
	result = RT_MEMORY_ZERO(buffer, size);
	if (RT_UNLIKELY(result != buffer)) goto end;
	if (RT_UNLIKELY(!zz_test_set_memory_check(buffer, size, 0, 85))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_set_memory(void)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 1; i < 28; i++) {
		if (RT_UNLIKELY(!zz_test_set_memory_do(i))) goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_swap_do(rt_un size)
{
	rt_uchar8 area1[32];
	rt_uchar8 area2[32];
	rt_uchar8 i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < size; i++) {
		area1[i] = 'a' + i;
		area2[i] = 'A' + i;
	}
	area1[size] = 1;
	area2[size] = 2;

	rt_memory_swap(area1, area2, size);
	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(area1[i] != 'A' + i)) goto end;
		if (RT_UNLIKELY(area2[i] != 'a' + i)) goto end;
	}
	if (RT_UNLIKELY(area1[size] != 1)) goto end;
	if (RT_UNLIKELY(area2[size] != 2)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_swap(void)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < 27; i++) {
		if (RT_UNLIKELY(!zz_test_memory_swap_do(i)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_get_chunks_count_values(rt_un size, rt_un chunk_size)
{
	rt_un expected;
	rt_s ret = RT_FAILED;

	expected = size / chunk_size;
	if (size % chunk_size)
		expected++;

	if (RT_UNLIKELY(RT_MEMORY_GET_CHUNKS_COUNT(size, chunk_size) != expected))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_get_chunks_count(void)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 2; i < 80000; i++) {
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 1)))   goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 2)))   goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 4)))   goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 8)))   goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 16)))  goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 32)))  goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 64)))  goto end;
		if (RT_UNLIKELY(!zz_test_memory_get_chunks_count_values(i, 128))) goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_modulo(void)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < 80000; i++) {

		if (RT_UNLIKELY(i % sizeof(rt_n) != RT_MEMORY_MODULO_RT_UN_SIZE(i))) goto end;

		if (RT_UNLIKELY(i % 1   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 1)))   goto end;
		if (RT_UNLIKELY(i % 2   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 2)))   goto end;
		if (RT_UNLIKELY(i % 4   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 4)))   goto end;
		if (RT_UNLIKELY(i % 8   != RT_MEMORY_MODULO_POWER_OF_TWO(i, 8)))   goto end;
		if (RT_UNLIKELY(i % 16  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 16)))  goto end;
		if (RT_UNLIKELY(i % 32  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 32)))  goto end;
		if (RT_UNLIKELY(i % 64  != RT_MEMORY_MODULO_POWER_OF_TWO(i, 64)))  goto end;
		if (RT_UNLIKELY(i % 128 != RT_MEMORY_MODULO_POWER_OF_TWO(i, 128))) goto end;

		switch (i) {
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
			if (RT_UNLIKELY(!RT_MEMORY_IS_POWER_OF_TWO(i))) goto end;
			break;
		default:
			if (RT_UNLIKELY(RT_MEMORY_IS_POWER_OF_TWO(i))) goto end;
		}
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_set_char16_do(rt_un size)
{
	rt_un16 buffer[32];
	void *result;
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i <= size; i++)
		buffer[i] = L'à';

	result = rt_memory_set_char16(buffer, L'é', size);
	if (RT_UNLIKELY(result != buffer)) goto end;

	for (i = 0; i < size; i++) {
		if (RT_UNLIKELY(buffer[i] != L'é'))
			goto end;
	}
	if (RT_UNLIKELY(buffer[size] != L'à'))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_set_char16(void)
{
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 1; i < 28; i++) {
		if (RT_UNLIKELY(!zz_test_memory_set_char16_do(i))) goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_set_char(void)
{
	rt_char buffer[20];
	void *result;
	rt_un i;
	rt_un size;
	rt_s ret = RT_FAILED;

	for (size = 1; size < 15; size++) {
		for (i = 0; i < 20; i++)
			buffer[i] = _R('b');

		result = RT_MEMORY_SET_CHAR(buffer, _R('a'), 7);
		if (RT_UNLIKELY(result != buffer))
			goto end;

		for (i = 0; i < 7; i++)
			if (RT_UNLIKELY(buffer[i] != _R('a')))
				goto end;

		for (i = 7; i < 20; i++)
			if (RT_UNLIKELY(buffer[i] != _R('b')))
				goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_xnor(void)
{
	rt_b zero = 0;
	rt_b one = 1;
	rt_b two = 2;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!RT_MEMORY_XNOR(zero, zero))) goto end;
	if (RT_UNLIKELY(RT_MEMORY_XNOR(zero, one))) goto end;
	if (RT_UNLIKELY(RT_MEMORY_XNOR(zero, two))) goto end;

	if (RT_UNLIKELY(RT_MEMORY_XNOR(one, zero))) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(one, one))) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(one, two))) goto end;

	if (RT_UNLIKELY(RT_MEMORY_XNOR(two, zero))) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(two, one))) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(two, two))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_swap_bytes16(void)
{
	rt_char8 data[4] = { 0x02, 0x01, 0x04, 0x03 };
	rt_un16 *source = (rt_un16*)data;
	rt_un16 *result;
	rt_n i;
	rt_s ret = RT_FAILED;

	result = rt_memory_swap_bytes16(source, source, 2);
	if (RT_UNLIKELY(result != source))
		goto end;

	for (i = 0; i < 4; i++)
		if (RT_UNLIKELY(data[i] != i + 1))
			goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_memory_swap_bytes32(void)
{
	rt_char8 data[8] = { 0x04, 0x03, 0x02, 0x01, 0x08, 0x07, 0x06, 0x05 };
	rt_un32 *source = (rt_un32*)data;
	rt_un32 *result;
	rt_n i;
	rt_s ret = RT_FAILED;

	result = rt_memory_swap_bytes32(source, source, 2);
	if (RT_UNLIKELY(result != source))
		goto end;

	for (i = 0; i < 8; i++)
		if (RT_UNLIKELY(data[i] != i + 1))
			goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_memory(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_compare_memory())) goto end;
	if (RT_UNLIKELY(!zz_test_copy_memory())) goto end;
	if (RT_UNLIKELY(!zz_test_move_memory())) goto end;
	if (RT_UNLIKELY(!zz_test_set_memory())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_swap())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_get_chunks_count())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_modulo())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_set_char16())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_set_char())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_xnor())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_swap_bytes16())) goto end;
	if (RT_UNLIKELY(!zz_test_memory_swap_bytes32())) goto end;

	ret = RT_OK;
end:
	return ret;
}
