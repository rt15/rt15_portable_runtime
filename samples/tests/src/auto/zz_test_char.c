#include <rpr.h>

static rt_s zz_test_char_equals_single(rt_char *str1, rt_char *str2, rt_b expected)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_char_equals(str1, rt_char_get_size(str1), str2, rt_char_get_size(str2)) != !expected)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(str2, rt_char_get_size(str2), str1, rt_char_get_size(str1)) != !expected)) goto error;

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

	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("Foo"),     _R("Foo"),      RT_TRUE)))  goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("foo"),     _R("Foo"),      RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("Foo"),     _R("Bar"),      RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R(""),        _R(""),         RT_TRUE)))  goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("a"),       _R(""),         RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R(""),        _R("a"),        RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("notsame"), _R("notsama"),  RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals_single(_R("notsame"), _R("notsamez"), RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_compare()
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char_compare(_R("abc"), _R("abc")))) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R(""), _R("")))) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("abc"), _R("abd")) >= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("abd"), _R("abc")) <= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("abc"), _R("ab")) <= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("ab"), _R("abc")) >= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("a"), _R("È")) >= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("È"), _R("a")) <= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("a"), _R(" ")) <= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R(" "), _R("a")) >= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R("a"), _R("")) <= 0)) goto error;
	if (RT_UNLIKELY(rt_char_compare(_R(""), _R("a")) >= 0)) goto error;

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

	if (RT_UNLIKELY(!rt_char_append(prefix, rt_char_get_size(prefix), buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char_get_size(prefix))) goto error;
	if (RT_UNLIKELY(!rt_char_append(suffix, rt_char_get_size(suffix), buffer, 14, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char_get_size(prefix) + rt_char_get_size(suffix))) goto error;

	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected)))) goto error;

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

	if (RT_UNLIKELY(!rt_char_append(prefix, rt_char_get_size(prefix), buffer, 200, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char_get_size(prefix))) goto error;
	if (RT_UNLIKELY(rt_char_append(suffix, rt_char_get_size(suffix), buffer, 14, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != 13)) goto error;

	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected)))) goto error;

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

	if (RT_UNLIKELY(!zz_test_char_append_ok(_R("Hello"),   _R(", World!"), _R("Hello, World!")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_ok(_R(""),        _R("Foo"),      _R("Foo"))))           goto error;
	if (RT_UNLIKELY(!zz_test_char_append_ok(_R("Bar"),     _R(""),         _R("Bar"))))           goto error;
	if (RT_UNLIKELY(!zz_test_char_append_ok(_R(""),        _R(""),         _R(""))))              goto error;
	if (RT_UNLIKELY(!zz_test_char_append_ok(_R("1234567"), _R("123456"),   _R("1234567123456")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("1234567"), _R("1234567"),  _R("1234567123456")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("1234567"), _R("12345678"), _R("1234567123456")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("12345678"),       _R("123456"), _R("1234567812345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("123456789"),      _R("123456"), _R("1234567891234")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("1234567890"),     _R("123456"), _R("1234567890123")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("12345678901"),    _R("123456"), _R("1234567890112")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("123456789012"),   _R("123456"), _R("1234567890121")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("1234567890123"),  _R("123456"), _R("1234567890123")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("12345678901234"), _R("123456"), _R("1234567890123")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_failed(_R("12345678901235"), _R("123456"), _R("1234567890123")))) goto error;


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
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, status)))
		goto error;

	if (RT_UNLIKELY(local_buffer_size != rt_char_get_size(expected)))
		goto error;

	if (RT_UNLIKELY(!rt_char_equals(buffer, local_buffer_size, expected, local_buffer_size)))
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

	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_OK, 0, _R("b")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_OK, 6, _R("aaaaaab")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_OK, 7, _R("aaaaaaab")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_OK, 8, _R("aaaaaaaab")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_FAILED, 9, _R("aaaaaaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_FAILED, 10, _R("aaaaaaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char_do(RT_FAILED, 11, _R("aaaaaaaaa")))) goto error;

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
	if (RT_UNLIKELY(!rt_char_append(str, rt_char_get_size(str), buffer, 6, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char_get_size(str))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), str, rt_char_get_size(str)))) goto error;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test copy. */
	if (RT_UNLIKELY(!rt_char_copy(str, rt_char_get_size(str), buffer, 6))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), str, rt_char_get_size(str)))) goto error;

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
	if (RT_UNLIKELY(rt_char_append(str, rt_char_get_size(str), buffer, 6, &buffer_size))) goto error;
	if (RT_UNLIKELY(buffer_size != rt_char_get_size(expected))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected)))) goto error;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 6);

	/* Test copy. */
	if (RT_UNLIKELY(rt_char_copy(str, rt_char_get_size(str), buffer, 6))) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, rt_char_get_size(buffer), expected, rt_char_get_size(expected)))) goto error;

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

	if (RT_UNLIKELY(!zz_test_char_copy_ok(_R("Hello")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_copy_ok(_R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_copy_failed(_R("Hello1"), _R("Hello")))) goto error;

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

	if (RT_UNLIKELY(rt_char_get_size(_R("")) != 0)) goto error;
	if (RT_UNLIKELY(rt_char_get_size(_R("foo")) != 3)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_un_ok(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char *expected)
{
	rt_char buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (RT_UNLIKELY(!rt_char_append_un(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (RT_UNLIKELY(rt_char_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_un_failed(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char *expected)
{
	rt_char buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET_CHAR(buffer, _R('a'), 200);

	if (RT_UNLIKELY(rt_char_append_un(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (RT_UNLIKELY(rt_char_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, expected_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_char_append_un()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 7, 5, _R("aaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 6, 5, _R("aaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 5, 5, _R("aaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 4, 5, _R("aaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 3, 5, _R("aa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 2, 5, _R("a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12, 1, 1, 5, _R("")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_un_ok(0, 10, 10, 0, _R("0")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(1, 10, 10, 0, _R("1")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12, 10, 10, 0, _R("12")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(123, 10, 10, 0, _R("123")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(1234, 10, 10, 0, _R("1234")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12345, 10, 10, 0, _R("12345")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12345, 10, 10, 1, _R("a12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12345, 10, 10, 2, _R("aa12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12345, 10, 10, 3, _R("aaa12345")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_un_ok(12345, 10, 9, 3, _R("aaa12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12345, 10, 8, 3, _R("aaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un_failed(12345, 10, 7, 3, _R("aaa")))) goto error;

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

	if (RT_UNLIKELY(!rt_char_append_n(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (RT_UNLIKELY(rt_char_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, expected_size, expected, expected_size))) goto error;

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

	if (RT_UNLIKELY(rt_char_append_n(value, base, buffer, buffer_capacity, &local_buffer_size)))
		goto error;
	expected_size = rt_char_get_size(expected);
	if (RT_UNLIKELY(rt_char_get_size(buffer) != expected_size)) goto error;
	if (RT_UNLIKELY(local_buffer_size != expected_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, expected_size, expected, expected_size))) goto error;

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

	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 7, 5, _R("aaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 6, 5, _R("aaaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 5, 5, _R("aaaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 4, 5, _R("aaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 3, 5, _R("aa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 2, 5, _R("a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12, 1, 1, 5, _R("")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_n_ok(0, 10, 10, 0, _R("0")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(1, 10, 10, 0, _R("1")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12, 10, 10, 0, _R("12")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(123, 10, 10, 0, _R("123")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(1234, 10, 10, 0, _R("1234")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12345, 10, 10, 0, _R("12345")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12345, 10, 10, 1, _R("a12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12345, 10, 10, 2, _R("aa12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12345, 10, 10, 3, _R("aaa12345")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_n_ok(12345, 10, 9, 3, _R("aaa12345")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12345, 10, 8, 3, _R("aaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(12345, 10, 7, 3, _R("aaa")))) goto error;

	if (RT_UNLIKELY(!zz_test_char_append_n_ok(-1234, 10, 9, 3, _R("aaa-1234")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(-1234, 10, 8, 3, _R("aaa")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n_failed(-1234, 10, 7, 3, _R("aaa")))) goto error;

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

	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('@')) != _R('@'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('A')) != _R('a'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('Z')) != _R('z'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('[')) != _R('['))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('`')) != _R('`'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('a')) != _R('a'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('z')) != _R('z'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('{')) != _R('{'))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('È')) != _R('È'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_LOWER_CHAR(_R('…')) != _R('…'))) goto error;
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

	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('@')) != _R('@'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('A')) != _R('A'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('Z')) != _R('Z'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('[')) != _R('['))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('`')) != _R('`'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('a')) != _R('A'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('z')) != _R('Z'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('{')) != _R('{'))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('È')) != _R('È'))) goto error;
	if (RT_UNLIKELY(RT_CHAR_FAST_UPPER_CHAR(_R('…')) != _R('…'))) goto error;
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

	if (RT_UNLIKELY(!rt_char_copy(str, rt_char_get_size(str), buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (upper) {
		result = rt_char_fast_upper(buffer);
	} else {
		result = rt_char_fast_lower(buffer);
	}
	if (RT_UNLIKELY(rt_char_get_size(buffer) != result)) goto error;
	if (RT_UNLIKELY(rt_char_get_size(expected) != result)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, result, expected, result))) goto error;


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

	if (RT_UNLIKELY(!zz_test_char_fast_lower_or_upper_do(_R("@AZ[`az{È…"), _R("@az[`az{È…"), RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_fast_lower_or_upper_do(_R("@AZ[`az{È…"), _R("@AZ[`AZ{È…"), RT_TRUE))) goto error;

	if (RT_UNLIKELY(!zz_test_char_fast_lower_or_upper_do(_R(""), _R(""), RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_fast_lower_or_upper_do(_R(""), _R(""), RT_TRUE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_successfully_convert_to_un(const rt_char *str, rt_un expected)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char_convert_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char_convert_to_un_with_size(str, rt_char_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_failed_to_convert_to_un(const rt_char *str)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char_convert_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char_convert_to_un_with_size(str, rt_char_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_convert_to_un()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_un(_R("0"), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_un(_R("1"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_un(_R("4294967295"), 4294967295ul))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_un(_R("18446744073709551615"), 18446744073709551615ull))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R(" ")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("-1")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("-")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("-a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("2a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_un(_R("a2")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_successfully_convert_to_n(const rt_char *str, rt_n expected)
{
	rt_n result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char_convert_to_n(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char_convert_to_n_with_size(str, rt_char_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_failed_to_convert_to_n(const rt_char *str)
{
	rt_n result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char_convert_to_n(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char_convert_to_n_with_size(str, rt_char_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_convert_to_n()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("0"), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("1"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("2147483647"), 2147483647))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("-1"), -1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("-0"), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("-2111222333"), -2111222333))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_to_n(_R("9223372036854775807"), 9223372036854775807ll))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R(" ")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("-")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("1-")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("-a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("2a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_to_n(_R("a2")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_successfully_convert_hex_to_un(const rt_char *str, rt_un expected)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_char_convert_hex_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	result = 99;
	if (RT_UNLIKELY(!rt_char_convert_hex_to_un_with_size(str, rt_char_get_size(str), &result))) goto error;
	if (RT_UNLIKELY(result != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_failed_to_convert_hex_to_un(const rt_char *str)
{
	rt_un result;
	rt_s ret;

	if (RT_UNLIKELY(rt_char_convert_hex_to_un(str, &result))) goto error;
	if (RT_UNLIKELY(rt_char_convert_hex_to_un_with_size(str, rt_char_get_size(str), &result))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_convert_hex_to_un()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("0"), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("1"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("5C9D2"), 379346))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("5c9d2"), 379346))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("AF0"), 2800))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("af0"), 2800))) goto error;
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("ffffffff"), 4294967295ul))) goto error;
#ifdef RT_DEFINE_64
	if (RT_UNLIKELY(!zz_test_char_successfully_convert_hex_to_un(_R("ffffffffffffffff"), 18446744073709551615ull))) goto error;
#endif

	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R(" ")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("-1")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("-")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("-a")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("G")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_failed_to_convert_hex_to_un(_R("g")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_trim_do(rt_b left, rt_b right, const rt_char *str, const rt_char *expected)
{
	rt_char buffer[200];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = rt_char_get_size(str);
	if (RT_UNLIKELY(!rt_char_copy(str, buffer_size, buffer, 200))) goto error;
	rt_char_trim(left, right, buffer, &buffer_size);
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_trim()
{
	rt_s ret;

	/* Right. */
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("f"),       _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("f "),      _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("foo"),     _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("foo "),    _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("foo\t"),   _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("foo  "),   _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("foo\t\t"), _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R(" foo"),    _R(" foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("fo o "),   _R("fo o")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R(""),        _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R(" "),       _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("\t"),      _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("  "),      _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_FALSE, RT_TRUE, _R("\t\t"),    _R("")))) goto error;

	/* Left. */
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("f"),       _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R(" f"),      _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("foo"),     _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R(" foo"),    _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("\tfoo"),   _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("  foo"),   _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("\t\tfoo"), _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("foo "),    _R("foo ")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R(" fo o"),   _R("fo o")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R(""),        _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R(" "),       _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("\t"),      _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("  "),      _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_FALSE, _R("\t\t"),    _R("")))) goto error;

	/* Both. */
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("f"),           _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R(" f"),          _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("f "),          _R("f")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("foo"),         _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R(" foo "),       _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("\tfoo\t"),     _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("  foo  "),     _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("\t\tfoo\t\t"), _R("foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R(" fo o "),      _R("fo o")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R(""),            _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R(" "),           _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("\t"),          _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("  "),          _R("")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim_do(RT_TRUE, RT_TRUE, _R("\t\t"),        _R("")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_left_pad_do(const rt_char *input, rt_un size, const rt_char *expected)
{
	rt_un input_size = rt_char_get_size(input);
	rt_un expected_size = rt_char_get_size(expected);
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	/* Buffer is too small. */
	if (RT_UNLIKELY(rt_char_left_pad(input, input_size, _R('O'), size, buffer, expected_size, &buffer_size))) goto error;

	/* input != buffer. */
	for (i = 0; i < RT_CHAR_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = _R('z');
	if (RT_UNLIKELY(!rt_char_left_pad(input, input_size, _R('O'), size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, expected_size))) goto error;

	/* input == buffer. */
	for (i = 0; i < RT_CHAR_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = _R('z');
	if (RT_UNLIKELY(!rt_char_copy(input, input_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_left_pad(buffer, input_size, _R('O'), size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_left_pad()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R("Foo"), 6, _R("OOOFoo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R("Foo"), 3, _R("Foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R(""), 6, _R("OOOOOO")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R("Foo"), 16, _R("OOOOOOOOOOOOOFoo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R("ABCDEFGHIJKLM"), 16, _R("OOOABCDEFGHIJKLM")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad_do(_R("ABCDEFGHIJKLM"), 3, _R("ABCDEFGHIJKLM")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_right_pad_do(const rt_char *input, rt_un size, const rt_char *expected)
{
	rt_un expected_size = rt_char_get_size(expected);
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_un i;
	rt_s ret;

	/* Buffer is too small. */
	for (i = 0; i < RT_CHAR_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = _R('z');
	buffer_size = rt_char_get_size(input);
	if (buffer_size < size) {
		if (RT_UNLIKELY(!rt_char_copy(input, buffer_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto error;
		if (RT_UNLIKELY(rt_char_right_pad(_R('O'), size, buffer, expected_size, &buffer_size))) goto error;
	}

	for (i = 0; i < RT_CHAR_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = _R('z');
	buffer_size = rt_char_get_size(input);
	if (RT_UNLIKELY(!rt_char_copy(input, buffer_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_right_pad(_R('O'), size, buffer, expected_size + 1, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, expected_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_right_pad()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R("Foo"), 6, _R("FooOOO")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R("Foo"), 3, _R("Foo")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R(""), 6, _R("OOOOOO")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R("Foo"), 16, _R("FooOOOOOOOOOOOOO")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R("ABCDEFGHIJKLM"), 16, _R("ABCDEFGHIJKLMOOO")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad_do(_R("ABCDEFGHIJKLM"), 3, _R("ABCDEFGHIJKLM")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_ends_with_do(const rt_char *str, const rt_char *searched, rt_b expected)
{
	rt_b ends_with;
	rt_s ret;

	ends_with = rt_char_ends_with(str, rt_char_get_size(str), searched, rt_char_get_size(searched));
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(ends_with, expected)))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_ends_with()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R("o"), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R("oo"), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R("foo"), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ"), _R("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ"), _R("ABCDEFGHIJKLMNOPQRSTUVWXYA"), RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R("ffoo"), RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R("fo"), RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R("foo"), _R(""), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R(""), _R(""), RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with_do(_R(""), _R("a"), RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_search_do(const rt_char *str, const rt_char *searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char_search(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char_search_with_size(str, rt_char_get_size(str), searched, rt_char_get_size(searched)) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_search()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_search_do(_R(""), _R(""), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R(""), _R("o"), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("o"), _R(""), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("Foo"), _R("oo"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("Foo"), _R("o"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("Foo"), _R("Fo"), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("Foo"), _R("ob"), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("BarFo"), _R("Foo"), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("BarFoo"), _R("Fo"), 3))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("FoFoFoFoFoo"), _R("Foo"), 8))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_do(_R("FoFoFoFFoo"), _R("Foo"), 7))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_search_char_do(const rt_char *str, rt_char searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char_search_char(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char_search_char_with_size(str, rt_char_get_size(str), searched) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_search_char()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R(""), _R('a'), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("b"), _R('a'), RT_TYPE_MAX_UN))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("a"), _R('a'), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("ab"), _R('a'), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("ba"), _R('a'), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("aba"), _R('a'), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("abcde"), _R('a'), 0))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("abcde"), _R('b'), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("abcde"), _R('d'), 3))) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char_do(_R("abcde"), _R('e'), 4))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}


static rt_s zz_test_char_count_occurrences_do(const rt_char *str, const rt_char *searched, rt_un expected)
{
	rt_s ret;

	if (RT_UNLIKELY(rt_char_count_occurrences(str, searched) != expected)) goto error;
	if (RT_UNLIKELY(rt_char_count_occurrences_with_size(str, rt_char_get_size(str), searched, rt_char_get_size(searched)) != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_count_occurrences()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("foo"), _R("foo"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("foofoo"), _R("foo"), 2))) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("ofoofoo"), _R("o"), 5))) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("foof"), _R("o"), 2))) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("FoFoF"), _R("FoF"), 1))) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences_do(_R("FoFFoF"), _R("FoF"), 2))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_concat()
{
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	/* Buffer too small. */
	buffer_size = 0;
	if (RT_UNLIKELY(rt_char_concat(buffer, 6, &buffer_size, _R("foo"), _R("bar"), RT_NULL))) goto error;

	buffer_size = 0;
	if (RT_UNLIKELY(!rt_char_concat(buffer, 7, &buffer_size, _R("foo"), _R("bar"), RT_NULL))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, _R("foobar"), 6))) goto error;

	if (RT_UNLIKELY(!rt_char_concat(buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size, _R(""), _R("t"), _R("e"), _R("a"), _R("m"), RT_NULL))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, _R("foobarteam"), 10))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_replace_do(const rt_char *str, const rt_char *searched, const rt_char *replacement, const rt_char *expected)
{
	rt_un str_size = rt_char_get_size(str);
	rt_un searched_size = rt_char_get_size(searched);
	rt_un replacement_size = rt_char_get_size(replacement);
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = 3;
	if (RT_UNLIKELY(!rt_char_copy(_R("XYZ"), buffer_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_replace(str, str_size, searched, searched_size, replacement, replacement_size, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;
	if (RT_UNLIKELY(!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected)))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_replace()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foo"),       _R("foo"), _R("bar"),         _R("XYZbar"))))                       goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foo"),       _R("o"),   _R("a"),           _R("XYZfaa"))))                       goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foobarfoo"), _R("foo"), _R("a"),           _R("XYZabara"))))                     goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foobarfoo"), _R("foo"), _R("long_string"), _R("XYZlong_stringbarlong_string")))) goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foobarfoo"), _R("foo"), _R(""),            _R("XYZbar"))))                       goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foofoo"),    _R("foo"), _R(""),            _R("XYZ"))))                          goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("foobar"),    _R("foo"), _R("bar"),         _R("XYZbarbar"))))                    goto error;
	if (RT_UNLIKELY(!zz_test_char_replace_do(_R("fobbar"),    _R("foo"), _R("bar"),         _R("XYZfobbar"))))                    goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_comparison_callback_do(const rt_char *str1, const rt_char *str2, rt_n expected)
{
	rt_n comparison_result;
	rt_s ret;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char_comparison_callback(str1, str2, RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char_comparison_with_size_callback(str1, rt_char_get_size(str1), str2, rt_char_get_size(str2), RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char_comparison_callback(str2, str1, RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != -expected)) goto error;

	comparison_result = 99;
	if (RT_UNLIKELY(!rt_char_comparison_with_size_callback(str2, rt_char_get_size(str2), str1, rt_char_get_size(str1), RT_NULL, &comparison_result))) goto error;
	if (RT_UNLIKELY(comparison_result != -expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_comparison_callback()
{
	rt_s ret;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R("Foo"), _R("Foo"), 0)))     goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R("a"),   _R("b"),   -1)))    goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R("A"),   _R("a"),   -0x20))) goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R(""),    _R(""),    0)))     goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R("a"),   _R(""),    97)))    goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback_do(_R("ab"),  _R("a"),   98)))    goto error;
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_hash_do(const rt_char *data, rt_un expected)
{
	rt_un data_size = rt_char_get_size(data);
	rt_un hash;
	rt_s ret;

	hash = rt_char_hash(data, data_size);
	if (RT_UNLIKELY(hash != expected)) goto error;

	hash = 99;
	if (RT_UNLIKELY(!rt_char_hash_callback(data, data_size, RT_NULL, &hash))) goto error;
	if (RT_UNLIKELY(hash != expected)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char_hash()
{
	rt_s ret;

#ifdef RT_DEFINE_32
	if (RT_UNLIKELY(!zz_test_char_hash_do(_R("Foo"), 209589879u))) goto error;
	if (RT_UNLIKELY(!zz_test_char_hash_do(_R("Bar"), 3686563258u))) goto error;
#else
	if (RT_UNLIKELY(!zz_test_char_hash_do(_R("Foo"), 17490737515057045975ul))) goto error;
	if (RT_UNLIKELY(!zz_test_char_hash_do(_R("Bar"), 1603655411640082298ul))) goto error;
#endif

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

	if (RT_UNLIKELY(!zz_test_char_get_size())) goto error;
	if (RT_UNLIKELY(!zz_test_char_equals())) goto error;
	if (RT_UNLIKELY(!zz_test_char_compare())) goto error;
	if (RT_UNLIKELY(!zz_test_char_append())) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char_copy())) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char_append_n())) goto error;
	if (RT_UNLIKELY(!zz_test_char_fast_lower_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char_fast_upper_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char_fast_lower_or_upper())) goto error;
	if (RT_UNLIKELY(!zz_test_char_convert_to_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char_convert_to_n())) goto error;
	if (RT_UNLIKELY(!zz_test_char_convert_hex_to_un())) goto error;
	if (RT_UNLIKELY(!zz_test_char_trim())) goto error;
	if (RT_UNLIKELY(!zz_test_char_left_pad())) goto error;
	if (RT_UNLIKELY(!zz_test_char_right_pad())) goto error;
	if (RT_UNLIKELY(!zz_test_char_ends_with())) goto error;
	if (RT_UNLIKELY(!zz_test_char_search())) goto error;
	if (RT_UNLIKELY(!zz_test_char_search_char())) goto error;
	if (RT_UNLIKELY(!zz_test_char_count_occurrences())) goto error;
	if (RT_UNLIKELY(!zz_test_char_concat())) goto error;
	if (RT_UNLIKELY(!zz_test_char_replace())) goto error;
	if (RT_UNLIKELY(!zz_test_char_comparison_callback())) goto error;
	if (RT_UNLIKELY(!zz_test_char_hash())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
