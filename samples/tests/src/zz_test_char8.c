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

static rt_s zz_test_char8_append_failure(const rt_char8 *prefix, const rt_char8 *suffix, const rt_char8 *expected)
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

	if (!zz_test_char8_append_failure("1234567", "1234567",  "1234567123456")) goto error;
	if (!zz_test_char8_append_failure("1234567", "12345678", "1234567123456")) goto error;

	if (!zz_test_char8_append_failure("12345678",       "123456", "1234567812345")) goto error;
	if (!zz_test_char8_append_failure("123456789",      "123456", "1234567891234")) goto error;
	if (!zz_test_char8_append_failure("1234567890",     "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failure("12345678901",    "123456", "1234567890112")) goto error;
	if (!zz_test_char8_append_failure("123456789012",   "123456", "1234567890121")) goto error;
	if (!zz_test_char8_append_failure("1234567890123",  "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failure("12345678901234", "123456", "1234567890123")) goto error;
	if (!zz_test_char8_append_failure("12345678901235", "123456", "1234567890123")) goto error;


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

	/* Test append. */
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

	/* Test append. */
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

rt_s zz_test_char8()
{
	rt_s ret;

	if (!zz_test_char8_get_size()) goto error;
	if (!zz_test_char8_equals()) goto error;
	if (!zz_test_char8_append()) goto error;
	if (!zz_test_char8_copy()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
