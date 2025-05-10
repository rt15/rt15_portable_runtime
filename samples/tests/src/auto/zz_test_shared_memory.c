#include <rpr.h>

#define ZZ_TEST_SHARED_MEMORY_SIZE 6144
#define ZZ_TEST_SHARED_MEMORY_EXPECTED_SUM 783360u

#define ZZ_TEST_SHARED_MEMORY_NAME _R("/zz_test_shared_memory")

static rt_s zz_test_shared_memory_do(enum rt_shared_memory_right shared_memory_right)
{
	struct rt_shared_memory shared_memory;
	rt_b shared_memory_created = RT_FALSE;
	rt_b shared_memory_initialized = RT_FALSE;
	rt_uchar8 *area;
	rt_un32 sum;
	rt_un32 i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_shared_memory_create(&shared_memory, ZZ_TEST_SHARED_MEMORY_NAME, ZZ_TEST_SHARED_MEMORY_SIZE, shared_memory_right)))
		goto end;
	shared_memory_created = RT_TRUE;
	shared_memory_initialized = RT_TRUE;

	area = shared_memory.area;

	/* Check that the memory has been initialized to zero. */
	for (i = 0; i < ZZ_TEST_SHARED_MEMORY_SIZE; i++)
		if (area[i])
			goto end;

	/* Write in the memory, if possible. */
	if (shared_memory_right == RT_SHARED_MEMORY_RIGHT_READ_WRITE || shared_memory_right == RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE) {
		for (i = 0; i < ZZ_TEST_SHARED_MEMORY_SIZE; i++)
			area[i] = (rt_uchar8)i;
	}

	/* Perform a checksum on the written bytes. */
	sum = 0;
	for (i = 0; i < ZZ_TEST_SHARED_MEMORY_SIZE; i++)
		sum += area[i];

	/* Check the checksum. */
	if (shared_memory_right == RT_SHARED_MEMORY_RIGHT_READ_WRITE || shared_memory_right == RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE) {
		if (sum != ZZ_TEST_SHARED_MEMORY_EXPECTED_SUM)
			goto end;
	}

	ret = RT_OK;
end:
	if (shared_memory_created) {
		if (RT_UNLIKELY(!rt_shared_memory_free(&shared_memory)))
			ret = RT_FAILED;
	}
	if (shared_memory_initialized) {
		if (RT_UNLIKELY(!rt_shared_memory_destroy(ZZ_TEST_SHARED_MEMORY_NAME)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_test_shared_memory(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_READ))) goto end;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_READ_WRITE))) goto end;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_EXECUTE_READ))) goto end;
	if (RT_UNLIKELY(!zz_test_shared_memory_do(RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE))) goto end;

	ret = RT_OK;
end:
	return ret;
}
