#include "layer002/rt_chrono.h"

#include "layer001/rt_os_headers.h"

#ifdef RT_DEFINE_LINUX

/**
 * Subtract the struct timespec values x and y, storing the result in result.
 */
static void rt_chrono_substract_timespecs(struct timespec *x, struct timespec *y, struct timespec *result)
{
	struct timespec local_y;
	int seconds;

	/* Perform a copy of Y as we will adjust it. */
	local_y = *y;

	/* Ensure that x->tv_nsec is greater than local_y.tv_nsec. */
	if (x->tv_nsec < local_y.tv_nsec) {
		/* Compute seconds that will be removed from local_y.tv_nsec. */
		seconds = (local_y.tv_nsec - x->tv_nsec) / 1000000000 + 1;
		/* Remove seconds from local_y.tv_nsec. */
		local_y.tv_nsec -= 1000000000 * seconds;
		/* Add them back to local_y.tv_sec. */
		local_y.tv_sec += seconds;
	}
	/* Ensure that result->tv_nsec will be less than a second. */
	if (x->tv_nsec - local_y.tv_nsec > 1000000000) {
		/* Compute the surplus seconds. */
		seconds = (x->tv_nsec - local_y.tv_nsec) / 1000000000;
		/* Add them to local_y.tv_nsec. */
		local_y.tv_nsec += 1000000000 * seconds;
		/* Remove them of local_y.tv_sec. It might become negative but we don't care. */
		local_y.tv_sec -= seconds;
	}

	result->tv_sec = x->tv_sec - local_y.tv_sec;
	result->tv_nsec = x->tv_nsec - local_y.tv_nsec;
}

#endif

rt_s rt_chrono_create(struct rt_chrono *chrono)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (!QueryPerformanceCounter((LARGE_INTEGER*)chrono))
		goto error;
#else
	/* clock_gettime returns -1 and set errno in case of failure. */
	if (clock_gettime(CLOCK_MONOTONIC_RAW, (struct timespec*)chrono) == -1)
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_chrono_get_duration(struct rt_chrono *chrono, rt_un *micro_seconds)
{
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER *start_counter;
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter_value;
#else
	struct timespec *start_counter;
	struct timespec counter_value;
	struct timespec difference;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	start_counter = (LARGE_INTEGER*)chrono;

	if (!QueryPerformanceCounter(&counter_value))
		goto error;

	/* Beware that returned frequency can be greater than processor frequency and RT_TYPE_MAX_UN32. */
	if (!QueryPerformanceFrequency(&frequency))
		goto error;

	*micro_seconds = (rt_un)((counter_value.QuadPart - start_counter->QuadPart) * 1000000 / frequency.QuadPart);
#else
	start_counter = (struct timespec*)chrono;

	/* clock_gettime returns -1 and set errno in case of failure. */
	if (clock_gettime(CLOCK_MONOTONIC_RAW, &counter_value) == -1)
		goto error;

	rt_chrono_substract_timespecs(&counter_value, start_counter, &difference);

	 /* Nano to micro and seconds to micro. */
	*micro_seconds = difference.tv_sec * 1000000 + difference.tv_nsec / 1000;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	*micro_seconds = 0;
	ret = RT_FAILED;
	goto free;
}
