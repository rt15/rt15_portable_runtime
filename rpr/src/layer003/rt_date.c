#include "layer003/rt_date.h"

#include "layer002/rt_error.h"

rt_s rt_date_add_years(struct rt_date *date, rt_n years)
{
	rt_n year = date->year;
	rt_un days_in_month;
	rt_s ret;

	year += years;

	if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, date->month, &days_in_month)))
		goto error;

	if (date->day > (rt_n)days_in_month) {
		date->day = (rt_uchar8)days_in_month;
	}

	date->year = (rt_un16)year;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_date_add_months(struct rt_date *date, rt_n months)
{
	rt_un year = date->year;
	rt_n month = date->month;
	rt_un days_in_month;
	rt_s ret;

	if (months < 0) {
		month += months;
		if (month < 1) {
			year += month / 12 - 1;
			month = 12 + month % 12;
		}
	} else {
		month += months;
		if (month > 12) {
			year += month / 12;
			month = month % 12;
		}
	}

	if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, (rt_uchar8)month, &days_in_month)))
		goto error;
	
	if (date->day > (rt_n)days_in_month) {
		date->day = (rt_uchar8)days_in_month;
	}

	date->year = (rt_un16)year;
	date->month = (rt_uchar8)month;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_date_add_days(struct rt_date *date, rt_n days)
{
	rt_n local_days = days;
	rt_un year = date->year;
	rt_n month = date->month;
	rt_n day = date->day;
	rt_un days_in_month;
	rt_s ret;

	if (local_days > 0) {
		while (local_days > 0) {
			if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, (rt_uchar8)month, &days_in_month)))
				goto error;
			if (day + local_days > (rt_n)days_in_month) {
				/* Substract the days of the month minus the initial number of days. */
				local_days -= days_in_month - day + 1;
				day = 1;
				month++;
				if (month > 12) {
					month = 1;
					year++;
				}
			} else {
				day += local_days;
				local_days = 0;
			}
		}
	} else {
		while (local_days < 0) {
			if (day + local_days < 1) {
				month--;
				if (month < 1) {
					month = 12;
					year--;
				}
				if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, (rt_uchar8)month, &days_in_month)))
					goto error;
				local_days = local_days + day;
				day = days_in_month;
				while (local_days < -(rt_n)days_in_month) {
					local_days += days_in_month;
					month--;
					if (month < 1) {
						month = 12;
						year--;
					}
					if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, (rt_uchar8)month, &days_in_month)))
						goto error;
					day = days_in_month;
				}
			} else {
				day += local_days;
				local_days = 0;
			}
		}
	}
	date->year = (rt_un16)year;
	date->month = (rt_uchar8)month;
	date->day = (rt_uchar8)day;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_date_get_days_in_month(rt_un16 year, rt_uchar8 month, rt_un *days)
{
	rt_s ret;

	switch (month) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		*days = 31;
		break;
	case 2:
		if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
			*days = 29;
		else
			*days = 28;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		*days = 30;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
