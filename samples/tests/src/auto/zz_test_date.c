#include <rpr.h>

static rt_s zz_test_date_add_years_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n years, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret;

	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_years(&date, years))) goto error;

	if (RT_UNLIKELY(date.year != expected_year)) goto error;

	if (RT_UNLIKELY(date.month != expected_month)) goto error;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_date_add_years()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 0, 2024, 2, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 1, 2025, 2, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, -3, 2021, 2, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 15, 3, 2027, 2, 15))) goto error;

	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, 1, 2025, 2, 28))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, -1, 2023, 2, 28))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_years_do(2024, 2, 29, 4, 2028, 2, 29))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_date_add_months_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n months, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret;

	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_months(&date, months))) goto error;

	if (RT_UNLIKELY(date.year != expected_year)) goto error;

	if (RT_UNLIKELY(date.month != expected_month)) goto error;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_date_add_months()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, 0, 2024, 2, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, 1, 2024, 3, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 2, 15, -3, 2023, 11, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2023, 11, 15, 3, 2024, 2, 15))) goto error;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 6, 30, 1, 2024, 7, 30))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 7, 31, 1, 2024, 8, 31))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 8, 31, 1, 2024, 9, 30))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 9, 30, 1, 2024, 10, 30))) goto error;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 6, 30, -1, 2024, 5, 30))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 7, 31, -1, 2024, 6, 30))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 8, 31, -1, 2024, 7, 31))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 9, 30, -1, 2024, 8, 30))) goto error;

	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 11, 15, 2123, 2201, 10, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months_do(2024, 11, 15, -2123, 1847, 12, 15))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_date_add_days_do(rt_un16 year, rt_uchar8 month, rt_uchar8 day, rt_n days, rt_un16 expected_year, rt_uchar8 expected_month, rt_uchar8 expected_day)
{
	struct rt_date date;
	rt_s ret;
	
	date.year = year;
	date.month = month;
	date.day = day;

	if (RT_UNLIKELY(!rt_date_add_days(&date, days))) goto error;

	if (RT_UNLIKELY(date.year != expected_year)) goto error;

	if (RT_UNLIKELY(date.month != expected_month)) goto error;
	
	if (RT_UNLIKELY(date.day != expected_day)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_date_add_days()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 0, 2024, 2, 15))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 1, 2024, 2, 16))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -1, 2024, 2, 14))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 30, 2024, 3, 16))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -30, 2024, 1, 16))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, 15123, 2065, 7, 12))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -13, 2024, 2, 2))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -14, 2024, 2, 1))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -15, 2024, 1, 31))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -16, 2024, 1, 30))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -60, 2023, 12, 17))) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days_do(2024, 2, 15, -15123, 1982, 9, 20))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_date()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_date_add_years())) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_months())) goto error;
	if (RT_UNLIKELY(!zz_test_date_add_days())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
