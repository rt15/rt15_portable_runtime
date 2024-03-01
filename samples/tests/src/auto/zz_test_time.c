#include <rpr.h>

rt_s zz_test_time(void)
{
	rt_n unix_time;
	struct rt_time_info time_info;
	rt_n unix_time2;
	struct rt_time_info accurate_time_info;
	rt_un milliseconds;
	rt_s ret;

	if (RT_UNLIKELY(!rt_time_get_unix_time(&unix_time))) goto error;
	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, unix_time))) goto error;
	if (RT_UNLIKELY(!rt_time_get_unix_time_from_time_info(&time_info, &unix_time2))) goto error;
	if (RT_UNLIKELY(unix_time != unix_time2)) goto error;

	if (RT_UNLIKELY(!rt_time_info_create_accurate(&accurate_time_info, &milliseconds))) goto error;
	if (RT_UNLIKELY(accurate_time_info.minutes != time_info.minutes)) goto error;
	if (RT_UNLIKELY(accurate_time_info.hours != time_info.hours)) goto error;
	if (RT_UNLIKELY(accurate_time_info.day != time_info.day)) goto error;
	if (RT_UNLIKELY(accurate_time_info.month != time_info.month)) goto error;
	if (RT_UNLIKELY(accurate_time_info.year != time_info.year)) goto error;

	if (RT_UNLIKELY(!rt_time_get_unix_time_from_time_info(&accurate_time_info, RT_NULL))) goto error;
	if (RT_UNLIKELY(accurate_time_info.minutes != time_info.minutes)) goto error;
	if (RT_UNLIKELY(accurate_time_info.hours != time_info.hours)) goto error;
	if (RT_UNLIKELY(accurate_time_info.day != time_info.day)) goto error;
	if (RT_UNLIKELY(accurate_time_info.month != time_info.month)) goto error;
	if (RT_UNLIKELY(accurate_time_info.year != time_info.year)) goto error;
	if (RT_UNLIKELY(accurate_time_info.day_of_week != time_info.day_of_week)) goto error;
	if (RT_UNLIKELY(accurate_time_info.day_of_year != time_info.day_of_year)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
