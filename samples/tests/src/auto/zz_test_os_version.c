#include <rpr.h>

static rt_s zz_test_is_greater_or_equal_to_do(rt_un major, rt_un minor, rt_un patch, rt_b expected)
{
	rt_b result;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_os_version_is_greater_or_equal_to(major, minor, patch, &result))) goto end;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, expected))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_is_greater_or_equal_to(rt_un major, rt_un minor, rt_un patch)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major + 1, 0, 0, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor + 1, 0, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch + 1, RT_FALSE))) goto end;

	if (major > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major - 1, minor + 1, patch + 1, RT_TRUE))) goto end;
	}
	if (minor > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor - 1, patch + 1, RT_TRUE))) goto end;
	}
	if (patch > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch - 1, RT_TRUE))) goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_os_version(void)
{
	rt_un major;
	rt_un minor;
	rt_un patch;
	rt_un major_only;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_os_version_get(&major, &minor, &patch))) goto end;
	if (RT_UNLIKELY(!rt_os_version_get(&major_only, RT_NULL, RT_NULL))) goto end;
	if (RT_UNLIKELY(major != major_only)) goto end;

	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to(major, minor, patch))) goto end;

	ret = RT_OK;
end:
	return ret;
}
