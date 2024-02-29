#include <rpr.h>

static rt_s zz_manual_test_time_append_two_digits(rt_un value, rt_un base, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret;

	if (value < 10) {
		if (RT_UNLIKELY(!rt_char_append_char(_R('0'), buffer, buffer_capacity, buffer_size)))
			goto error;
	}

	if (RT_UNLIKELY(!rt_char_append_un(value, base, buffer, buffer_capacity, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * yyyy-MM-dd HH:mm:ss
 */
static rt_s zz_manual_test_time_display_time_info(struct rt_time_info *time_info)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = 13;
	if (RT_UNLIKELY(!rt_char_copy(_R("Local time = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_un(time_info->year, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('-'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_append_two_digits(time_info->month, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('-'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_append_two_digits(time_info->day, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R(' '), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_append_two_digits(time_info->hours, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R(':'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_append_two_digits(time_info->minutes, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R(':'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_append_two_digits(time_info->seconds, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_time_display_more_time_info(struct rt_time_info *time_info)
{
	const rt_char *month_name;
	const rt_char *day_of_week_name;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	const rt_char *daylight_saving_time;
	rt_s ret;

	buffer_size = 8;
	if (RT_UNLIKELY(!rt_char_copy(_R("Month = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;

	if (RT_UNLIKELY(!rt_date_get_month_name(time_info->month, &month_name)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append(month_name, rt_char_get_size(month_name), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size)))
		goto error;

	buffer_size = 14;
	if (RT_UNLIKELY(!rt_char_copy(_R("Day of week = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;

	if (RT_UNLIKELY(!rt_date_get_day_of_week_name(time_info->day_of_week, &day_of_week_name)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append(day_of_week_name, rt_char_get_size(day_of_week_name), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size)))
		goto error;

	buffer_size = 14;
	if (RT_UNLIKELY(!rt_char_copy(_R("Day of year = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_un(time_info->day_of_year, 10, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size)))
		goto error;

	buffer_size = 23;
	if (RT_UNLIKELY(!rt_char_copy(_R("Daylight saving time = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE)))
		goto error;

	if (time_info->daylight_saving_time > 0)
		daylight_saving_time = _R("in effect");
	else if (time_info->daylight_saving_time == 0)
		daylight_saving_time = _R("not in effect");
	else
		daylight_saving_time = _R("unknown");

	if (RT_UNLIKELY(!rt_char_append(daylight_saving_time, rt_char_get_size(daylight_saving_time), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)))
		goto error;

	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_time(void)
{
	rt_n unix_time;
	struct rt_time_info time_info;
	rt_s ret;

	if (RT_UNLIKELY(!rt_time_get_unix_time(&unix_time)))
		goto error;

	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, unix_time)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_display_time_info(&time_info)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_time_display_more_time_info(&time_info)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
