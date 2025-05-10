#include <rpr.h>

rt_s zz_test_time(void)
{
	rt_n unix_time;
	struct rt_time_info time_info;
	rt_n unix_time2;
	struct rt_time_info accurate_time_info;
	rt_un milliseconds;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_time_get_unix_time(&unix_time))) goto end;
	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, unix_time))) goto end;
	if (RT_UNLIKELY(!rt_time_get_unix_time_from_time_info(&time_info, &unix_time2))) goto end;
	if (RT_UNLIKELY(unix_time != unix_time2)) goto end;

	if (RT_UNLIKELY(!rt_time_info_create_accurate(&accurate_time_info, &milliseconds))) goto end;
	if (RT_UNLIKELY(accurate_time_info.minutes != time_info.minutes)) goto end;
	if (RT_UNLIKELY(accurate_time_info.hours != time_info.hours)) goto end;
	if (RT_UNLIKELY(accurate_time_info.day != time_info.day)) goto end;
	if (RT_UNLIKELY(accurate_time_info.month != time_info.month)) goto end;
	if (RT_UNLIKELY(accurate_time_info.year != time_info.year)) goto end;
	if (RT_UNLIKELY(accurate_time_info.daylight_saving_time != time_info.daylight_saving_time)) goto end;

	if (RT_UNLIKELY(!rt_time_get_unix_time_from_time_info(&accurate_time_info, RT_NULL))) goto end;
	if (RT_UNLIKELY(accurate_time_info.minutes != time_info.minutes)) goto end;
	if (RT_UNLIKELY(accurate_time_info.hours != time_info.hours)) goto end;
	if (RT_UNLIKELY(accurate_time_info.day != time_info.day)) goto end;
	if (RT_UNLIKELY(accurate_time_info.month != time_info.month)) goto end;
	if (RT_UNLIKELY(accurate_time_info.year != time_info.year)) goto end;
	if (RT_UNLIKELY(accurate_time_info.daylight_saving_time != time_info.daylight_saving_time)) goto end;
	if (RT_UNLIKELY(accurate_time_info.day_of_week != time_info.day_of_week)) goto end;
	if (RT_UNLIKELY(accurate_time_info.day_of_year != time_info.day_of_year)) goto end;

	ret = RT_OK;
end:
	return ret;
}
