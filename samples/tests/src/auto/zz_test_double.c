#include <rpr.h>

static rt_s zz_test_double_round(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-100000000000.5) != -100000000001)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-100000000000.4) != -100000000000)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-1.5) != -2)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-1.4) != -1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-0.9) != -1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-0.5) != -1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(-0.4) != 0)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(0.4) != 0)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(0.5) != 1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(0.9) != 1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(1.4) != 1)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(1.5) != 2)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(100000000000.4) != 100000000000)) goto end;
	if (RT_UNLIKELY(RT_DOUBLE_ROUND(100000000000.5) != 100000000001)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_double_equals_do(rt_f64 x, rt_f64 y, rt_b expected)
{
	rt_b value;
	rt_s ret = RT_FAILED;

	value = RT_DOUBLE_EQUALS(x, y);

	if (RT_UNLIKELY(!RT_MEMORY_XNOR(value, expected))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_double_equals(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_double_equals_do(0.0, 0.0, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(100000000000.0, 100000000000.0, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(1.0000000001 + 2.0000000002, 3.0000000003, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(-1.0000000001 - 2.0000000002, -3.0000000003, RT_TRUE))) goto end;

	if (RT_UNLIKELY(!zz_test_double_equals_do(1.0000000001 + 2.0000000003, 3.0000000003, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(1.0000000001 + 2.0000000001, 3.0000000003, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(-1.0000000001 - 2.0000000003, -3.0000000003, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals_do(-1.0000000001 - 2.0000000001, -3.0000000003, RT_FALSE))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_double(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_double_round())) goto end;
	if (RT_UNLIKELY(!zz_test_double_equals())) goto end;

	ret = RT_OK;
end:
	return ret;
}
