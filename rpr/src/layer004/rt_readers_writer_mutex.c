#include "layer004/rt_readers_writer_mutex.h"

#include "layer002/rt_error.h"
#include "layer003/rt_spinlock.h"

rt_s rt_readers_writer_mutex_create(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_b critical_section_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_create(&readers_writer_mutex->critical_section, RT_FALSE)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_create(&readers_writer_mutex->event)))
		goto end;
	event_created = RT_TRUE;

	readers_writer_mutex->readers_count = 0;
	readers_writer_mutex->writer = RT_FALSE;
	readers_writer_mutex->spinlock = 0;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (event_created) {
			rt_event_free(&readers_writer_mutex->event);
		}
		if (critical_section_created) {
			rt_critical_section_free(&readers_writer_mutex->critical_section);
		}
	}

	return ret;
}

rt_s rt_readers_writer_mutex_acquire_read(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_s ret = RT_FAILED;

	/* Check that there is no writing in progress. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&readers_writer_mutex->critical_section)))
		goto end;

	/* We protect the readers_count update with the spinlock because a releasing reader might update it too. */
	RT_SPINLOCK_ACQUIRE(readers_writer_mutex->spinlock);
	/* Signal that we gonna read. There is no race condition as we acquired the critical section. */
	readers_writer_mutex->readers_count++;
	RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);

	/* Release the critical section so that others reader and writer can try to acquire it. */
	if (RT_UNLIKELY(!rt_critical_section_leave(&readers_writer_mutex->critical_section)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_readers_writer_mutex_release_read(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_s ret = RT_FAILED;

	RT_SPINLOCK_ACQUIRE(readers_writer_mutex->spinlock);
	readers_writer_mutex->readers_count--;
	/* The last reader signals to the writer that it can now write. */
	if (!readers_writer_mutex->readers_count && readers_writer_mutex->writer) {
		if (RT_UNLIKELY(!rt_event_signal(&readers_writer_mutex->event))) {
			RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);
			goto end;
		}
	}
	RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_readers_writer_mutex_acquire_write(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_b wait = RT_FALSE;
	rt_s ret = RT_FAILED;

	/* We acquire the critical section until acquire_release is called. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&readers_writer_mutex->critical_section)))
		goto end;
	
	RT_SPINLOCK_ACQUIRE(readers_writer_mutex->spinlock);
	readers_writer_mutex->writer = RT_TRUE;
	if (readers_writer_mutex->readers_count) {
		wait = RT_TRUE;
		/* Make sure the event is ready to be signaled by the last reader. */
		if (RT_UNLIKELY(!rt_event_reset(&readers_writer_mutex->event))) {
			rt_critical_section_leave(&readers_writer_mutex->critical_section);
			RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);
			goto end;
		}
	}
	RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);

	if (wait) {
		/* Wait for the last reader to signal that it is finished. */
		if (RT_UNLIKELY(!rt_event_wait_for(&readers_writer_mutex->event))) {
			rt_critical_section_leave(&readers_writer_mutex->critical_section);
			goto end;
		}
		/* Check readers_count, just to be sure. */
		if (RT_UNLIKELY(readers_writer_mutex->readers_count)) {
			rt_critical_section_leave(&readers_writer_mutex->critical_section);
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto end;
		}
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_readers_writer_mutex_release_write(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_s ret = RT_FAILED;

	RT_SPINLOCK_ACQUIRE(readers_writer_mutex->spinlock);
	readers_writer_mutex->writer = RT_FALSE;
	RT_SPINLOCK_RELEASE(readers_writer_mutex->spinlock);

	if (RT_UNLIKELY(!rt_critical_section_leave(&readers_writer_mutex->critical_section)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_readers_writer_mutex_free(struct rt_readers_writer_mutex *readers_writer_mutex)
{
	rt_s ret = RT_OK;

	if (RT_UNLIKELY(!rt_event_free(&readers_writer_mutex->event)))
		ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_free(&readers_writer_mutex->critical_section)))
		ret = RT_FAILED;

	return ret;
}
