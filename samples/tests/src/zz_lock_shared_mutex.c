#include <rpr.h>

rt_s zz_lock_shared_mutex(void)
{
	struct rt_shared_mutex shared_mutex;
	rt_b shared_mutex_created = RT_FALSE;
	rt_b shared_mutex_acquired = RT_FALSE;
	rt_char character;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Creating the mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_shared_mutex_create(&shared_mutex, _R("/zz_lock_shared_mutex"))))
		goto error;
	shared_mutex_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Acquiring the mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_shared_mutex_acquire(&shared_mutex)))
		goto error;
	shared_mutex_acquired = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex acquired. Press any key.\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto error;

	ret = RT_OK;
free:

	if (shared_mutex_acquired) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Releasing mutex...\n")) && ret))
			goto error;

		shared_mutex_acquired = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_mutex_release(&shared_mutex) && ret))
			goto error;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex released.\n"))&& ret ))
			goto error;
	}

	if (shared_mutex_created) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Freeing mutex...\n")) && ret))
			goto error;

		shared_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_shared_mutex_free(&shared_mutex) && ret))
			goto error;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex freed.\n")) && ret))
			goto error;
	}

	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
