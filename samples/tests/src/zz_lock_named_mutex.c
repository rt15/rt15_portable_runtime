#include <rpr.h>

rt_s zz_lock_named_mutex(void)
{
	struct rt_named_mutex named_mutex;
	rt_char character;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Creating the mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_named_mutex_create(&named_mutex, _R("/zz_lock_named_mutex"))))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Acquiring the mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_named_mutex_acquire(&named_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex acquired. Press any key.\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Releasing mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_named_mutex_release(&named_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex released.\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Freeing mutex...\n"))))
		goto error;

	if (RT_UNLIKELY(!rt_named_mutex_free(&named_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Mutex freed.\n"))))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}