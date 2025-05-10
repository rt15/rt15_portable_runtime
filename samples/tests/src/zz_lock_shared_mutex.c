#include <rpr.h>

rt_s zz_lock_shared_mutex(void)
{
	struct rt_shared_mutex shared_mutex;
	rt_b shared_mutex_created = RT_FALSE;
	rt_b shared_mutex_acquired = RT_FALSE;
	rt_char character;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Creating the mutex...\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_shared_mutex_create(&shared_mutex, _R("/zz_lock_shared_mutex"))))
		goto end;
	shared_mutex_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Acquiring the mutex...\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_shared_mutex_acquire(&shared_mutex)))
		goto end;
	shared_mutex_acquired = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex acquired. Press any key.\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto end;

	ret = RT_OK;
end:

	if (shared_mutex_acquired) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Releasing mutex...\n"))))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_shared_mutex_release(&shared_mutex)))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex released.\n"))))
			ret = RT_FAILED;
	}

	if (shared_mutex_created) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Freeing mutex...\n"))))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_shared_mutex_free(&shared_mutex)))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex freed.\n"))))
			ret = RT_FAILED;
	}

	return ret;
}
