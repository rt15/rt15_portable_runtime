#ifndef RT_MONOTONIC_CLOCK_H
#define RT_MONOTONIC_CLOCK_H

#include "layer000/rt_types.h"

/**
 * @file
 * Clock that only moves forward at a steady rate.
 *
 * <p>
 * Unlike the system time, it is not affected by system time changes (by the user, NTP...).<br>
 * As a result, it can be used to compute deadlines and to measure durations.
 * </p>
 *
 * <p>
 * Windows implementation details:<br>
 * <tt>QueryPerformanceCounter</tt> is used.<br>
 * The resolution is <tt>1 / QueryPerformanceFrequency</tt>, typically 100 nanoseconds.
 * </p>
 *
 * <p>
 * Linux implementation details:<br>
 * <tt>clock_gettime(CLOCK_MONOTONIC)</tt> is used, the clock used by kernel timeouts (<tt>poll</tt>...).<br>
 * The resolution is typically 1 nanosecond.<br>
 * The time spent in suspend is not counted.
 * </p>
 */

/**
 * Retrieves the number of nanoseconds elapsed since an arbitrary point (typically the boot).
 *
 * <p>
 * Only differences between two values are meaningful.
 * </p>
 */
RT_API rt_s rt_monotonic_clock_get(rt_un64 *nanoseconds);

#endif /* RT_MONOTONIC_CLOCK_H */
