#include <rpr.h>

static rt_s zz_test_char8_equals_single(rt_char8 *str1, rt_char8 *str2, rt_b expected)
{
	rt_s ret;

	if (!rt_char8_equals(str1, rt_char8_get_size(str1), str2, rt_char8_get_size(str2)) != !expected) goto error;
	if (!rt_char8_equals(str2, rt_char8_get_size(str2), str1, rt_char8_get_size(str1)) != !expected) goto error;

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

static rt_s zz_test_char8_compare()
{
	rt_s ret;

	if (rt_char8_compare("abc", "abc")) goto error;
	if (rt_char8_compare("", "")) goto error;
	if (rt_char8_compare("abc", "abd") >= 0) goto error;
	if (rt_char8_compare("abd", "abc") <= 0) goto error;
	if (rt_char8_compare("abc", "ab") <= 0) goto error;
	if (rt_char8_compare("ab", "abc") >= 0) goto error;
	if (rt_char8_compare("a", "È") >= 0) goto error;
	if (rt_char8_compare("È", "a") <= 0) goto error;
	if (rt_char8_compare("a", " ") <= 0) goto error;
	if (rt_char8_compare(" ", "a") >= 0) goto error;
	if (rt_char8_compare("a", "") <= 0) goto error;
	if (rt_char8_compare("", "a") >= 0) goto error;

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
	rt_char8 buffer[200];
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

static rt_s zz_test_char8_append_char_do(rt_s status, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[10];
	rt_un local_buffer_size = buffer_size;
	rt_s ret;
	rt_s result;

	RT_MEMORY_SET(buffer, 'a', 10);

	result = rt_char8_append_char('b', buffer, 10, &local_buffer_size);
	if (!RT_MEMORY_XNOR(result, status))
		goto error;

	if (local_buffer_size != rt_char8_get_size(expected))
		goto error;

	if (!rt_char8_equals(buffer, local_buffer_size, expected, local_buffer_size))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_append_char()
{
	rt_s ret;

	if (!zz_test_char8_append_char_do(RT_OK, 0, "b")) goto error;
	if (!zz_test_char8_append_char_do(RT_OK, 6, "aaaaaab")) goto error;
	if (!zz_test_char8_append_char_do(RT_OK, 7, "aaaaaaab")) goto error;
	if (!zz_test_char8_append_char_do(RT_OK, 8, "aaaaaaaab")) goto error;
	if (!zz_test_char8_append_char_do(RT_FAILED, 9, "aaaaaaaaa")) goto error;
	if (!zz_test_char8_append_char_do(RT_FAILED, 10, "aaaaaaaaa")) goto error;
	if (!zz_test_char8_append_char_do(RT_FAILED, 11, "aaaaaaaaa")) goto error;

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
	if (!rt_char8_append(str, rt_char8_get_size(str), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(str)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), str, rt_char8_get_size(str))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (!rt_char8_copy(str, rt_char8_get_size(str), buffer, 6)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), str, rt_char8_get_size(str))) goto error;

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
	if (rt_char8_append(str, rt_char8_get_size(str), buffer, 6, &buffer_size)) goto error;
	if (buffer_size != rt_char8_get_size(expected)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(buffer), expected, rt_char8_get_size(expected))) goto error;

	RT_MEMORY_SET(buffer, 'a', 6);

	/* Test copy. */
	if (rt_char8_copy(str, rt_char8_get_size(str), buffer, 6)) goto error;
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

rt_s zz_test_char8_append_un_ok(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (!rt_char8_append_un(value, base, buffer, buffer_capacity, &local_buffer_size))
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

rt_s zz_test_char8_append_un_failed(rt_un value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
{
	rt_char8 buffer[200];
	rt_un local_buffer_size = buffer_size;
	rt_un expected_size;
	rt_s ret;

	RT_MEMORY_SET(buffer, 'a', 200);

	if (rt_char8_append_un(value, base, buffer, buffer_capacity, &local_buffer_size))
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

rt_s zz_test_char8_append_un()
{
	rt_s ret;

	if (!zz_test_char8_append_un_failed(12, 1, 7, 5, "aaaaa")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 6, 5, "aaaaa")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 5, 5, "aaaa")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 4, 5, "aaa")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 3, 5, "aa")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 2, 5, "a")) goto error;
	if (!zz_test_char8_append_un_failed(12, 1, 1, 5, "")) goto error;

	if (!zz_test_char8_append_un_ok(0, 10, 10, 0, "0")) goto error;
	if (!zz_test_char8_append_un_ok(1, 10, 10, 0, "1")) goto error;
	if (!zz_test_char8_append_un_ok(12, 10, 10, 0, "12")) goto error;
	if (!zz_test_char8_append_un_ok(123, 10, 10, 0, "123")) goto error;
	if (!zz_test_char8_append_un_ok(1234, 10, 10, 0, "1234")) goto error;
	if (!zz_test_char8_append_un_ok(12345, 10, 10, 0, "12345")) goto error;

	if (!zz_test_char8_append_un_ok(12345, 10, 10, 1, "a12345")) goto error;
	if (!zz_test_char8_append_un_ok(12345, 10, 10, 2, "aa12345")) goto error;
	if (!zz_test_char8_append_un_ok(12345, 10, 10, 3, "aaa12345")) goto error;

	if (!zz_test_char8_append_un_ok(12345, 10, 9, 3, "aaa12345")) goto error;
	if (!zz_test_char8_append_un_failed(12345, 10, 8, 3, "aaa")) goto error;
	if (!zz_test_char8_append_un_failed(12345, 10, 7, 3, "aaa")) goto error;

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

rt_s zz_test_char8_append_n_failed(rt_n value, rt_un base, rt_un buffer_capacity, rt_un buffer_size, const rt_char8 *expected)
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

static rt_s zz_test_char8_fast_lower_char()
{
	rt_s ret;

	if (RT_CHAR8_FAST_LOWER_CHAR('@') != '@') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('A') != 'a') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('Z') != 'z') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('[') != '[') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('`') != '`') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('a') != 'a') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('z') != 'z') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('{') != '{') goto error;

	/* Execution charset is uTF-8 so this cannot work.
	if (RT_CHAR8_FAST_LOWER_CHAR('È') != 'È') goto error;
	if (RT_CHAR8_FAST_LOWER_CHAR('…') != '…') goto error;
	*/

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_upper_char()
{
	rt_s ret;

	if (RT_CHAR8_FAST_UPPER_CHAR('@') != '@') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('A') != 'A') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('Z') != 'Z') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('[') != '[') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('`') != '`') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('a') != 'A') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('z') != 'Z') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('{') != '{') goto error;

	/* Execution charset is uTF-8 so this cannot work.
	if (RT_CHAR8_FAST_UPPER_CHAR('È') != 'È') goto error;
	if (RT_CHAR8_FAST_UPPER_CHAR('…') != '…') goto error;
	*/

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

	if (!rt_char8_copy(str, rt_char8_get_size(str), buffer, RT_CHAR8_HALF_BIG_STRING_SIZE)) goto error;
	if (upper) {
		result = rt_char8_fast_upper(buffer);
	} else {
		result = rt_char8_fast_lower(buffer);
	}
	if (rt_char8_get_size(buffer) != result) goto error;
	if (rt_char8_get_size(expected) != result) goto error;
	if (!rt_char8_equals(buffer, result, expected, result)) goto error;


	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_fast_lower_or_upper()
{
	rt_s ret;

	if (!zz_test_char8_fast_lower_or_upper_do("@AZ[`az{È…", "@az[`az{È…", RT_FALSE)) goto error;
	if (!zz_test_char8_fast_lower_or_upper_do("@AZ[`az{È…", "@AZ[`AZ{È…", RT_TRUE)) goto error;

	if (!zz_test_char8_fast_lower_or_upper_do("", "", RT_FALSE)) goto error;
	if (!zz_test_char8_fast_lower_or_upper_do("", "", RT_TRUE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_successful_char_convert_to_un(const rt_char8 *str, rt_un expected)
{
	rt_un result;
	rt_s ret;

	if (!rt_char8_convert_to_un(str, &result)) goto error;
	if (result != expected) goto error;

	result = 99;
	if (!rt_char8_convert_to_un_with_size(str, rt_char8_get_size(str), &result)) goto error;
	if (result != expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_failed_char_convert_to_un(const rt_char8 *str)
{
	rt_un result;
	rt_s ret;

	if (rt_char8_convert_to_un(str, &result)) goto error;
	if (rt_char8_convert_to_un_with_size(str, rt_char8_get_size(str), &result)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_un()
{
	rt_s ret;

	if (!zz_test_successful_char_convert_to_un("0", 0)) goto error;
	if (!zz_test_successful_char_convert_to_un("1", 1)) goto error;
	if (!zz_test_successful_char_convert_to_un("4294967296", 4294967296)) goto error;
	/* TODO: Test larger numbers under 64 bits. */

	if (!zz_test_failed_char_convert_to_un("")) goto error;
	if (!zz_test_failed_char_convert_to_un(" ")) goto error;
	if (!zz_test_failed_char_convert_to_un("-1")) goto error;
	if (!zz_test_failed_char_convert_to_un("-")) goto error;
	if (!zz_test_failed_char_convert_to_un("-a")) goto error;
	if (!zz_test_failed_char_convert_to_un("a")) goto error;
	if (!zz_test_failed_char_convert_to_un("2a")) goto error;
	if (!zz_test_failed_char_convert_to_un("a2")) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_successful_char_convert_to_n(const rt_char8 *str, rt_n expected)
{
	rt_n result;
	rt_s ret;

	if (!rt_char8_convert_to_n(str, &result)) goto error;
	if (result != expected) goto error;

	result = 99;
	if (!rt_char8_convert_to_n_with_size(str, rt_char8_get_size(str), &result)) goto error;
	if (result != expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_failed_char_convert_to_n(const rt_char8 *str)
{
	rt_n result;
	rt_s ret;

	if (rt_char8_convert_to_n(str, &result)) goto error;
	if (rt_char8_convert_to_n_with_size(str, rt_char8_get_size(str), &result)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_convert_to_n()
{
	rt_s ret;

	if (!zz_test_successful_char_convert_to_n("0", 0)) goto error;
	if (!zz_test_successful_char_convert_to_n("1", 1)) goto error;
	if (!zz_test_successful_char_convert_to_n("2147483647", 2147483647)) goto error;
	if (!zz_test_successful_char_convert_to_n("-1", -1)) goto error;
	if (!zz_test_successful_char_convert_to_n("-0", 0)) goto error;
	if (!zz_test_successful_char_convert_to_n("-2111222333", -2111222333)) goto error;
	/* TODO: Test larger numbers under 64 bits. */

	if (!zz_test_failed_char_convert_to_n("")) goto error;
	if (!zz_test_failed_char_convert_to_n(" ")) goto error;
	if (!zz_test_failed_char_convert_to_n("-")) goto error;
	if (!zz_test_failed_char_convert_to_n("1-")) goto error;
	if (!zz_test_failed_char_convert_to_n("-a")) goto error;
	if (!zz_test_failed_char_convert_to_n("a")) goto error;
	if (!zz_test_failed_char_convert_to_n("2a")) goto error;
	if (!zz_test_failed_char_convert_to_n("a2")) goto error;

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
	if (!rt_char8_copy(str, buffer_size, buffer, 200)) goto error;
	rt_char8_trim(left, right, buffer, &buffer_size);
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_trim()
{
	rt_s ret;

	/* Right. */
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "f",       "f")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "f ",      "f")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo",     "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo ",    "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo\t",   "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo  ",   "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "foo\t\t", "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, " foo",    " foo")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "fo o ",   "fo o")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "",        "")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, " ",       "")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "\t",      "")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "  ",      "")) goto error;
	if (!zz_test_char8_trim_do(RT_FALSE, RT_TRUE, "\t\t",    "")) goto error;

	/* Left. */
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "f",       "f")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " f",      "f")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "foo",     "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " foo",    "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\tfoo",   "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "  foo",   "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t\tfoo", "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "foo ",    "foo ")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " fo o",   "fo o")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "",        "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, " ",       "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t",      "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "  ",      "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_FALSE, "\t\t",    "")) goto error;

	/* Both. */
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "f",           "f")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " f",          "f")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "f ",          "f")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "foo",         "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " foo ",       "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\tfoo\t",     "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "  foo  ",     "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t\tfoo\t\t", "foo")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " fo o ",      "fo o")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "",            "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, " ",           "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t",          "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "  ",          "")) goto error;
	if (!zz_test_char8_trim_do(RT_TRUE, RT_TRUE, "\t\t",        "")) goto error;

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
	if (rt_char8_left_pad(input, input_size, 'O', size, buffer, expected_size, &buffer_size)) goto error;

	/* input != buffer. */
	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	if (!rt_char8_left_pad(input, input_size, 'O', size, buffer, expected_size + 1, &buffer_size)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, expected, expected_size)) goto error;

	/* input == buffer. */
	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	if (!rt_char8_copy(input, input_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE)) goto error;
	if (!rt_char8_left_pad(buffer, input_size, 'O', size, buffer, expected_size + 1, &buffer_size)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_left_pad()
{
	rt_s ret;

	if (!zz_test_char8_left_pad_do("Foo", 6, "OOOFoo")) goto error;
	if (!zz_test_char8_left_pad_do("Foo", 3, "Foo")) goto error;
	if (!zz_test_char8_left_pad_do("", 6, "OOOOOO")) goto error;
	if (!zz_test_char8_left_pad_do("Foo", 16, "OOOOOOOOOOOOOFoo")) goto error;
	if (!zz_test_char8_left_pad_do("ABCDEFGHIJKLM", 16, "OOOABCDEFGHIJKLM")) goto error;
	if (!zz_test_char8_left_pad_do("ABCDEFGHIJKLM", 3, "ABCDEFGHIJKLM")) goto error;

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
		if (!rt_char8_copy(input, buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE)) goto error;
		if (rt_char8_right_pad('O', size, buffer, expected_size, &buffer_size)) goto error;
	}

	for (i = 0; i < RT_CHAR8_QUARTER_BIG_STRING_SIZE; i++)
		buffer[i] = 'z';
	buffer_size = rt_char8_get_size(input);
	if (!rt_char8_copy(input, buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE)) goto error;
	if (!rt_char8_right_pad('O', size, buffer, expected_size + 1, &buffer_size)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, expected, expected_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_right_pad()
{
	rt_s ret;

	if (!zz_test_char8_right_pad_do("Foo", 6, "FooOOO")) goto error;
	if (!zz_test_char8_right_pad_do("Foo", 3, "Foo")) goto error;
	if (!zz_test_char8_right_pad_do("", 6, "OOOOOO")) goto error;
	if (!zz_test_char8_right_pad_do("Foo", 16, "FooOOOOOOOOOOOOO")) goto error;
	if (!zz_test_char8_right_pad_do("ABCDEFGHIJKLM", 16, "ABCDEFGHIJKLMOOO")) goto error;
	if (!zz_test_char8_right_pad_do("ABCDEFGHIJKLM", 3, "ABCDEFGHIJKLM")) goto error;

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
	if (!RT_MEMORY_XNOR(ends_with, expected))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_ends_with()
{
	rt_s ret;

	if (!zz_test_char8_ends_with_do("foo", "o", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("foo", "oo", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("foo", "foo", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("ZZZZZZABCDEFGHIJKLMNOPQRSTUVWXYZ", "ABCDEFGHIJKLMNOPQRSTUVWXYA", RT_FALSE)) goto error;
	if (!zz_test_char8_ends_with_do("foo", "ffoo", RT_FALSE)) goto error;
	if (!zz_test_char8_ends_with_do("foo", "fo", RT_FALSE)) goto error;
	if (!zz_test_char8_ends_with_do("foo", "", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("", "", RT_TRUE)) goto error;
	if (!zz_test_char8_ends_with_do("", "a", RT_FALSE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search()
{
	rt_s ret;

	if (rt_char8_search("", "") != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search("", "o") != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search("o", "") != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search("Foo", "oo") != 1) goto error;
	if (rt_char8_search("Foo", "o") != 1) goto error;
	if (rt_char8_search("Foo", "Fo") != 0) goto error;
	if (rt_char8_search("Foo", "ob") != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search("BarFo", "Foo") != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search("BarFoo", "Fo") != 3) goto error;
	if (rt_char8_search("FoFoFoFoFoo", "Foo") != 8) goto error;
	if (rt_char8_search("FoFoFoFFoo", "Foo") != 7) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_search_char()
{
	rt_s ret;

	if (rt_char8_search_char("", 'a') != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search_char("b", 'a') != RT_TYPE_MAX_UN) goto error;
	if (rt_char8_search_char("a", 'a') != 0) goto error;
	if (rt_char8_search_char("ab", 'a') != 0) goto error;
	if (rt_char8_search_char("ba", 'a') != 1) goto error;
	if (rt_char8_search_char("aba", 'a') != 0) goto error;
	if (rt_char8_search_char("abcde", 'a') != 0) goto error;
	if (rt_char8_search_char("abcde", 'b') != 1) goto error;
	if (rt_char8_search_char("abcde", 'd') != 3) goto error;
	if (rt_char8_search_char("abcde", 'e') != 4) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_count_occurrences()
{
	rt_s ret;

	if (rt_char8_count_occurrences("foo", "foo") != 1) goto error;
	if (rt_char8_count_occurrences("foofoo", "foo") != 2) goto error;
	if (rt_char8_count_occurrences("ofoofoo", "o") != 5) goto error;
	if (rt_char8_count_occurrences("foof", "o") != 2) goto error;
	if (rt_char8_count_occurrences("FoFoF", "FoF") != 1) goto error;
	if (rt_char8_count_occurrences("FoFFoF", "FoF") != 2) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_concat()
{
	rt_char8 buffer[RT_CHAR8_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	/* Buffer too small. */
	buffer_size = 0;
	if (rt_char8_concat(buffer, 6, &buffer_size, "foo", "bar", RT_NULL)) goto error;

	buffer_size = 0;
	if (!rt_char8_concat(buffer, 7, &buffer_size, "foo", "bar", RT_NULL)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, "foobar", 6)) goto error;

	if (!rt_char8_concat(buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size, "", "t", "e", "a", "m", RT_NULL)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, "foobarteam", 10)) goto error;

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
	if (!rt_char8_copy("XYZ", buffer_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE)) goto error;
	if (!rt_char8_replace(str, str_size, searched, searched_size, replacement, replacement_size, buffer, RT_CHAR8_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (rt_char8_get_size(buffer) != buffer_size) goto error;
	if (!rt_char8_equals(buffer, buffer_size, expected, rt_char8_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_char8_replace()
{
	rt_s ret;

	if (!zz_test_char8_replace_do("foo",       "foo", "bar",         "XYZbar"))                       goto error;
	if (!zz_test_char8_replace_do("foo",       "o",   "a",           "XYZfaa"))                       goto error;
	if (!zz_test_char8_replace_do("foobarfoo", "foo", "a",           "XYZabara"))                     goto error;
	if (!zz_test_char8_replace_do("foobarfoo", "foo", "long_string", "XYZlong_stringbarlong_string")) goto error;
	if (!zz_test_char8_replace_do("foobarfoo", "foo", "",            "XYZbar"))                       goto error;
	if (!zz_test_char8_replace_do("foofoo",    "foo", "",            "XYZ"))                          goto error;
	if (!zz_test_char8_replace_do("foobar",    "foo", "bar",         "XYZbarbar"))                    goto error;
	if (!zz_test_char8_replace_do("fobbar",    "foo", "bar",         "XYZfobbar"))                    goto error;

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
	if (!zz_test_char8_compare()) goto error;
	if (!zz_test_char8_append()) goto error;
	if (!zz_test_char8_append_char()) goto error;
	if (!zz_test_char8_copy()) goto error;
	if (!zz_test_char8_append_un()) goto error;
	if (!zz_test_char8_append_n()) goto error;
	if (!zz_test_char8_fast_lower_char()) goto error;
	if (!zz_test_char8_fast_upper_char()) goto error;
	if (!zz_test_char8_fast_lower_or_upper()) goto error;
	if (!zz_test_char8_convert_to_un()) goto error;
	if (!zz_test_char8_convert_to_n()) goto error;
	if (!zz_test_char8_trim()) goto error;
	if (!zz_test_char8_left_pad()) goto error;
	if (!zz_test_char8_right_pad()) goto error;
	if (!zz_test_char8_ends_with()) goto error;
	if (!zz_test_char8_search()) goto error;
	if (!zz_test_char8_search_char()) goto error;
	if (!zz_test_char8_count_occurrences()) goto error;
	if (!zz_test_char8_concat()) goto error;
	if (!zz_test_char8_replace()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
