#ifndef RT_TIME_H
#define RT_TIME_H

#include "layer000/rt_types.h"

struct rt_time_info {
	rt_n32 seconds;              /* Ranges from 0 to 59, but can be up to 61 to allow for leap seconds. */
	rt_n32 minutes;              /* Ranges from 0 to 59. */
	rt_n32 hours;                /* Ranges from 0 to 23. */
	rt_n32 day;                  /* Ranges from 1 to 31. */
	rt_n32 month;                /* Ranges from 1 to 12. */
	rt_n32 year;                 /* 2024 is 2024. */
	rt_n32 day_of_week;          /* Ranges from 1 to 7, 1 is Sunday. */
	rt_n32 day_of_year;          /* Ranges from 1 to 366. */
	rt_n32 daylight_saving_time; /* Positive means daylight saving time is in effect, zero means not, negative means unknown. */
#ifdef RT_DEFINE_LINUX
	rt_n utc_offset;
	const rt_char8 *time_zone;
#endif
};

/**
 * Get current unix time, number of seconds since 1970-01-01 00:00:00 UTC.
 */
RT_API rt_s rt_time_get_unix_time(rt_n *unix_time);

/**
 * Allow to retrieve the current time with millisecond precision.
 * 
 * <p>
 * This function does not initialize day_of_week, day_of_year and daylight_saving_time fields.<br>
 * They can be initiliazed by calling <tt>rt_time_get_unix_time_from_time_info</tt>.
 * </p>
 * 
 * @param milliseconds If not RT_NULL, it will be filled with milliseconds since the last second.
 */
RT_API rt_s rt_time_info_create_accurate(struct rt_time_info *time_info, rt_un *milliseconds);

/**
 * Fill <tt>time_info</tt> (in local time) based on given <tt>unix_time</tt>.
 */
RT_API rt_s rt_time_info_create_local(struct rt_time_info *time_info, rt_n unix_time);

/**
 * <p>
 * This function will adjust <tt>time_info</tt>, for example if day is more than 31.<br>
 * <tt>day_of_week</tt> and <tt>day_of_year</tt> are initialized/fixed by this function.
 * </p>
 * 
 * @param unix_time If not RT_NULL, it will be filled with the number of seconds since 1970-01-01 00:00:00 UTC.
 */
RT_API rt_s rt_time_get_unix_time_from_time_info(struct rt_time_info *time_info, rt_n *unix_time);

#endif /* RT_TIME_H */
