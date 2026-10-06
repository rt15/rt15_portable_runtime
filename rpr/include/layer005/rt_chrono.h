#ifndef RT_CHRONO_H
#define RT_CHRONO_H

#include "layer000/rt_types.h"

/**
 * @file
 * Can be used to measure time durations.
 *
 * <p>
 * Based on <tt>rt_monotonic_clock</tt>, so not affected by system time changes.
 * </p>
 */

struct rt_chrono {
	/* Value of the monotonic clock when the chrono has been created, in nanoseconds. */
	rt_un64 start;
};

/**
 * No need for a destructor: the constructor only store the current time.
 */
RT_API rt_s rt_chrono_create(struct rt_chrono *chrono);

/**
 * Retrieves the duration since <tt>rt_chrono_create</tt>.
 *
 * <p>
 * Under 32 bits systems, <tt>micro_seconds</tt> overflows after around 71 minutes.
 * </p>
 */
RT_API rt_s rt_chrono_get_duration(struct rt_chrono *chrono, rt_un *micro_seconds);

#endif /* RT_CHRONO_H */
