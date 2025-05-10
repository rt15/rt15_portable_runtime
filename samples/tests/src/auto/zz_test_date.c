#include <rpr.h>

static rt_s zz_test_date_add_years_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n years, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret = RT_FAILED;

	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_years(&date, years))) goto end;

	if (RT_UNLIKELY(date.year != expected_year)) goto end;

	if (RT_UNLIKELY(date.month != expected_month)) goto end;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_add_years(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 0, 2024, 2, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 1, 2025, 2, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, -3, 2021, 2, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 3, 2027, 2, 15))) goto end;

	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, 1, 2025, 2, 28))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, -1, 2023, 2, 28))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, 4, 2028, 2, 29))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_add_months_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n months, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret = RT_FAILED;

	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_months(&date, months))) goto end;

	if (RT_UNLIKELY(date.year != expected_year)) goto end;

	if (RT_UNLIKELY(date.month != expected_month)) goto end;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_add_months(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, 0, 2024, 2, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, 1, 2024, 3, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, -3, 2023, 11, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2023, 11, 15, 3, 2024, 2, 15))) goto end;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 6, 30, 1, 2024, 7, 30))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 7, 31, 1, 2024, 8, 31))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 8, 31, 1, 2024, 9, 30))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 9, 30, 1, 2024, 10, 30))) goto end;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 6, 30, -1, 2024, 5, 30))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 7, 31, -1, 2024, 6, 30))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 8, 31, -1, 2024, 7, 31))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 9, 30, -1, 2024, 8, 30))) goto end;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 11, 15, 2123, 2201, 10, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 11, 15, -2123, 1847, 12, 15))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_add_days_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n days, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret = RT_FAILED;
	
	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_days(&date, days))) goto end;

	if (RT_UNLIKELY(date.year != expected_year)) goto end;

	if (RT_UNLIKELY(date.month != expected_month)) goto end;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_add_days(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 0, 2024, 2, 15))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 1, 2024, 2, 16))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -1, 2024, 2, 14))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 30, 2024, 3, 16))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -30, 2024, 1, 16))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 15123, 2065, 7, 12))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -13, 2024, 2, 2))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -14, 2024, 2, 1))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -15, 2024, 1, 31))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -16, 2024, 1, 30))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -60, 2023, 12, 17))) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -15123, 1982, 9, 20))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_get_month_name_do(rt_un month, const rt_char *expected)
{
	const rt_char *month_name;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_date_get_month_name(month, &month_name))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(month_name, rt_char_get_size(month_name), expected, rt_char_get_size(expected)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_get_month_name(void)
{
	const rt_char *month_name;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_get_month_name_do(1, _R("January")))) goto end;
	if (RT_UNLIKELY(!zz_test_date_get_month_name_do(12, _R("December")))) goto end;
	if (RT_UNLIKELY(rt_date_get_month_name(13, &month_name))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_get_day_of_week_name_do(rt_un day_of_week, const rt_char *expected)
{
	const rt_char *day_of_week_name;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_date_get_day_of_week_name(day_of_week, &day_of_week_name))) goto end;
	if (RT_UNLIKELY(!rt_char_equals(day_of_week_name, rt_char_get_size(day_of_week_name), expected, rt_char_get_size(expected)))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_date_get_day_of_week_name(void)
{
	const rt_char *day_of_week_name;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_get_day_of_week_name_do(1, _R("Sunday")))) goto end;
	if (RT_UNLIKELY(!zz_test_date_get_day_of_week_name_do(7, _R("Saturday")))) goto end;
	if (RT_UNLIKELY(rt_date_get_day_of_week_name(8, &day_of_week_name))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_date(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_date_add_years())) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_months())) goto end;
	if (RT_UNLIKELY(!zz_test_date_add_days())) goto end;
	if (RT_UNLIKELY(!zz_test_date_get_month_name())) goto end;
	if (RT_UNLIKELY(!zz_test_date_get_day_of_week_name())) goto end;

	ret = RT_OK;
end:
	return ret;
}
