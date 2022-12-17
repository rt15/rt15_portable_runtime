#include <rpr.h>

static rt_s zz_test_comparison_do(const rt_char8 *str1, const rt_char8 *str2, rt_n expected)
{
	rt_n comparison_result;
	rt_s ret;

	comparison_result = 99;
	if (!rt_comparison_default_callback(str1, str2, RT_NULL, &comparison_result)) goto error;
	if (comparison_result != expected) goto error;

	comparison_result = 99;
	if (!rt_comparison_default_with_size_callback(str1, rt_char8_get_size(str1), str2, rt_char8_get_size(str2), RT_NULL, &comparison_result)) goto error;
	if (comparison_result != expected) goto error;

	comparison_result = 99;
	if (!rt_comparison_default_callback(str2, str1, RT_NULL, &comparison_result)) goto error;
	if (comparison_result != -expected) goto error;

	comparison_result = 99;
	if (!rt_comparison_default_with_size_callback(str2, rt_char8_get_size(str2), str1, rt_char8_get_size(str1), RT_NULL, &comparison_result)) goto error;
	if (comparison_result != -expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_comparison()
{
	rt_s ret;
	if (!zz_test_comparison_do("Foo", "Foo", 0)) goto error;
	if (!zz_test_comparison_do("a", "b", -1)) goto error;
	if (!zz_test_comparison_do("A", "a", -0x20)) goto error;
	if (!zz_test_comparison_do("", "", 0)) goto error;
	if (!zz_test_comparison_do("a", "", 97)) goto error;
	if (!zz_test_comparison_do("ab", "a", 98)) goto error;
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
