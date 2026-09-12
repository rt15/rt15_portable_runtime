#include "layer003/rt_condition_variable.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_condition_variable_create(struct rt_condition_variable *condition_variable, struct rt_critical_section *critical_section)
{
#ifdef RT_DEFINE_WINDOWS
	rt_b semaphore_handle_created = RT_FALSE;
	rt_b waiters_count_lock_created = RT_FALSE;
	rt_b waiters_done_event_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	condition_variable->critical_section = critical_section;

	/* The handle is not inherited. */
	/* Returns null and sets last error in case of issue. */
	condition_variable->semaphore_handle = CreateSemaphore(RT_NULL, 0, RT_TYPE_MAX_N32, RT_NULL);
	if (RT_UNLIKELY(!condition_variable->semaphore_handle))
		goto end;
	semaphore_handle_created = RT_TRUE;

	/* Initialize waiters count and waiters count lock. */
	condition_variable->waiters_count = 0;
	if (RT_UNLIKELY(!rt_critical_section_create(&condition_variable->waiters_count_lock, RT_FALSE)))
		goto end;
	waiters_count_lock_created = RT_TRUE;

	condition_variable->was_broadcast = RT_FALSE;

	if (RT_UNLIKELY(!rt_event_create(&condition_variable->waiters_done_event)))
		goto end;
	waiters_done_event_created = RT_TRUE;

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret)) {
		if (waiters_done_event_created)
			rt_event_free(&condition_variable->waiters_done_event);

		if (waiters_count_lock_created)
			rt_critical_section_free(&condition_variable->waiters_count_lock);

		if (semaphore_handle_created)
			CloseHandle(condition_variable->semaphore_handle);
	}

	return ret;
#else
	int error;
	rt_s ret = RT_FAILED;

	condition_variable->critical_section = critical_section;

	error = pthread_cond_init((pthread_cond_t*)&condition_variable->data, RT_NULL);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
#endif
}

rt_s rt_condition_variable_wait(struct rt_condition_variable *condition_variable)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
	rt_b last_waiter;
	rt_s ret = RT_FAILED;

	/* Increment the count of waiters. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&condition_variable->waiters_count_lock)))
		goto end;
	condition_variable->waiters_count++;
	if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable->waiters_count_lock))) {
		condition_variable->waiters_count--;
		goto end;
	}

	/* Release the critical section as we gonna wait. */
	if (RT_UNLIKELY(!rt_critical_section_leave(condition_variable->critical_section))) {
		condition_variable->waiters_count--;
		goto end;
	}

	/* Wait on the semaphore. Signal/broadcast will release it. */
	returned_value = WaitForSingleObject(condition_variable->semaphore_handle, INFINITE);
	if (returned_value != WAIT_OBJECT_0) {
		/* If returned value is WAIT_FAILED, then last error is set. */
		if (RT_UNLIKELY(returned_value != WAIT_FAILED)) {
			/* Set an arbitrary last error for WAIT_TIMEOUT/WAIT_ABANDONED cases. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}

		rt_critical_section_enter(&condition_variable->waiters_count_lock);
		condition_variable->waiters_count--;
		last_waiter = (condition_variable->was_broadcast && !condition_variable->waiters_count);
		rt_critical_section_leave(&condition_variable->waiters_count_lock);

		/* Notify the broadcasting thread that all waiters are done. */
		if (last_waiter) {
			rt_event_signal(&condition_variable->waiters_done_event);
		}

		rt_critical_section_enter(condition_variable->critical_section);
		goto end;
	}

	/* Decrement the count of waiters. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&condition_variable->waiters_count_lock))) {
		/* We failed to acquire the lock, but we have to decrement the number of waiters anyway. */
		condition_variable->waiters_count--;
		last_waiter = (condition_variable->was_broadcast && !condition_variable->waiters_count);

		/* Notify the broadcasting thread that all waiters are done. */
		if (last_waiter) {
			rt_event_signal(&condition_variable->waiters_done_event);
		}

		rt_critical_section_enter(condition_variable->critical_section);
		goto end;
	}
	condition_variable->waiters_count--;
	last_waiter = (condition_variable->was_broadcast && !condition_variable->waiters_count);
	if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable->waiters_count_lock))) {
		/* Notify the broadcasting thread that all waiters are done. */
		if (last_waiter) {
			rt_event_signal(&condition_variable->waiters_done_event);
		}

		rt_critical_section_enter(condition_variable->critical_section);
		goto end;
	}

	/* Notify the broadcasting thread that all waiters are done. */
	if (last_waiter) {
		if (RT_UNLIKELY(!rt_event_signal(&condition_variable->waiters_done_event))) {
			rt_critical_section_enter(condition_variable->critical_section);
			goto end;
		}
	}

	/* Reacquire the critical section. */
	if (RT_UNLIKELY(!rt_critical_section_enter(condition_variable->critical_section)))
		goto end;
	
	ret = RT_OK;
end:
	return ret;
#else
	int error;
	rt_s ret = RT_FAILED;

	error = pthread_cond_wait((pthread_cond_t*)&condition_variable->data, (pthread_mutex_t*)condition_variable->critical_section);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
#endif
}

rt_s rt_condition_variable_signal(struct rt_condition_variable *condition_variable)
{
#ifdef RT_DEFINE_WINDOWS
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&condition_variable->waiters_count_lock)))
		goto end;

	if (condition_variable->waiters_count > 0) {
		/* Returns FALSE and set last error in case of issue. */
		if (RT_UNLIKELY(!ReleaseSemaphore(condition_variable->semaphore_handle, 1, RT_NULL))) {
			rt_critical_section_leave(&condition_variable->waiters_count_lock);
			goto end;
		}
	}

	if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable->waiters_count_lock)))
		goto end;

	ret = RT_OK;
end:
	return ret;
#else
	int error;
	rt_s ret = RT_FAILED;

	error = pthread_cond_signal((pthread_cond_t*)&condition_variable->data);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
#endif
}

rt_s rt_condition_variable_broadcast(struct rt_condition_variable *condition_variable)
{
#ifdef RT_DEFINE_WINDOWS
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&condition_variable->waiters_count_lock)))
		goto end;

	if (condition_variable->waiters_count > 0) {
		/* Returns FALSE and set last error in case of issue. */
		if (RT_UNLIKELY(!ReleaseSemaphore(condition_variable->semaphore_handle, (LONG)condition_variable->waiters_count, RT_NULL))) {
			rt_critical_section_leave(&condition_variable->waiters_count_lock);
			goto end;
		}

		condition_variable->was_broadcast = RT_TRUE;

		if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable->waiters_count_lock))) {
			condition_variable->was_broadcast = RT_FALSE;
			goto end;
		}

		/* The last released waiter signals the event. */
		if (RT_UNLIKELY(!rt_event_wait_for(&condition_variable->waiters_done_event))) {
			condition_variable->was_broadcast = RT_FALSE;
			goto end;
		}

		condition_variable->was_broadcast = RT_FALSE;

	} else {
		if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable->waiters_count_lock)))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
#else
	int error;
	rt_s ret = RT_FAILED;

	error = pthread_cond_broadcast((pthread_cond_t*)&condition_variable->data);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
#endif
}

rt_s rt_condition_variable_free(struct rt_condition_variable *condition_variable)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!rt_event_free(&condition_variable->waiters_done_event)))
		ret = RT_FAILED;
	
	if (RT_UNLIKELY(!rt_critical_section_free(&condition_variable->waiters_count_lock)))
		ret = RT_FAILED;

	/* Returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!CloseHandle(condition_variable->semaphore_handle)))
		ret = RT_FAILED;
#else
	error = pthread_cond_destroy((pthread_cond_t*)&condition_variable->data);
	if (RT_UNLIKELY(error)) {
		errno = error;
		ret = RT_FAILED;
	}
#endif

	return ret;
}
