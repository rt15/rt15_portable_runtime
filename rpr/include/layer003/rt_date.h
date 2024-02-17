#ifndef RT_DATE_H
#define RT_DATE_H

#include "layer000/rt_types.h"

struct rt_date {
	rt_un16 year;    /* 2024 is 2024. */
	rt_uchar8 month; /* 1 is January. */
	rt_uchar8 day;   /* 1 is the first day of the month. */
};

RT_API rt_s rt_date_add_months(struct rt_date *date, rt_n months);

RT_API rt_s rt_date_add_days(struct rt_date *date, rt_n days);

/**
 * Return the number of days in the month corresponding to the given <tt>year</tt> and </tt>month</tt>.
 * 
 * <p>
 * This function takes into account leap years.
 * </p>
 */
RT_API rt_s rt_date_get_days_in_month(rt_un16 year, rt_uchar8 month, rt_un *days);


#endif /* RT_DATE_H */
