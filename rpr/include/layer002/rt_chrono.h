#ifndef RT_CHRONO_H
#define RT_CHRONO_H

#include "layer000/rt_types.h"

/**
 * @file
 * Can be used to measure time durations.
 */


struct rt_chrono {
#ifdef RT_DEFINE_WINDOWS
	/* LARGE_INTEGER. */
	rt_un64 start_counter;
#else
	/* timespec (time_t and long). */
	rt_n seconds;
	rt_n nano_seconds;
#endif
};

/**
 * No need for a destructor: the constructor only store the current time.
 */
rt_s rt_chrono_create(struct rt_chrono *chrono);
rt_s rt_chrono_get_duration(struct rt_chrono *chrono, rt_un *micro_seconds);

#endif /* RT_CHRONO_H */
