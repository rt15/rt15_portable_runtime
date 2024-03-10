#include <rpr.h>

static rt_s zz_test_float_round(void)
{
	rt_s ret;

	if (RT_UNLIKELY(RT_FLOAT_ROUND(-1000000.5f) != -1000001)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-1000000.4f) != -1000000)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-1.5f) != -2)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-1.4f) != -1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-0.9f) != -1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-0.5f) != -1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(-0.4f) != 0)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(0.4f) != 0)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(0.5f) != 1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(0.9f) != 1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(1.4f) != 1)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(1.5f) != 2)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(1000000.4f) != 1000000)) goto error;
	if (RT_UNLIKELY(RT_FLOAT_ROUND(1000000.5f) != 1000001)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_float_equals_do(rt_f32 x, rt_f32 y, rt_b expected)
{
	rt_b value;
	rt_s ret;

	value = RT_FLOAT_EQUALS(x, y);

	if (RT_UNLIKELY(!RT_MEMORY_XNOR(value, expected))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_float_equals(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_float_equals_do(0.0f, 0.0f, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(1000000.0f, 1000000.0f, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(1.00001f + 2.00002f, 3.00003f, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(-1.00001f - 2.00002f, -3.00003f, RT_TRUE))) goto error;

	if (RT_UNLIKELY(!zz_test_float_equals_do(1.00001f + 2.00003f, 3.00003f, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(1.00001f + 2.00001f, 3.00003f, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(-1.00001f - 2.00003f, -3.00003f, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals_do(-1.00001f - 2.00001f, -3.00003f, RT_FALSE))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_float(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_float_round())) goto error;
	if (RT_UNLIKELY(!zz_test_float_equals())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
