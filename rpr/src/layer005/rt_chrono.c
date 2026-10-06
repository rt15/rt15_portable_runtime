#include "layer005/rt_chrono.h"

#include "layer004/rt_monotonic_clock.h"

rt_s rt_chrono_create(struct rt_chrono *chrono)
{
	return rt_monotonic_clock_get(&chrono->start);
}

rt_s rt_chrono_get_duration(struct rt_chrono *chrono, rt_un *micro_seconds)
{
	rt_un64 now;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_monotonic_clock_get(&now)))
		goto end;

	/* Nano to micro. */
	*micro_seconds = (rt_un)((now - chrono->start) / 1000);

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret))
		*micro_seconds = 0;

	return ret;
}
