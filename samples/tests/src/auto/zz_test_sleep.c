#include <rpr.h>

rt_s zz_test_sleep(void)
{
	struct rt_chrono chrono;
	rt_un duration;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_chrono_create(&chrono)))
		goto end;

	rt_sleep_sleep(500);

	if (RT_UNLIKELY(!rt_chrono_get_duration(&chrono, &duration)))
		goto end;

	if (RT_UNLIKELY(duration < 400000 || duration > 600000))
		goto end;

	ret = RT_OK;
end:
	return ret;
}
