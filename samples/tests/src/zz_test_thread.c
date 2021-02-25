#include <rpr.h>

static rt_un32 zz_test_thread_callback(void *parameter)
{
	rt_un32 exit_code;

	rt_sleep_sleep(500);
	if (*((rt_un*)parameter) != 12)
		exit_code = 1;
	else
		exit_code = 42;
	return exit_code;
}

rt_s zz_test_thread()
{
	struct rt_chrono chrono;
	rt_un duration;
	rt_un parameter = 12;
	rt_b thread_created = RT_FALSE;
	struct rt_thread thread;
	rt_un32 exit_code;
	rt_s ret;

	if (!rt_chrono_create(&chrono))
		goto error;

	if (!rt_thread_create(&thread, &zz_test_thread_callback, &parameter))
		goto error;
	thread_created = RT_TRUE;

	if (!rt_thread_join(&thread))
		goto error;

	if (!rt_chrono_get_duration(&chrono, &duration))
		goto error;

	if (duration < 250000 || duration > 1000000)
		goto error;

	if (!rt_thread_get_exit_code(&thread, &exit_code))
		goto error;

	if (exit_code != 42)
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if ((!rt_thread_free(&thread)) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
