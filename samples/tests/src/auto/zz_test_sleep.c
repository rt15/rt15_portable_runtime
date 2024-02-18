#include <rpr.h>

rt_s zz_test_sleep(void)
{
	struct rt_chrono chrono;
	rt_un duration;
	rt_s ret;

	if (RT_UNLIKELY(!rt_chrono_create(&chrono)))
		goto error;

	rt_sleep_sleep(500);

	if (RT_UNLIKELY(!rt_chrono_get_duration(&chrono, &duration)))
		goto error;

	if (RT_UNLIKELY(duration < 400000 || duration > 600000))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
