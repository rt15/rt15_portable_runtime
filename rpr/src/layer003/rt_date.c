#include "layer003/rt_date.h"

#include "layer002/rt_error.h"

static const rt_char *const rt_date_month_names[12] = {
	_R("January"),
	_R("February"),
	_R("March"),
	_R("April"),
	_R("May"),
	_R("June"),
	_R("July"),
	_R("August"),
	_R("September"),
	_R("October"),
	_R("November"),
	_R("December")
};

static const rt_char *const rt_date_day_of_week_names[7] = {
	_R("Sunday"),
	_R("Monday"),
	_R("Tuesday"),
	_R("Wednesday"),
	_R("Thursday"),
	_R("Friday"),
	_R("Saturday")
};

rt_s rt_date_add_years(struct rt_date *date, rt_n years)
{
	rt_n year = date->year;
	rt_un days_in_month;
	rt_s ret = RT_FAILED;

	year += years;

	if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, date->month, &days_in_month)))
		goto end;

	if (date->day > (rt_n)days_in_month) {
		date->day = (rt_uchar8)days_in_month;
	}

	date->year = (rt_un16)year;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_date_add_months(struct rt_date *date, rt_n months)
{
	rt_un year = date->year;
	rt_n month = date->month;
	rt_un days_in_month;
	rt_s ret = RT_FAILED;

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
		goto end;
	
	if (date->day > (rt_n)days_in_month) {
		date->day = (rt_uchar8)days_in_month;
	}

	date->year = (rt_un16)year;
	date->month = (rt_uchar8)month;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_date_add_days(struct rt_date *date, rt_n days)
{
	rt_n local_days = days;
	rt_un year = date->year;
	rt_n month = date->month;
	rt_n day = date->day;
	rt_un days_in_month;
	rt_s ret = RT_FAILED;

	if (local_days > 0) {
		while (local_days > 0) {
			if (RT_UNLIKELY(!rt_date_get_days_in_month((rt_un16)year, (rt_uchar8)month, &days_in_month)))
				goto end;
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
					goto end;
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
						goto end;
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
end:
	return ret;
}

rt_s rt_date_get_days_in_month(rt_un16 year, rt_uchar8 month, rt_un *days)
{
	rt_s ret = RT_FAILED;

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
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_date_get_month_name(rt_un month, const rt_char **name)
{
	rt_s ret = RT_FAILED;

	if (month > 12) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	*name = rt_date_month_names[month - 1];

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_date_get_day_of_week_name(rt_un day_of_week, const rt_char **name)
{
	rt_s ret = RT_FAILED;

	if (day_of_week > 7) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

	*name = rt_date_day_of_week_names[day_of_week - 1];

	ret = RT_OK;
end:
	return ret;
}
