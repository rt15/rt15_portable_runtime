#include <rpr.h>

#define ZZ_TEST_CHAR8_ACCENTED_CHARACTERS_CASE 0

static rt_s zz_test_char8_equals_single(rt_char8 *str1, rt_char8 *str2, rt_b expected)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_equals(str1, rt_char8_get_size(str1), str2, rt_char8_get_size(str2)) != !expected)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(str2, rt_char8_get_size(str2), str1, rt_char8_get_size(str1)) != !expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_equals(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_equals_single("Foo",     "Foo",      RT_TRUE)))  goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("foo",     "Foo",      RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("Foo",     "Bar",      RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("",        "",         RT_TRUE)))  goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("a",       "",         RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("",        "a",        RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("notsame", "notsama",  RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals_single("notsame", "notsamez", RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_compare(void)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_compare("abc", "abc"))) goto error;
	if (RT_UNLIKELY(rt_char8_compare("", ""))) goto error;
	if (RT_UNLIKELY(rt_char8_compare("abc", "abd") >= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("abd", "abc") <= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("abc", "ab") <= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("ab", "abc") >= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("a", "È") >= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("È", "a") <= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("a", " ") <= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare(" ", "a") >= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("a", "") <= 0)) goto error;
	if (RT_UNLIKELY(rt_char8_compare("", "a") >= 0)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_ok(const rt_char8 *prefix, const rt_char8 *suffix, const rt_char8 *expected)
{
	rt_s ret;
	rt_char8 buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append(prefix, rt_char8_get_size(prefix), buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(prefix))) goto error;
	if (RT_UNLIKELY(!rt_char8_append(suffix, rt_char8_get_size(suffix), buffer, 14, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(prefix) + rt_char8_get_size(suffix))) goto error;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_failed(const rt_char8 *prefix, const rt_char8 *suffix, const rt_char8 *expected)
{
	rt_s ret;
	rt_char8 buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append(prefix, rt_char8_get_size(prefix), buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(prefix))) goto error;
	if (RT_UNLIKELY(rt_char8_append(suffix, rt_char8_get_size(suffix), buffer, 14, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != 13)) goto error;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_ok("Hello",   ", World!", "Hello, World!"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_ok("",        "Foo",      "Foo")))           goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_ok("Bar",     "",         "Bar")))           goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_ok("",        "",         "")))              goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_ok("1234567", "123456",   "1234567123456"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_failed("1234567", "1234567",  "1234567123456"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("1234567", "12345678", "1234567123456"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_failed("12345678",       "123456", "1234567812345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("123456789",      "123456", "1234567891234"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("1234567890",     "123456", "1234567890123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("12345678901",    "123456", "1234567890112"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("123456789012",   "123456", "1234567890121"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("1234567890123",  "123456", "1234567890123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("12345678901234", "123456", "1234567890123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_failed("12345678901235", "123456", "1234567890123"))) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_char_do(rt_s status, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[10];
	rt_un local_buffer_size = buffer_size;
	rt_s ret;
	rt_s result;

	RT_MEMORY_SET(buffer, 'a', 10);

	result = rt_char8_append_char('b', buffer, 10, &local_buffer_size);
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, status)))
		goto error;

	if (RT_UNLIKELY(local_buffer_size != rt_char8_get_size(expected)))
		goto error;

	if (RT_UNLIKELY(!rt_char8_equals(buffer, local_buffer_size, expected, local_buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_char(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_OK, 0, "b"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_OK, 6, "aaaaaab"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_OK, 7, "aaaaaaab"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_OK, 8, "aaaaaaaab"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_FAILED, 9, "aaaaaaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_FAILED, 10, "aaaaaaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char_do(RT_FAILED, 11, "aaaaaaaaa"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy_ok(const rt_char8 *str)
{
	rt_s ret;
	rt_char8 buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test append without prefix. */
	if (RT_UNLIKELY(!rt_char8_append(str, rt_char8_get_size(str), buffer, 6, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(str))) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), str, rt_char8_get_size(str)))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (RT_UNLIKELY(!rt_char8_copy(str, rt_char8_get_size(str), buffer, 6))) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), str, rt_char8_get_size(str)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy_failed(const rt_char8 *str, const rt_char8 *expected)
{
	rt_s ret;
	rt_char8 buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test append without prefix. */
	if (RT_UNLIKELY(rt_char8_append(str, rt_char8_get_size(str), buffer, 6, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char8_get_size(expected))) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected)))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (RT_UNLIKELY(rt_char8_copy(str, rt_char8_get_size(str), buffer, 6))) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_copy_ok("Hello"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_copy_ok(""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_copy_failed("Hello1", "Hello"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_get_size(void)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_get_size("") != 0)) goto error;
	if (RT_UNLIKELY(rt_char8_get_size("foo") != 3)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_un_ok(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append_un(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_un_failed(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(rt_char8_append_un(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_un(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 7, 5, "aaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 6, 5, "aaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 5, 5, "aaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 4, 5, "aaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 3, 5, "aa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 2, 5, "a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12, 1, 1, 5, ""))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(0, 10, 10, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(1, 10, 10, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12, 10, 10, 0, "12"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(123, 10, 10, 0, "123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(1234, 10, 10, 0, "1234"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12345, 10, 10, 0, "12345"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12345, 10, 10, 1, "a12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12345, 10, 10, 2, "aa12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12345, 10, 10, 3, "aaa12345"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_un_ok(12345, 10, 9, 3, "aaa12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12345, 10, 8, 3, "aaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un_failed(12345, 10, 7, 3, "aaa"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n_ok(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append_n(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n_failed(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(rt_char8_append_n(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 7, 5, "aaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 6, 5, "aaaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 5, 5, "aaaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 4, 5, "aaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 3, 5, "aa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 2, 5, "a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12, 1, 1, 5, ""))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(0, 10, 10, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(1, 10, 10, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12, 10, 10, 0, "12"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(123, 10, 10, 0, "123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(1234, 10, 10, 0, "1234"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12345, 10, 10, 0, "12345"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12345, 10, 10, 1, "a12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12345, 10, 10, 2, "aa12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12345, 10, 10, 3, "aaa12345"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(12345, 10, 9, 3, "aaa12345"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12345, 10, 8, 3, "aaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(12345, 10, 7, 3, "aaa"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_n_ok(-1234, 10, 9, 3, "aaa-1234"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(-1234, 10, 8, 3, "aaa"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n_failed(-1234, 10, 7, 3, "aaa"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_f32(rt_f32 value, rt_un decimal_count, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un buffer_size = 0;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append_f32(value, decimal_count, buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_f64(rt_f64 value, rt_un decimal_count, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un buffer_size = 0;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (RT_UNLIKELY(!rt_char8_append_f64(value, decimal_count, buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_f_do(const rt_char8 *str, rt_f64 expected)
{
	rt_f32 result32;
	rt_f64 result64;
	rt_un str_size = rt_char8_get_size(str);
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_convert_to_f32(str, &result32))) goto error;
	if (RT_UNLIKELY(!RT_FLOAT_EQUALS(result32, (rt_f32)expected))) goto error;

	result32 = 99.9f;
	if (RT_UNLIKELY(!rt_char8_convert_to_f32_with_size(str, str_size, &result32))) goto error;
	if (RT_UNLIKELY(!RT_FLOAT_EQUALS(result32, (rt_f32)expected))) goto error;

	if (RT_UNLIKELY(!rt_char8_convert_to_f64(str, &result64))) goto error;
	if (RT_UNLIKELY(!RT_DOUBLE_EQUALS(result64, expected))) goto error;

	result64 = 99.9f;
	if (RT_UNLIKELY(!rt_char8_convert_to_f64_with_size(str, str_size, &result64))) goto error;
	if (RT_UNLIKELY(!RT_DOUBLE_EQUALS(result64, expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_f_do(rt_f64 value, rt_un decimal_count, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_f32((rt_f32)value, decimal_count, expected))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f64(value, decimal_count, expected))) goto error;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_append_f64(value, 4, buffer, 200, &buffer_size))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_convert_to_f_do(buffer, value))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_f(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0, 3, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-0.1, 1, "-0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3.2, 0, "3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.5, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3.2, 1, "3.2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3, 1, "3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3.02, 2, "3.02"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3.02, 3, "3.02"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(3.002, 3, "3.002"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(123456, 3, "123456"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(12345.1, 3, "12345.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(12345.12, 3, "12345.12"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(12345.123, 3, "12345.123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(12345.1234, 3, "12345.123"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1234.1235, 3, "1234.124"))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.003, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.005, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.007, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.033, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.035, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.037, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.053, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.055, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.057, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.073, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.075, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.077, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.303, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.305, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.307, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.333, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.335, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.337, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.353, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.355, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.357, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.373, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.375, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.377, 0, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.503, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.505, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.507, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.533, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.535, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.537, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.553, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.555, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.557, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.573, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.575, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.577, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.703, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.705, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.707, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.733, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.735, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.737, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.753, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.755, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.757, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.773, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.775, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.777, 0, "-2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.003, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.005, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.007, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.033, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.035, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.037, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.053, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.055, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.057, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.073, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.075, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.077, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.303, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.305, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.307, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.333, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.335, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.337, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.353, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.355, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.357, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.373, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.375, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.377, 0, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.503, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.505, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.507, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.533, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.535, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.537, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.553, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.555, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.557, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.573, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.575, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.577, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.703, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.705, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.707, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.733, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.735, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.737, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.753, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.755, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.757, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.773, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.775, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.777, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.003, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.005, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.007, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.033, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.035, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.037, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.053, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.055, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.057, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.073, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.075, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.077, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.303, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.305, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.307, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.333, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.335, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.337, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.353, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.355, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.357, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.373, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.375, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.377, 0, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.503, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.505, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.507, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.533, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.535, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.537, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.553, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.555, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.557, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.573, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.575, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.577, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.703, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.705, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.707, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.733, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.735, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.737, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.753, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.755, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.757, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.773, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.775, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.777, 0, "2"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.003, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.005, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.007, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.033, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.035, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.037, 1, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.053, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.055, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.057, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.073, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.075, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.077, 1, "-1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.303, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.305, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.307, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.333, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.335, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.337, 1, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.353, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.355, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.357, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.373, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.375, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.377, 1, "-1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.503, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.505, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.507, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.533, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.535, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.537, 1, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.553, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.555, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.557, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.573, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.575, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.577, 1, "-1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.703, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.705, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.707, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.733, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.735, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.737, 1, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.753, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.755, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.757, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.773, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.775, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.777, 1, "-1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.003, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.005, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.007, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.033, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.035, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.037, 1, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.053, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.055, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.057, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.073, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.075, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.077, 1, "0.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.303, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.305, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.307, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.333, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.335, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.337, 1, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.353, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.355, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.357, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.373, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.375, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.377, 1, "0.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.503, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.505, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.507, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.533, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.535, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.537, 1, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.553, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.555, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.557, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.573, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.575, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.577, 1, "0.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.703, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.705, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.707, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.733, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.735, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.737, 1, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.753, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.755, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.757, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.773, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.775, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.777, 1, "0.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.003, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.005, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.007, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.033, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.035, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.037, 1, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.053, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.055, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.057, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.073, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.075, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.077, 1, "1.1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.303, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.305, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.307, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.333, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.335, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.337, 1, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.353, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.355, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.357, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.373, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.375, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.377, 1, "1.4"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.503, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.505, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.507, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.533, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.535, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.537, 1, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.553, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.555, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.557, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.573, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.575, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.577, 1, "1.6"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.703, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.705, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.707, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.733, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.735, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.737, 1, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.753, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.755, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.757, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.773, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.775, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.777, 1, "1.8"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.003, 2, "-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.005, 2, "-1.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.007, 2, "-1.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.033, 2, "-1.03"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.035, 2, "-1.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.037, 2, "-1.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.053, 2, "-1.05"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.055, 2, "-1.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.057, 2, "-1.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.073, 2, "-1.07"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.075, 2, "-1.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.077, 2, "-1.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.303, 2, "-1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.305, 2, "-1.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.307, 2, "-1.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.333, 2, "-1.33"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.335, 2, "-1.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.337, 2, "-1.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.353, 2, "-1.35"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.355, 2, "-1.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.357, 2, "-1.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.373, 2, "-1.37"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.375, 2, "-1.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.377, 2, "-1.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.503, 2, "-1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.505, 2, "-1.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.507, 2, "-1.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.533, 2, "-1.53"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.535, 2, "-1.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.537, 2, "-1.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.553, 2, "-1.55"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.555, 2, "-1.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.557, 2, "-1.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.573, 2, "-1.57"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.575, 2, "-1.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.577, 2, "-1.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.703, 2, "-1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.705, 2, "-1.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.707, 2, "-1.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.733, 2, "-1.73"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.735, 2, "-1.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.737, 2, "-1.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.753, 2, "-1.75"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.755, 2, "-1.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.757, 2, "-1.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.773, 2, "-1.77"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.775, 2, "-1.78"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(-1.777, 2, "-1.78"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.003, 2, "0"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.005, 2, "0.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.007, 2, "0.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.033, 2, "0.03"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.035, 2, "0.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.037, 2, "0.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.053, 2, "0.05"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.055, 2, "0.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.057, 2, "0.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.073, 2, "0.07"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.075, 2, "0.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.077, 2, "0.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.303, 2, "0.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.305, 2, "0.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.307, 2, "0.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.333, 2, "0.33"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.335, 2, "0.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.337, 2, "0.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.353, 2, "0.35"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.355, 2, "0.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.357, 2, "0.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.373, 2, "0.37"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.375, 2, "0.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.377, 2, "0.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.503, 2, "0.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.505, 2, "0.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.507, 2, "0.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.533, 2, "0.53"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.535, 2, "0.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.537, 2, "0.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.553, 2, "0.55"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.555, 2, "0.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.557, 2, "0.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.573, 2, "0.57"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.575, 2, "0.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.577, 2, "0.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.703, 2, "0.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.705, 2, "0.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.707, 2, "0.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.733, 2, "0.73"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.735, 2, "0.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.737, 2, "0.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.753, 2, "0.75"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.755, 2, "0.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.757, 2, "0.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.773, 2, "0.77"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.775, 2, "0.78"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(0.777, 2, "0.78"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.003, 2, "1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.005, 2, "1.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.007, 2, "1.01"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.033, 2, "1.03"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.035, 2, "1.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.037, 2, "1.04"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.053, 2, "1.05"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.055, 2, "1.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.057, 2, "1.06"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.073, 2, "1.07"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.075, 2, "1.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.077, 2, "1.08"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.303, 2, "1.3"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.305, 2, "1.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.307, 2, "1.31"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.333, 2, "1.33"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.335, 2, "1.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.337, 2, "1.34"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.353, 2, "1.35"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.355, 2, "1.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.357, 2, "1.36"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.373, 2, "1.37"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.375, 2, "1.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.377, 2, "1.38"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.503, 2, "1.5"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.505, 2, "1.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.507, 2, "1.51"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.533, 2, "1.53"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.535, 2, "1.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.537, 2, "1.54"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.553, 2, "1.55"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.555, 2, "1.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.557, 2, "1.56"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.573, 2, "1.57"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.575, 2, "1.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.577, 2, "1.58"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.703, 2, "1.7"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.705, 2, "1.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.707, 2, "1.71"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.733, 2, "1.73"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.735, 2, "1.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.737, 2, "1.74"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.753, 2, "1.75"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.755, 2, "1.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.757, 2, "1.76"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.773, 2, "1.77"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.775, 2, "1.78"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f_do(1.777, 2, "1.78"))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_lower_char(void)
{
	rt_s ret;

	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('@') != '@')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('A') != 'a')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('Z') != 'z')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('[') != '[')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('`') != '`')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('a') != 'a')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('z') != 'z')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('{') != '{')) goto error;
#ifdef RT_DEFINE_WINDOWS
#if ZZ_TEST_CHAR8_ACCENTED_CHARACTERS_CASE
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('È') != 'È')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_LOWER_CHAR('…') != '…')) goto error;
#endif
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_upper_char(void)
{
	rt_s ret;

	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('@') != '@')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('A') != 'A')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('Z') != 'Z')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('[') != '[')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('`') != '`')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('a') != 'A')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('z') != 'Z')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('{') != '{')) goto error;
#ifdef RT_DEFINE_WINDOWS
#if ZZ_TEST_CHAR8_ACCENTED_CHARACTERS_CASE
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('È') != 'È')) goto error;
	if (RT_UNLIKELY(RT_CHAR8_FAST_UPPER_CHAR('…') != '…')) goto error;
#endif
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_lower_or_upper_do(const rt_char8 *str, const rt_char8 *expected, rt_b upper)
{
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_copy(str, rt_char8_get_size(str), buffer, RT_CHAR8_HALF_BIG_STRING_SIZE))) goto error;
	if (upper) {
		result = rt_char8_fast_upper(buffer);
	} else {
		result = rt_char8_fast_lower(buffer);
	}
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != result)) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(expected) != result)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, result, expected, result))) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_lower_or_upper(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_fast_lower_or_upper_do("@AZ[`az{È…", "@az[`az{È…", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_fast_lower_or_upper_do("@AZ[`az{È…", "@AZ[`AZ{È…", RT_TRUE))) goto error;

	if (RT_UNLIKELY(!zz_test_char8_fast_lower_or_upper_do("", "", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_fast_lower_or_upper_do("", "", RT_TRUE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_successfully_convert_to_un(const rt_char8 *str, rt_un expected)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_convert_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char8_convert_to_un_with_size(str, rt_char8_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_failed_to_convert_to_un(const rt_char8 *str)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_convert_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char8_convert_to_un_with_size(str, rt_char8_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_un(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_un("0", 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_un("1", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_un("4294967295", 4294967295ul))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_un("18446744073709551615", 18446744073709551615ull))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un(""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un(" "))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("-"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("-a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("2a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_un("a2"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_successfully_convert_to_n(const rt_char8 *str, rt_n expected)
{
	rt_n result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_convert_to_n(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char8_convert_to_n_with_size(str, rt_char8_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_failed_to_convert_to_n(const rt_char8 *str)
{
	rt_n result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_convert_to_n(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char8_convert_to_n_with_size(str, rt_char8_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_n(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("0", 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("1", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("2147483647", 2147483647))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("-1", -1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("-0", 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("-2111222333", -2111222333))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_to_n("9223372036854775807", 9223372036854775807ll))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n(""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n(" "))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("-"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("1-"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("-a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("2a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_to_n("a2"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_successfully_convert_hex_to_un(const rt_char8 *str, rt_un expected)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_convert_hex_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char8_convert_hex_to_un_with_size(str, rt_char8_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_failed_to_convert_hex_to_un(const rt_char8 *str)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_convert_hex_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char8_convert_hex_to_un_with_size(str, rt_char8_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_hex_to_un(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("0", 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("1", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("5C9D2", 379346))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("5c9d2", 379346))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("AF0", 2800))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("af0", 2800))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("ffffffff", 4294967295ul))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char8_successfully_convert_hex_to_un("ffffffffffffffff", 18446744073709551615ull))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un(""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un(" "))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un("-1"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un("-"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un("-a"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un("G"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_failed_to_convert_hex_to_un("g"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_f(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_convert_to_f_do("0", 0.0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_to_f_do("123.456", 123.456))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_to_f_do("-23.7", -23.7))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_trim_do(rt_b left, rt_b right, const rt_char8 *str, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = rt_char8_get_size(str);
	if (RT_UNLIKELY(!rt_char8_copy(str, buffer_size, buffer, 200))) goto error;
	rt_char8_trim(left, right, buffer, &buffer_size);
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_trim(void)
{
	rt_s ret;

	/* Right. */
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "f",       "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "f ",      "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo",     "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo ",    "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo\t",   "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo  ",   "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo\t\t", "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, " foo",    " foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "fo o ",   "fo o"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "",        ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, " ",       ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "\t",      ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "  ",      ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "\t\t",    ""))) goto error;

	/* Left. */
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "f",       "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " f",      "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "foo",     "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " foo",    "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\tfoo",   "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "  foo",   "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t\tfoo", "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "foo ",    "foo "))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " fo o",   "fo o"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "",        ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " ",       ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t",      ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "  ",      ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t\t",    ""))) goto error;

	/* Both. */
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "f",           "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " f",          "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "f ",          "f"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "foo",         "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " foo ",       "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\tfoo\t",     "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "  foo  ",     "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t\tfoo\t\t", "foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " fo o ",      "fo o"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "",            ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " ",           ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t",          ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "  ",          ""))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t\t",        ""))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_left_pad_do(const rt_char8 *input, rt_un size, const rt_char8 *expected)
{
	rt_un input_size = rt_char8_get_size(input);
	rt_un expected_size = rt_char8_get_size(expected);
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	/* Buffer is too small. */
	if (RT_UNLIKELY(rt_char8_left_pad(input, input_size, 'O', size, buffer, expected_size, &buffer_size))) goto error;

	/* input != buffer. */
	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	if (RT_UNLIKELY(!rt_char8_left_pad(input, input_size, 'O', size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, expected_size))) goto error;

	/* input == buffer. */
	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	if (RT_UNLIKELY(!rt_char8_copy(input, input_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char8_left_pad(buffer, input_size, 'O', size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_left_pad(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("Foo", 6, "OOOFoo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("Foo", 3, "Foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("", 6, "OOOOOO"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("Foo", 16, "OOOOOOOOOOOOOFoo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("ABCDEFGHIJKLM", 16, "OOOABCDEFGHIJKLM"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad_do("ABCDEFGHIJKLM", 3, "ABCDEFGHIJKLM"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_right_pad_do(const rt_char8 *input, rt_un size, const rt_char8 *expected)
{
	rt_un expected_size = rt_char8_get_size(expected);
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	/* Buffer is too small. */
	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	buffer_size = rt_char8_get_size(input);
	if (buffer_size < size) {
		if (RT_UNLIKELY(!rt_char8_copy(input, buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE))) goto error;
		if (RT_UNLIKELY(rt_char8_right_pad('O', size, buffer, expected_size, &buffer_size))) goto error;
	}

	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	buffer_size = rt_char8_get_size(input);
	if (RT_UNLIKELY(!rt_char8_copy(input, buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char8_right_pad('O', size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_right_pad(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("Foo", 6, "FooOOO"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("Foo", 3, "Foo"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("", 6, "OOOOOO"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("Foo", 16, "FooOOOOOOOOOOOOO"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("ABCDEFGHIJKLM", 16, "ABCDEFGHIJKLMOOO"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad_do("ABCDEFGHIJKLM", 3, "ABCDEFGHIJKLM"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_ends_with_do(const rt_char8 *str, const rt_char8 *searched, rt_b expected)
{
	rt_b ends_with;
	rt_s ret;

	ends_with = rt_char8_ends_with(str, rt_char8_get_size(str), searched, rt_char8_get_size(searched));
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(ends_with, expected)))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_ends_with(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "o", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "oo", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "foo", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYA", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "ffoo", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "fo", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("foo", "", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("", "", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with_do("", "a", RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search_do(const rt_char8 *str, const rt_char8 *searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_search(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char8_search_with_size(str, rt_char8_get_size(str), searched, rt_char8_get_size(searched)) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_search_do("", "", RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("", "o", RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("o", "", RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("Foo", "oo", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("Foo", "o", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("Foo", "Fo", 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("Foo", "ob", RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("BarFo", "Foo", RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("BarFoo", "Fo", 3))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("FoFoFoFoFoo", "Foo", 8))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_do("FoFoFoFFoo", "Foo", 7))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search_char_do(const rt_char8 *str, rt_char8 searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_search_char(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char8_search_char_with_size(str, rt_char8_get_size(str), searched) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search_char(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_search_char_do("", 'a', RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("b", 'a', RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("a", 'a', 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("ab", 'a', 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("ba", 'a', 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("aba", 'a', 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("abcde", 'a', 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("abcde", 'b', 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("abcde", 'd', 3))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char_do("abcde", 'e', 4))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}


static rt_s zz_test_char8_count_occurrences_do(const rt_char8 *str, const rt_char8 *searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char8_count_occurrences(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char8_count_occurrences_with_size(str, rt_char8_get_size(str), searched, rt_char8_get_size(searched)) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_count_occurrences(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("foo", "foo", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("foofoo", "foo", 2))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("ofoofoo", "o", 5))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("foof", "o", 2))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("FoFoF", "FoF", 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences_do("FoFFoF", "FoF", 2))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_concat(void)
{
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	/* Buffer too small. */
	buffer_size = 0;
	if (RT_UNLIKELY(rt_char8_concat(buffer, 6, &buffer_size, "foo", "bar", RT_NULL))) goto error;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_char8_concat(buffer, 7, &buffer_size, "foo", "bar", RT_NULL))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, "foobar", 6))) goto error;

	if (RT_UNLIKELY(!rt_char8_concat(buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size, "", "t", "e", "a", "m", RT_NULL))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, "foobarteam", 10))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_replace_do(const rt_char8 *str, const rt_char8 *searched, const rt_char8 *replacement, const rt_char8 *expected)
{
	rt_un str_size = rt_char8_get_size(str);
	rt_un searched_size = rt_char8_get_size(searched);
	rt_un replacement_size = rt_char8_get_size(replacement);
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = 3;
	if (RT_UNLIKELY(!rt_char8_copy("XYZ", buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char8_replace(str, str_size, searched, searched_size, replacement, replacement_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char8_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_replace(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_replace_do("foo",       "foo", "bar",         "XYZbar")))                       goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foo",       "o",   "a",           "XYZfaa")))                       goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foobarfoo", "foo", "a",           "XYZabara")))                     goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foobarfoo", "foo", "long_string", "XYZlong_stringbarlong_string"))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foobarfoo", "foo", "",            "XYZbar")))                       goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foofoo",    "foo", "",            "XYZ")))                          goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("foobar",    "foo", "bar",         "XYZbarbar")))                    goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace_do("fobbar",    "foo", "bar",         "XYZfobbar")))                    goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_comparison_callback_do(const rt_char8 *str1, const rt_char8 *str2, rt_n expected)
{
	rt_n comparison_result;
	rt_s ret;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char8_comparison_callback(str1, str2, RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char8_comparison_with_size_callback(str1, rt_char8_get_size(str1), str2, rt_char8_get_size(str2), RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char8_comparison_callback(str2, str1, RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != -expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char8_comparison_with_size_callback(str2, rt_char8_get_size(str2), str1, rt_char8_get_size(str1), RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != -expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_comparison_callback(void)
{
	rt_s ret;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("Foo", "Foo", 0)))     goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("a",   "b",   -1)))    goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("A",   "a",   -0x20))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("",    "",    0)))     goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("a",   "",    97)))    goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback_do("ab",  "a",   98)))    goto error;
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_hash_do(const rt_char8 *data, rt_un expected)
{
	rt_un data_size = rt_char8_get_size(data);
	rt_un hash;
	rt_s ret;

	hash = rt_char8_hash(data, data_size);
	if (RT_UNLIKELY(hash != expected)) goto error;

	hash = 99;
	if (RT_UNLIKELY(!rt_char8_hash_callback(data, data_size, RT_NULL, &hash))) goto error;
	if (RT_UNLIKELY(hash != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_hash(void)
{
	rt_s ret;

#ifdef RT_DEFINE_32
	if (RT_UNLIKELY(!zz_test_char8_hash_do("Foo", 209589879u))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_hash_do("Bar", 3686563258u))) goto error;
#else
	if (RT_UNLIKELY(!zz_test_char8_hash_do("Foo", 17490737515057045975ul))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_hash_do("Bar", 1603655411640082298ul))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_split_do(const rt_char8 *str, const rt_char8 *delimiters, ...)
{
	va_list args_list;
	rt_char8 buffer[RT_CHAR8_HALF_BIG_STRING_SIZE];
	rt_char8 *parts[4];
	rt_un parts_size;
	rt_char8 *expected;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char8_copy(str, rt_char8_get_size(str), buffer, RT_CHAR8_HALF_BIG_STRING_SIZE))) goto error;

	if (RT_UNLIKELY(!rt_char8_split(buffer, delimiters, parts, 4, &parts_size))) goto error;

	va_start(args_list, delimiters);

	for (i = 0; i < parts_size; i++) {
		expected = va_arg(args_list, rt_char8*);
		if (RT_UNLIKELY(!expected))
			goto error;

		if (RT_UNLIKELY(!rt_char8_equals(parts[i], rt_char8_get_size(parts[i]), expected, rt_char8_get_size(expected))))
			goto error;
	}

	/* Check that expected is now empty. */
	expected = va_arg(args_list, rt_char8*);
	if (RT_UNLIKELY(expected))
		goto error;

	va_end(args_list);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_split(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_split_do("foo, bar", ", ", "foo", "bar", RT_NULL))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_split_do("foo, bar, team", ", ", "foo", "bar", "team", RT_NULL))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_split_do(" , ", ", ", RT_NULL))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_split_do(", ,foo, ,", " ,", "foo", RT_NULL))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_split_do(", ,foo, ,", ",", " ", "foo", " ", RT_NULL))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_is_empty_or_blank_do(const rt_char8 *str, rt_b expected)
{
	rt_b result;
	rt_s ret;

	result = rt_char8_is_empty_or_blank(str);
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, expected))) goto error;

	result = rt_char8_is_empty_or_blank_with_size(str, rt_char8_get_size(str));
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_is_empty_or_blank(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do("", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do(" ", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do("  ", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do("	", RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do("foo", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do(" foo", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do("foo ", RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank_do(" foo ", RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char8_get_size())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_equals())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_compare())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_copy())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_n())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_append_f())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_fast_lower_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_fast_upper_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_fast_lower_or_upper())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_to_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_to_n())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_hex_to_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_convert_to_f())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_trim())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_left_pad())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_right_pad())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_ends_with())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_search_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_count_occurrences())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_concat())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_replace())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_comparison_callback())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_hash())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_split())) goto error;
	if (RT_UNLIKELY(!zz_test_char8_is_empty_or_blank())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
