#include <rpr.h>

static rt_s zz_test_random_get_unsigned_integer_with_boundaries_do(rt_un lower_bound, rt_un upper_bound, rt_b check_boundaries)
{
	rt_un unsigned_integer;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (check_boundaries) {
		/* Check that lower bound is hit. */
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_random_get_unsigned_integer_with_boundaries(lower_bound, upper_bound, &unsigned_integer)))
				goto end;
			if (unsigned_integer == lower_bound)
				break;
		}
		/* Check that upper bound is hit. */
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_random_get_unsigned_integer_with_boundaries(lower_bound, upper_bound, &unsigned_integer)))
				goto end;
			if (unsigned_integer == upper_bound)
				break;
		}
	}

	for (i = 0; i < 2000; i++) {
		if (RT_UNLIKELY(!rt_random_get_unsigned_integer_with_boundaries(lower_bound, upper_bound, &unsigned_integer)))
			goto end;
		if (RT_UNLIKELY(unsigned_integer < lower_bound || unsigned_integer > upper_bound))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_random_get_unsigned_integer_with_boundaries(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(0, 1, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(0, 2, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(0, 3, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(1, 2, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(1, 3, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(12, 15, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(0, 1000000000, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries_do(0, 2000000000, RT_FALSE))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_random_get_integer_with_boundaries_do(rt_n lower_bound, rt_n upper_bound, rt_b check_boundaries)
{
	rt_n integer;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (check_boundaries) {
		/* Check that lower bound is hit. */
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_random_get_integer_with_boundaries(lower_bound, upper_bound, &integer)))
				goto end;
			if (integer == lower_bound)
				break;
		}
		/* Check that upper bound is hit. */
		while (RT_TRUE) {
			if (RT_UNLIKELY(!rt_random_get_integer_with_boundaries(lower_bound, upper_bound, &integer)))
				goto end;
			if (integer == upper_bound)
				break;
		}
	}

	for (i = 0; i < 2000; i++) {
		if (RT_UNLIKELY(!rt_random_get_integer_with_boundaries(lower_bound, upper_bound, &integer)))
			goto end;
		if (RT_UNLIKELY(integer < lower_bound || integer > upper_bound))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_random_get_integer_with_boundaries(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-1, 0, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-2, 0, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(0, 1, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(0, 2, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-1, -1, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-2, -2, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-3, 3, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(12, 15, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-1000000000, 1000000000, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries_do(-2000000000, 2000000000, RT_FALSE))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_random(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_random_get_unsigned_integer_with_boundaries())) goto end;
	if (RT_UNLIKELY(!zz_test_random_get_integer_with_boundaries())) goto end;

	ret = RT_OK;
end:
	return ret;
}
