#include <rpr.h>

#define ZZ_TEST_SHARED_MEMORY_SIZE 6144
#define ZZ_TEST_SHARED_MEMORY_EXPECTED_SUM 18871296u

#define ZZ_TEST_SHARED_MEMORY_NAME _R("/zz_test_shared_memory")

static rt_s zz_test_shared_memory_do(enum rt_shared_memory_right shared_memory_right)
{
	struct rt_shared_memory shared_memory;
	rt_b shared_memory_created = RT_FALSE;
	rt_b shared_memory_initialized = RT_FALSE;
	rt_uchar8 *area;
	rt_un32 sum;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_shared_memory_create(&shared_memory, ZZ_TEST_SHARED_MEMORY_NAME, ZZ_TEST_SHARED_MEMORY_SIZE, shared_memory_right)))
		goto error;
	shared_memory_created = RT_TRUE;
	shared_memory_initialized = RT_TRUE;

	area = shared_memory.area;

	if (shared_memory_right == RT_SHARED_MEMORY_RIGHT_READ_WRITE || shared_memory_right == RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE) {
		for (i = 0; i < ZZ_TEST_SHARED_MEMORY_SIZE; i++)
			area[i] = (rt_uchar8)i;
	}

	sum = 0;
	for (i = 0; i < ZZ_TEST_SHARED_MEMORY_SIZE; i++)
		sum += i;

	if (shared_memory_right == RT_SHARED_MEMORY_RIGHT_READ_WRITE || shared_memory_right == RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE) {
		if (sum != ZZ_TEST_SHARED_MEMORY_EXPECTED_SUM)
			goto error;
	}

	ret = RT_OK;
free:
	if (shared_memory_created) {
		shared_memory_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_memory_free(&shared_memory) && ret))
			goto error;
	}
	if (shared_memory_initialized) {
		shared_memory_initialized = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_memory_destroy(ZZ_TEST_SHARED_MEMORY_NAME) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_shared_memory(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_READ))) goto error;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_READ_WRITE))) goto error;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_EXECUTE_READ))) goto error;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
