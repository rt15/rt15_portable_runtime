#include "layer002/rt_sleep.h"

#include "layer001/rt_os_headers.h"

void rt_sleep_yield()
{
#ifdef RT_DEFINE_WINDOWS
	Sleep(0);
#else
	/* Result is not checked as the Linux implementation always succeeds. */
	sched_yield();
#endif
}

void rt_sleep_sleep(rt_un32 milliseconds)
{
#ifndef RT_DEFINE_WINDOWS
	struct timespec requested;
#endif

#ifdef RT_DEFINE_WINDOWS
	Sleep(milliseconds);
#else
	/* Normalize, tv_nsec must be less than 999999999 (1 second). */
	requested.tv_sec = milliseconds / 1000;
	requested.tv_nsec = (milliseconds % 1000) * 1000000;
loop:
	/* Second argument is the remaining time if the function is interrupted. */
	if (nanosleep(&requested, &requested)) {
		/* The nanosleep can be interrupted by a signal. Other errors are ignored. */
		if (errno == EINTR) {
			/* Interrupted, wait for remaining time. */
			goto loop;
		}
	}
#endif
}
