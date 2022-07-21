#include <rpr.h>

static rt_s zz_test_char_equals_single(rt_char *string1, rt_char *string2, rt_b expected)
{
	rt_s ret;

	if (!rt_char_equals(string1, rt_char_get_size(string1), string2, rt_char_get_size(string2)) != !expected) goto error;
	if (!rt_char_equals(string2, rt_char_get_size(string2), string1, rt_char_get_size(string1)) != !expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_equals()
{
	rt_s ret;

	if (!zz_test_char_equals_single(_R("Foo"),     _R("Foo"),      RT_TRUE))  goto error;
	if (!zz_test_char_equals_single(_R("foo"),     _R("Foo"),      RT_FALSE)) goto error;
	if (!zz_test_char_equals_single(_R("Foo"),     _R("Bar"),      RT_FALSE)) goto error;
	if (!zz_test_char_equals_single(_R(""),        _R(""),         RT_TRUE))  goto error;
	if (!zz_test_char_equals_single(_R("a"),       _R(""),         RT_FALSE)) goto error;
	if (!zz_test_char_equals_single(_R(""),        _R("a"),        RT_FALSE)) goto error;
	if (!zz_test_char_equals_single(_R("notsame"), _R("notsama"),  RT_FALSE)) goto error;
	if (!zz_test_char_equals_single(_R("notsame"), _R("notsamez"), RT_FALSE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_append_ok(const rt_char *prefix, const rt_char *suffix, const rt_char *expected)
{
	rt_s ret;
	rt_char buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (!rt_char_append(prefix, rt_char_get_size(prefix), buffer, 200, &buffer_size)) goto error;
	if (buffer_size != rt_char_get_size(prefix)) goto error;
	if (!rt_char_append(suffix, rt_char_get_size(suffix), buffer, 14, &buffer_size)) goto error;
	if (buffer_size != rt_char_get_size(prefix) + rt_char_get_size(suffix)) goto error;

	if (!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_append_failed(const rt_char *prefix, const rt_char *suffix, const rt_char *expected)
{
	rt_s ret;
	rt_char buffer[200];
	rt_un buffer_size = 0;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (!rt_char_append(prefix, rt_char_get_size(prefix), buffer, 200, &buffer_size)) goto error;
	if (buffer_size != rt_char_get_size(prefix)) goto error;
	if (rt_char_append(suffix, rt_char_get_size(suffix), buffer, 14, &buffer_size)) goto error;
	if (buffer_size != 13) goto error;

	if (!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_append()
{
	rt_s ret;

	if (!zz_test_char_append_ok(_R("Hello"),   _R(", World!"), _R("Hello, World!"))) goto error;
	if (!zz_test_char_append_ok(_R(""),        _R("Foo"),      _R("Foo")))           goto error;
	if (!zz_test_char_append_ok(_R("Bar"),     _R(""),         _R("Bar")))           goto error;
	if (!zz_test_char_append_ok(_R(""),        _R(""),         _R("")))              goto error;
	if (!zz_test_char_append_ok(_R("1234567"), _R("123456"),   _R("1234567123456"))) goto error;

	if (!zz_test_char_append_failed(_R("1234567"), _R("1234567"),  _R("1234567123456"))) goto error;
	if (!zz_test_char_append_failed(_R("1234567"), _R("12345678"), _R("1234567123456"))) goto error;

	if (!zz_test_char_append_failed(_R("12345678"),       _R("123456"), _R("1234567812345"))) goto error;
	if (!zz_test_char_append_failed(_R("123456789"),      _R("123456"), _R("1234567891234"))) goto error;
	if (!zz_test_char_append_failed(_R("1234567890"),     _R("123456"), _R("1234567890123"))) goto error;
	if (!zz_test_char_append_failed(_R("12345678901"),    _R("123456"), _R("1234567890112"))) goto error;
	if (!zz_test_char_append_failed(_R("123456789012"),   _R("123456"), _R("1234567890121"))) goto error;
	if (!zz_test_char_append_failed(_R("1234567890123"),  _R("123456"), _R("1234567890123"))) goto error;
	if (!zz_test_char_append_failed(_R("12345678901234"), _R("123456"), _R("1234567890123"))) goto error;
	if (!zz_test_char_append_failed(_R("12345678901235"), _R("123456"), _R("1234567890123"))) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_append_char_do(rt_s status, rt_un buffer_size, const rt_char *expected)
{
	rt_char buffer[10];
	rt_un local_buffer_size = buffer_size;
	rt_s ret;
	rt_s result;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 10);

	result = rt_char_append_char(_R('b'), buffer, 10, &local_buffer_size);
	if (!RT_MEMORY_XNOR(result, status))
		goto error;

	if (local_buffer_size != rt_char_get_size(expected))
		goto error;

	if (!rt_char_equals(buffer, local_buffer_size, expected, local_buffer_size))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_append_char()
{
	rt_s ret;

	if (!zz_test_char_append_char_do(RT_OK, 0, _R("b"))) goto error;
	if (!zz_test_char_append_char_do(RT_OK, 6, _R("aaaaaab"))) goto error;
	if (!zz_test_char_append_char_do(RT_OK, 7, _R("aaaaaaab"))) goto error;
	if (!zz_test_char_append_char_do(RT_OK, 8, _R("aaaaaaaab"))) goto error;
	if (!zz_test_char_append_char_do(RT_FAILED, 9, _R("aaaaaaaaa"))) goto error;
	if (!zz_test_char_append_char_do(RT_FAILED, 10, _R("aaaaaaaaa"))) goto error;
	if (!zz_test_char_append_char_do(RT_FAILED, 11, _R("aaaaaaaaa"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_copy_ok(const rt_char *str)
{
	rt_s ret;
	rt_char buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test append without prefix. */
	if (!rt_char_append(str, rt_char_get_size(str), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char_get_size(str)) goto error;
	if (!rt_char_equals(buffer, rt_char_get_size(buffer), str, rt_char_get_size(str))) goto error;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test copy. */
	if (!rt_char_copy(str, rt_char_get_size(str), buffer, 6)) goto error;
	if (!rt_char_equals(buffer, rt_char_get_size(buffer), str, rt_char_get_size(str))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_copy_failed(const rt_char *str, const rt_char *expected)
{
	rt_s ret;
	rt_char buffer[6];
	rt_un buffer_size = 0;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test append without prefix. */
	if (rt_char_append(str, rt_char_get_size(str), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char_get_size(expected)) goto error;
	if (!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected))) goto error;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test copy. */
	if (rt_char_copy(str, rt_char_get_size(str), buffer, 6)) goto error;
	if (!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_copy()
{
	rt_s ret;

	if (!zz_test_char_copy_ok(_R("Hello"))) goto error;
	if (!zz_test_char_copy_ok(_R(""))) goto error;
	if (!zz_test_char_copy_failed(_R("Hello1"), _R("Hello"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_get_size()
{
	rt_s ret;

	if (rt_char_get_size(_R("")) != 0) goto error;
	if (rt_char_get_size(_R("foo")) != 3) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_n_ok(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char *expected)
{
	rt_char buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (!rt_char_append_n(value, base, buffer, buffer_capacity, &local_buffer_size))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (rt_char_get_size(buffer) != expected_size) goto error;
	if (local_buffer_size != expected_size) goto error;
	if (!rt_char_equals(buffer, expected_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_n_failed(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char *expected)
{
	rt_char buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (rt_char_append_n(value, base, buffer, buffer_capacity, &local_buffer_size))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (rt_char_get_size(buffer) != expected_size) goto error;
	if (local_buffer_size != expected_size) goto error;
	if (!rt_char_equals(buffer, expected_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_n()
{
	rt_s ret;

	if (!zz_test_char_append_n_failed(12, 1, 7, 5, _R("aaaaa"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 6, 5, _R("aaaaa"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 5, 5, _R("aaaa"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 4, 5, _R("aaa"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 3, 5, _R("aa"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 2, 5, _R("a"))) goto error;
	if (!zz_test_char_append_n_failed(12, 1, 1, 5, _R(""))) goto error;

	if (!zz_test_char_append_n_ok(0, 10, 10, 0, _R("0"))) goto error;
	if (!zz_test_char_append_n_ok(1, 10, 10, 0, _R("1"))) goto error;
	if (!zz_test_char_append_n_ok(12, 10, 10, 0, _R("12"))) goto error;
	if (!zz_test_char_append_n_ok(123, 10, 10, 0, _R("123"))) goto error;
	if (!zz_test_char_append_n_ok(1234, 10, 10, 0, _R("1234"))) goto error;
	if (!zz_test_char_append_n_ok(12345, 10, 10, 0, _R("12345"))) goto error;

	if (!zz_test_char_append_n_ok(12345, 10, 10, 1, _R("a12345"))) goto error;
	if (!zz_test_char_append_n_ok(12345, 10, 10, 2, _R("aa12345"))) goto error;
	if (!zz_test_char_append_n_ok(12345, 10, 10, 3, _R("aaa12345"))) goto error;

	if (!zz_test_char_append_n_ok(12345, 10, 9, 3, _R("aaa12345"))) goto error;
	if (!zz_test_char_append_n_failed(12345, 10, 8, 3, _R("aaa"))) goto error;
	if (!zz_test_char_append_n_failed(12345, 10, 7, 3, _R("aaa"))) goto error;

	if (!zz_test_char_append_n_ok(-1234, 10, 9, 3, _R("aaa-1234"))) goto error;
	if (!zz_test_char_append_n_failed(-1234, 10, 8, 3, _R("aaa"))) goto error;
	if (!zz_test_char_append_n_failed(-1234, 10, 7, 3, _R("aaa"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_fast_lower_char()
{
	rt_s ret;

	if (RT_CHAR_FAST_LOWER_CHAR(_R('@')) != _R('@')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('A')) != _R('a')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('Z')) != _R('z')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('[')) != _R('[')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('`')) != _R('`')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('a')) != _R('a')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('z')) != _R('z')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('{')) != _R('{')) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_CHAR_FAST_LOWER_CHAR(_R('È')) != _R('È')) goto error;
	if (RT_CHAR_FAST_LOWER_CHAR(_R('…')) != _R('…')) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_fast_upper_char()
{
	rt_s ret;

	if (RT_CHAR_FAST_UPPER_CHAR(_R('@')) != _R('@')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('A')) != _R('A')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('Z')) != _R('Z')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('[')) != _R('[')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('`')) != _R('`')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('a')) != _R('A')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('z')) != _R('Z')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('{')) != _R('{')) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_CHAR_FAST_UPPER_CHAR(_R('È')) != _R('È')) goto error;
	if (RT_CHAR_FAST_UPPER_CHAR(_R('…')) != _R('…')) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_fast_lower_or_upper_do(const rt_char *str, const rt_char *expected, rt_b upper)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un result;
	rt_s ret;

	if (!rt_char_copy(str, rt_char_get_size(str), buffer, RT_CHAR_HALF_BIG_STRING_SIZE)) goto error;
	if (upper) {
		result = rt_char_fast_upper(buffer);
	} else {
		result = rt_char_fast_lower(buffer);
	}
	if (rt_char_get_size(buffer) != result) goto error;
	if (rt_char_get_size(expected) != result) goto error;
	if (!rt_char_equals(buffer, result, expected, result)) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_fast_lower_or_upper()
{
	rt_s ret;

	if (!zz_test_char_fast_lower_or_upper_do(_R("@AZ[`az{È…"), _R("@az[`az{È…"), RT_FALSE)) goto error;
	if (!zz_test_char_fast_lower_or_upper_do(_R("@AZ[`az{È…"), _R("@AZ[`AZ{È…"), RT_TRUE)) goto error;

	if (!zz_test_char_fast_lower_or_upper_do(_R(""), _R(""), RT_FALSE)) goto error;
	if (!zz_test_char_fast_lower_or_upper_do(_R(""), _R(""), RT_TRUE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char()
{
	rt_s ret;

	if (!zz_test_char_get_size()) goto error;
	if (!zz_test_char_equals()) goto error;
	if (!zz_test_char_append()) goto error;
	if (!zz_test_char_append_char()) goto error;
	if (!zz_test_char_copy()) goto error;
	if (!zz_test_char_append_n()) goto error;
	if (!zz_test_char_fast_lower_char()) goto error;
	if (!zz_test_char_fast_upper_char()) goto error;
	if (!zz_test_char_fast_lower_or_upper()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
