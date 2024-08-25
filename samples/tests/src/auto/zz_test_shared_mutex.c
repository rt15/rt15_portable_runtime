#include <rpr.h>

#define ZZ_TEST_SHARED_MUTEX_NAME _R("/zz_test_shared_mutex")

rt_s zz_test_shared_mutex(void)
{
	struct rt_shared_mutex shared_mutex;
	rt_b shared_mutex_created = RT_FALSE;
	rt_b shared_memory_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_shared_mutex_create(&shared_mutex, ZZ_TEST_SHARED_MUTEX_NAME)))
		goto error;
	shared_mutex_created = RT_TRUE;
	shared_memory_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_shared_mutex_acquire(&shared_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_shared_mutex_release(&shared_mutex)))
		goto error;

	ret = RT_OK;
free:
	if (shared_mutex_created) {
		shared_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_mutex_free(&shared_mutex) && ret))
			goto error;
	}
	if (shared_memory_created) {
		shared_memory_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_mutex_destroy(ZZ_TEST_SHARED_MUTEX_NAME) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
