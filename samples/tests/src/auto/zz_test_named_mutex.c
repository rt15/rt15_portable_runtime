#include <rpr.h>

rt_s zz_test_named_mutex(void)
{
	struct rt_named_mutex named_mutex;
	rt_b named_mutex_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_named_mutex_create(&named_mutex, _R("/zz_test_named_mutex"))))
		goto error;
	named_mutex_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_named_mutex_acquire(&named_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_named_mutex_release(&named_mutex)))
		goto error;

	ret = RT_OK;
free:
	if (named_mutex_created) {
		named_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_named_mutex_free(&named_mutex) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}