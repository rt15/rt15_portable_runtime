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
	if (!rt_char8_append(suffix, rt_char8_get_size(suffix), buffer, 200, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(prefix) + rt_char8_get_size(suffix)) goto error;

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
	rt_char8 buffer[12];
	rt_un buffer_size = 0;
	rt_s ret;

	if (!zz_test_char8_append_ok("Hello", ", World!", "Hello, World!")) goto error;
	if (!zz_test_char8_append_ok("",      "Foo",      "Foo"))           goto error;
	if (!zz_test_char8_append_ok("Bar",   "",         "Bar"))           goto error;
	if (!zz_test_char8_append_ok("",      "",         ""))              goto error;

	if (!rt_char8_append("Hello,", rt_char8_get_size("Hello,"), buffer, 13, &buffer_size)) goto error;
	if (!rt_char8_append("Hello,", rt_char8_get_size(" world"), buffer, 13, &buffer_size)) goto error;

	/* Capacity should be to small for another character. */
	if (rt_char8_append("!", rt_char8_get_size("!"), buffer, 12, &buffer_size)) goto error;

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

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
