#include "layer003/rt_fast_initialization.h"

#include "layer001/rt_atomic.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_sleep.h"

rt_b rt_fast_initialization_is_required(struct rt_fast_initialization *fast_initialization)
{
	rt_b ret;

	/* We have to use an atomic operation here. */
	/* Yes, if initialized is true, it means that a thread does have perform the initialization. */
	/* We are sure that the initialization has been "written" to memory by the other thread. */
	/* But we also must be sure that this thread will read this just written memory. */
	/* So we need a memory barrier there. */
	if (RT_ATOMIC_LOAD(fast_initialization->initialized)) {
		/* Initialized is true and we have put a memory barrier, so we are sure that initialization is done and accessible. */
		ret = RT_FALSE;
	} else {
		if (RT_ATOMIC_TRY_TO_ACQUIRE(fast_initialization->lock)) {
			/* We are the first and the only one that has set 1 in initializing, so we are responsible for initialization. */
			ret = RT_TRUE;
		} else {
			/* Another thread is already performing initialization. */
			while (!RT_ATOMIC_LOAD(fast_initialization->initialized))
				rt_sleep_yield();
			ret = RT_FALSE;
		}
	}
	return ret;
}

void rt_fast_initialization_notify_done(struct rt_fast_initialization *fast_initialization)
{
	RT_ATOMIC_STORE(fast_initialization->initialized, 1);
}

rt_b rt_fast_initialization_is_done(struct rt_fast_initialization *fast_initialization)
{
	/* initialized should contain either 0 or 1, so it should be safe to cast it from rt_un into rt_b. */
	return RT_ATOMIC_LOAD(fast_initialization->initialized);
}
