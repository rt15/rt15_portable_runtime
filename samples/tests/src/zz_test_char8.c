#include <rpr.h>

static rt_s zz_test_char8_equals_single(rt_char8 *string1, rt_char8 *string2, rt_b expected)
{
	rt_s ret;

	if (!rt_char8_equals(string1, rt_char8_get_size(string1), string2, rt_char8_get_size(string2)) != !expected) goto error;
	if (!rt_char8_equals(string2, rt_char8_get_size(string2), string1, rt_char8_get_size(string1)) != !expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_equals()
{
	rt_s ret;

	if (!zz_test_char8_equals_single("Foo",     "Foo",      RT_TRUE))  goto error;
	if (!zz_test_char8_equals_single("foo",     "Foo",      RT_FALSE)) goto error;
	if (!zz_test_char8_equals_single("Foo",     "Bar",      RT_FALSE)) goto error;
	if (!zz_test_char8_equals_single("",        "",         RT_TRUE))  goto error;
	if (!zz_test_char8_equals_single("a",       "",         RT_FALSE)) goto error;
	if (!zz_test_char8_equals_single("",        "a",        RT_FALSE)) goto error;
	if (!zz_test_char8_equals_single("notsame", "notsama",  RT_FALSE)) goto error;
	if (!zz_test_char8_equals_single("notsame", "notsamez", RT_FALSE)) goto error;

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
	char buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (!rt_char8_append(prefix, rt_char8_get_size(prefix), buffer, 200, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(prefix)) goto error;
	if (!rt_char8_append(suffix, rt_char8_get_size(suffix), buffer, 14, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(prefix) + rt_char8_get_size(suffix)) goto error;

	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected))) goto error;

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
	char buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (!rt_char8_append(prefix, rt_char8_get_size(prefix), buffer, 200, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(prefix)) goto error;
	if (rt_char8_append(suffix, rt_char8_get_size(suffix), buffer, 14, &buffer_size)) goto error;
	if (buffer_size != 13) goto error;

	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append()
{
	rt_s ret;

	if (!zz_test_char8_append_ok("Hello",   ", World!", "Hello, World!")) goto error;
	if (!zz_test_char8_append_ok("",        "Foo",      "Foo"))           goto error;
	if (!zz_test_char8_append_ok("Bar",     "",         "Bar"))           goto error;
	if (!zz_test_char8_append_ok("",        "",         ""))              goto error;
	if (!zz_test_char8_append_ok("1234567", "123456",   "1234567123456")) goto error;

	if (!zz_test_char8_append_failed("1234567", "1234567",  "1234567123456")) goto error;
	if (!zz_test_char8_append_failed("1234567", "12345678", "1234567123456")) goto error;

	if (!zz_test_char8_append_failed("12345678",       "123456", "1234567812345")) goto error;
	if (!zz_test_char8_append_failed("123456789",      "123456", "1234567891234")) goto error;
	if (!zz_test_char8_append_failed("1234567890",     "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failed("12345678901",    "123456", "1234567890112")) goto error;
	if (!zz_test_char8_append_failed("123456789012",   "123456", "1234567890121")) goto error;
	if (!zz_test_char8_append_failed("1234567890123",  "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failed("12345678901234", "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failed("12345678901235", "123456", "1234567890123")) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy_ok(const rt_char8 *string)
{
	rt_s ret;
	char buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test append without prefix. */
	if (!rt_char8_append(string, rt_char8_get_size(string), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(string)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), string, rt_char8_get_size(string))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (!rt_char8_copy(string, rt_char8_get_size(string), buffer, 6)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), string, rt_char8_get_size(string))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy_failed(const rt_char8 *string, const rt_char8 *expected)
{
	rt_s ret;
	char buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test append without prefix. */
	if (rt_char8_append(string, rt_char8_get_size(string), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(expected)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (rt_char8_copy(string, rt_char8_get_size(string), buffer, 6)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_copy()
{
	rt_s ret;

	if (!zz_test_char8_copy_ok("Hello")) goto error;
	if (!zz_test_char8_copy_ok("")) goto error;
	if (!zz_test_char8_copy_failed("Hello1", "Hello")) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_get_size()
{
	rt_s ret;

	if (rt_char8_get_size("") != 0) goto error;
	if (rt_char8_get_size("foo") != 3) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n_ok(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (!rt_char8_append_n(value, base, buffer, buffer_capacity, &local_buffer_size))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (rt_char8_get_size(buffer) != expected_size) goto error;
	if (local_buffer_size != expected_size) goto error;
	if (!rt_char8_equals(buffer, expected_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n_failed(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (rt_char8_append_n(value, base, buffer, buffer_capacity, &local_buffer_size))
		goto error;
	expected_size = rt_char8_get_size(expected);
	if (rt_char8_get_size(buffer) != expected_size) goto error;
	if (local_buffer_size != expected_size) goto error;
	if (!rt_char8_equals(buffer, expected_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8_append_n()
{
	rt_s ret;

	if (!zz_test_char8_append_n_failed(12, 1, 7, 5, "aaaaa")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 6, 5, "aaaaa")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 5, 5, "aaaa")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 4, 5, "aaa")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 3, 5, "aa")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 2, 5, "a")) goto error;
	if (!zz_test_char8_append_n_failed(12, 1, 1, 5, "")) goto error;

	if (!zz_test_char8_append_n_ok(0, 10, 10, 0, "0")) goto error;
	if (!zz_test_char8_append_n_ok(1, 10, 10, 0, "1")) goto error;
	if (!zz_test_char8_append_n_ok(12, 10, 10, 0, "12")) goto error;
	if (!zz_test_char8_append_n_ok(123, 10, 10, 0, "123")) goto error;
	if (!zz_test_char8_append_n_ok(1234, 10, 10, 0, "1234")) goto error;
	if (!zz_test_char8_append_n_ok(12345, 10, 10, 0, "12345")) goto error;

	if (!zz_test_char8_append_n_ok(12345, 10, 10, 1, "a12345")) goto error;
	if (!zz_test_char8_append_n_ok(12345, 10, 10, 2, "aa12345")) goto error;
	if (!zz_test_char8_append_n_ok(12345, 10, 10, 3, "aaa12345")) goto error;

	if (!zz_test_char8_append_n_ok(12345, 10, 9, 3, "aaa12345")) goto error;
	if (!zz_test_char8_append_n_failed(12345, 10, 8, 3, "aaa")) goto error;
	if (!zz_test_char8_append_n_failed(12345, 10, 7, 3, "aaa")) goto error;

	if (!zz_test_char8_append_n_ok(-1234, 10, 9, 3, "aaa-1234")) goto error;
	if (!zz_test_char8_append_n_failed(-1234, 10, 8, 3, "aaa")) goto error;
	if (!zz_test_char8_append_n_failed(-1234, 10, 7, 3, "aaa")) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char8()
{
	rt_s ret;

	if (!zz_test_char8_get_size()) goto error;
	if (!zz_test_char8_equals()) goto error;
	if (!zz_test_char8_append()) goto error;
	if (!zz_test_char8_copy()) goto error;
	if (!zz_test_char8_append_n()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
