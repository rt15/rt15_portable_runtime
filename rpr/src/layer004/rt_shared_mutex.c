#include "layer004/rt_shared_mutex.h"

#include "layer002/rt_error.h"
#include "layer003/rt_spinlock.h"

rt_s rt_shared_mutex_create(struct rt_shared_mutex *shared_mutex)
{
	rt_b critical_section_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_critical_section_create(&shared_mutex->critical_section, RT_FALSE)))
		goto error;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_create(&shared_mutex->event)))
		goto error;
	event_created = RT_TRUE;

	shared_mutex->readers_count = 0;
	shared_mutex->writer = RT_FALSE;
	shared_mutex->spinlock = 0;

	ret = RT_OK;
free:
	return ret;

error:
	if (event_created) {
		event_created = RT_FALSE;
		rt_event_free(&shared_mutex->event);
	}
	if (critical_section_created) {
		critical_section_created = RT_FALSE;
		rt_critical_section_free(&shared_mutex->critical_section);
	}
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_mutex_acquire_read(struct rt_shared_mutex *shared_mutex)
{
	rt_s ret;

	/* Check that there is no writing in progress. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&shared_mutex->critical_section)))
		goto error;

	/* We protect the readers_count update with the spinlock because a releasing reader might update it too. */
	RT_SPINLOCK_ACQUIRE(shared_mutex->spinlock);
	/* Signal that we gonna read. There is no race condition as we acquired the critical section. */
	shared_mutex->readers_count++;
	RT_SPINLOCK_RELEASE(shared_mutex->spinlock);

	/* Release the critical section so that others reader and writer can try to acquire it. */
	if (RT_UNLIKELY(!rt_critical_section_leave(&shared_mutex->critical_section)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_mutex_release_read(struct rt_shared_mutex *shared_mutex)
{
	rt_s ret;

	RT_SPINLOCK_ACQUIRE(shared_mutex->spinlock);
	shared_mutex->readers_count--;
	/* The last reader signals to the writer that it can now write. */
	if (!shared_mutex->readers_count && shared_mutex->writer) {
		if (RT_UNLIKELY(!rt_event_signal(&shared_mutex->event))) {
			RT_SPINLOCK_RELEASE(shared_mutex->spinlock);
			goto error;
		}
	}
	RT_SPINLOCK_RELEASE(shared_mutex->spinlock);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_mutex_acquire_write(struct rt_shared_mutex *shared_mutex)
{
	rt_b wait = RT_FALSE;
	rt_s ret;

	/* We acquire the critical section until acquire_release is called. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&shared_mutex->critical_section)))
		goto error;
	
	RT_SPINLOCK_ACQUIRE(shared_mutex->spinlock);
	shared_mutex->writer = RT_TRUE;
	if (shared_mutex->readers_count) {
		wait = RT_TRUE;
		/* Make sure the event is ready to be signaled by the last reader. */
		if (RT_UNLIKELY(!rt_event_reset(&shared_mutex->event))) {
			rt_critical_section_leave(&shared_mutex->critical_section);
			RT_SPINLOCK_RELEASE(shared_mutex->spinlock);
			goto error;
		}
	}
	RT_SPINLOCK_RELEASE(shared_mutex->spinlock);

	if (wait) {
		/* Wait for the last reader to signal that it is finished. */
		if (RT_UNLIKELY(!rt_event_wait_for(&shared_mutex->event))) {
			rt_critical_section_leave(&shared_mutex->critical_section);
			goto error;
		}
		/* Check readers_count, just to be sure. */
		if (RT_UNLIKELY(shared_mutex->readers_count)) {
			rt_critical_section_leave(&shared_mutex->critical_section);
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_mutex_release_write(struct rt_shared_mutex *shared_mutex)
{
	rt_s ret;

	RT_SPINLOCK_ACQUIRE(shared_mutex->spinlock);
	shared_mutex->writer = RT_FALSE;
	RT_SPINLOCK_RELEASE(shared_mutex->spinlock);

	if (RT_UNLIKELY(!rt_critical_section_leave(&shared_mutex->critical_section)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_mutex_free(struct rt_shared_mutex *shared_mutex)
{
	rt_s ret = RT_OK;

	if (RT_UNLIKELY(!rt_event_free(&shared_mutex->event)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_free(&shared_mutex->critical_section)))
		ret = RT_FAILED;

	return ret;
}
