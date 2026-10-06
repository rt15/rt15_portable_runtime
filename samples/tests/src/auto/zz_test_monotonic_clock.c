#include <rpr.h>

rt_s zz_test_monotonic_clock(void)
{
	rt_un64 before;
	rt_un64 after;
	rt_un64 elapsed;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_monotonic_clock_get(&before))) goto end;
	rt_sleep_sleep(100);
	if (RT_UNLIKELY(!rt_monotonic_clock_get(&after))) goto end;

	/* The clock must never go backward. */
	if (RT_UNLIKELY(after < before)) goto end;

	/* Around 100 milliseconds should have elapsed. Sleep can last longer on a busy system. */
	elapsed = after - before;
	if (RT_UNLIKELY(elapsed < 90000000)) goto end;
	if (RT_UNLIKELY(elapsed > 1000000000)) goto end;

	ret = RT_OK;
end:
	return ret;
}
