#include "layer004/rt_monotonic_clock.h"

#include "layer001/rt_os_headers.h"
#include "layer003/rt_fast_initialization.h"

#ifdef RT_DEFINE_WINDOWS

static struct rt_fast_initialization rt_monotonic_clock_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;

/* Whether QueryPerformanceFrequency has been successful or not. */
static rt_b rt_monotonic_clock_initialization_successful = RT_FALSE;

/* Error code of failed initialization. */
static DWORD rt_monotonic_clock_initialization_error;

/* Number of counts per second. Fixed at system boot. */
static rt_un64 rt_monotonic_clock_frequency;

#endif

rt_s rt_monotonic_clock_get(rt_un64 *nanoseconds)
{
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter_value;
	rt_un64 counter;
#else
	struct timespec time_spec;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS

	if (rt_fast_initialization_is_required(&rt_monotonic_clock_initialization)) {
		/* Returns zero and sets last error in case of failure. Cannot fail since Windows XP. */
		if (QueryPerformanceFrequency(&frequency)) {
			rt_monotonic_clock_frequency = (rt_un64)frequency.QuadPart;
			rt_monotonic_clock_initialization_successful = RT_TRUE;
		} else {
			rt_monotonic_clock_initialization_error = GetLastError();
		}
		rt_fast_initialization_notify_done(&rt_monotonic_clock_initialization);
	}
	if (RT_UNLIKELY(!rt_monotonic_clock_initialization_successful)) {
		/* Set last error as when the initialization has failed. */
		SetLastError(rt_monotonic_clock_initialization_error);
		goto end;
	}

	/* Returns zero and sets last error in case of failure. Cannot fail since Windows XP. */
	if (RT_UNLIKELY(!QueryPerformanceCounter(&counter_value)))
		goto end;
	counter = (rt_un64)counter_value.QuadPart;

	/* counter * 1000000000 would overflow after a few minutes or hours of uptime, so seconds and remainder are converted separately. */
	/* The remainder is less than the frequency, so remainder * 1000000000 does not overflow as long as the frequency is less than 18 GHz. */
	*nanoseconds = (counter / rt_monotonic_clock_frequency) * 1000000000 + (counter % rt_monotonic_clock_frequency) * 1000000000 / rt_monotonic_clock_frequency;

#else

	/* clock_gettime returns -1 and sets errno in case of failure. */
	if (RT_UNLIKELY(clock_gettime(CLOCK_MONOTONIC, &time_spec) == -1))
		goto end;

	/* Cast before multiplying as tv_sec is 32 bits on 32 bits systems. */
	*nanoseconds = (rt_un64)time_spec.tv_sec * 1000000000 + (rt_un64)time_spec.tv_nsec;

#endif

	ret = RT_OK;
end:
	return ret;
}
