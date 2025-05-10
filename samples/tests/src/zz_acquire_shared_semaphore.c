#include <rpr.h>

rt_s zz_acquire_shared_semaphore(void)
{
	struct rt_semaphore semaphore;
	rt_b semaphore_created = RT_FALSE;
	rt_b semaphore_acquired = RT_FALSE;
	rt_char character;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Creating the semaphore...\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_semaphore_create(&semaphore, _R("/zz_acquire_shared_semaphore"), 2)))
		goto end;
	semaphore_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Acquiring the semaphore...\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_semaphore_acquire(&semaphore)))
		goto end;
	semaphore_acquired = RT_TRUE;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Semaphore acquired. Press any key.\n"))))
		goto end;

	if (RT_UNLIKELY(!rt_console_read_char(&character)))
		goto end;

	ret = RT_OK;
end:

	if (semaphore_acquired) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Releasing semaphore...\n"))))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_semaphore_release(&semaphore)))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Semaphore released.\n"))))
			ret = RT_FAILED;
	}

	if (semaphore_created) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Freeing semaphore...\n"))))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_semaphore_free(&semaphore)))
			ret = RT_FAILED;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Semaphore freed.\n"))))
			ret = RT_FAILED;
	}

	return ret;
}
