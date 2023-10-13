#include <rpr.h>

static rt_s zz_test_is_greater_or_equal_to_do(rt_un major, rt_un minor, rt_un patch, rt_b expected)
{
	rt_b result;
	rt_s ret;

	if (RT_UNLIKELY(!rt_os_version_is_greater_or_equal_to(major, minor, patch, &result))) goto error;
	if (RT_UNLIKELY(!RT_MEMORY_XNOR(result, expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_is_greater_or_equal_to(rt_un major, rt_un minor, rt_un patch)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major + 1, 0, 0, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor + 1, 0, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch + 1, RT_FALSE))) goto error;

	if (major > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major - 1, minor + 1, patch + 1, RT_TRUE))) goto error;
	}
	if (minor > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor - 1, patch + 1, RT_TRUE))) goto error;
	}
	if (patch > 0) {
		if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to_do(major, minor, patch - 1, RT_TRUE))) goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_os_version()
{
	rt_un major;
	rt_un minor;
	rt_un patch;
	rt_un major_only;
	rt_s ret;

	if (RT_UNLIKELY(!rt_os_version_get(&major, &minor, &patch))) goto error;
	if (RT_UNLIKELY(!rt_os_version_get(&major_only, RT_NULL, RT_NULL))) goto error;
	if (RT_UNLIKELY(major != major_only)) goto error;

	if (RT_UNLIKELY(!zz_test_is_greater_or_equal_to(major, minor, patch))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
