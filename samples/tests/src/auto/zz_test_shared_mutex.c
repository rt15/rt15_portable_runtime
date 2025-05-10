#include <rpr.h>

#define ZZ_TEST_SHARED_MUTEX_NAME _R("/zz_test_shared_mutex")

rt_s zz_test_shared_mutex(void)
{
	struct rt_shared_mutex shared_mutex;
	rt_b shared_mutex_created = RT_FALSE;
	rt_b shared_memory_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_shared_mutex_create(&shared_mutex, ZZ_TEST_SHARED_MUTEX_NAME)))
		goto end;
	shared_mutex_created = RT_TRUE;
	shared_memory_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_shared_mutex_acquire(&shared_mutex)))
		goto end;

	if (RT_UNLIKELY(!rt_shared_mutex_release(&shared_mutex)))
		goto end;

	ret = RT_OK;
end:
	if (shared_mutex_created) {
		if (RT_UNLIKELY(!rt_shared_mutex_free(&shared_mutex)))
			ret = RT_FAILED;
	}
	if (shared_memory_created) {
		if (RT_UNLIKELY(!rt_shared_mutex_destroy(ZZ_TEST_SHARED_MUTEX_NAME)))
			ret = RT_FAILED;
	}

	return ret;
}
