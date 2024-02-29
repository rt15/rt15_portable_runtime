#include <rpr.h>

rt_s zz_test_time(void)
{
	rt_n unix_time;
	struct rt_time_info time_info;
	rt_n unix_time2;
	rt_s ret;

	if (RT_UNLIKELY(!rt_time_get_unix_time(&unix_time)))
		goto error;

	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, unix_time)))
		goto error;

	if (RT_UNLIKELY(!rt_time_get_unix_time_from_time_info(&time_info, &unix_time2)))
		goto error;

	if (RT_UNLIKELY(unix_time != unix_time2))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
