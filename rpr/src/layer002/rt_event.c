#include "layer002/rt_event.h"

#include "layer001/rt_os_headers.h"

rt_s rt_event_create(struct rt_event *event)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* A non-manual-reset event object is used to avoid race condition. */
	event->event_handle = CreateEvent(RT_NULL, RT_FALSE, RT_FALSE, RT_NULL);
	if (RT_UNLIKELY(!event->event_handle))
		goto end;
#else
	/* Initial value is set to zero to ensure that read calls will block. */
	/* File descriptor is not inherited. */
	event->file_descriptor = eventfd(0, EFD_CLOEXEC);
	if (RT_UNLIKELY(event->file_descriptor == -1))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_event_signal(struct rt_event *event)
{
#ifdef RT_DEFINE_LINUX
	uint64_t value = 1;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!SetEvent(event->event_handle)))
		goto end;
#else
	/* Using write on an eventfd object add given value to current value. */
	/* Beware that on an (old?) Linux, write might release a single waiting thread, not all. */
	if (RT_UNLIKELY(write(event->file_descriptor, &value, sizeof(uint64_t)) != sizeof(uint64_t)))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_event_wait(struct rt_event *event)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#else
	uint64_t value;
	ssize_t bytes_read;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	returned_value = WaitForSingleObject(event->event_handle, INFINITE);
	if (RT_UNLIKELY(returned_value != WAIT_OBJECT_0)) {
		/* If returned value is WAIT_FAILED, then last error is set. */
		if (RT_UNLIKELY(returned_value != WAIT_FAILED)) {
			/* Set an arbitrary last error for WAIT_TIMEOUT/WAIT_ABANDONED cases. */
			SetLastError(ERROR_INVALID_FUNCTION);
		}
		goto end;
	}
#else
	/* This call to read will block on a zero eventfd object value until it is set to nonzero. */
	/* Then read set the eventfd object to zero. */
	while (RT_TRUE) {
		bytes_read = read(event->file_descriptor, &value, sizeof(uint64_t));
		if (RT_LIKELY(bytes_read == (ssize_t)sizeof(uint64_t)))
			break;
		/* Read is restarted after a signal handler only if the handler has been installed with SA_RESTART. */
		if (RT_UNLIKELY(bytes_read != -1 || errno != EINTR))
			goto end;
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_event_wait_with_timeout(struct rt_event *event, rt_un32 timeout_milliseconds, rt_b *signaled)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#else
	struct timespec time_spec;
	rt_un64 deadline_nanoseconds;
	rt_un64 current_nanoseconds;
	struct pollfd poll_file_descriptor;
	int poll_timeout;
	int returned_value;
	uint64_t value;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	returned_value = WaitForSingleObject(event->event_handle, timeout_milliseconds);
	if (returned_value == WAIT_OBJECT_0) {
		/* The event has been signaled. */
		*signaled = RT_TRUE;
	} else if (returned_value == WAIT_TIMEOUT) {
		/* Timeout has been reached. */
		*signaled = RT_FALSE;
	} else {
		/* If returned value is WAIT_FAILED, then last error is set. */
		if (RT_UNLIKELY(returned_value != WAIT_FAILED)) {
			/* Set an arbitrary last error for WAIT_ABANDONED case. */
			SetLastError(ERROR_INVALID_FUNCTION);
		}
		goto end;
	}
#else
	if (timeout_milliseconds == RT_TYPE_MAX_UN32) {
		/* We have to wait forever, so we simply call the function without timeout. */
		if (RT_UNLIKELY(!rt_event_wait(event)))
			goto end;
		*signaled = RT_TRUE;
	} else if (timeout_milliseconds > RT_TYPE_MAX_N32) {
		/* The timeout argument of poll is an int so it cannot be greater than RT_TYPE_MAX_N32. */
		/* So we split into two calls, one with RT_TYPE_MAX_N32 and one with what remains. */
		if (RT_UNLIKELY(!rt_event_wait_with_timeout(event, RT_TYPE_MAX_N32, signaled)))
			goto end;
		if (!*signaled) {
			if (RT_UNLIKELY(!rt_event_wait_with_timeout(event, timeout_milliseconds - RT_TYPE_MAX_N32, signaled)))
				goto end;
		}
	} else {
		/* The file descriptor is -1 if rt_event_reset failed, and poll silently ignores negative file descriptors. */
		if (RT_UNLIKELY(event->file_descriptor == -1)) {
			errno = EBADF;
			goto end;
		}

		poll_file_descriptor.fd = event->file_descriptor;
		poll_file_descriptor.events = POLLIN;

		/* clock_gettime returns -1 and sets errno in case of failure. */
		if (RT_UNLIKELY(clock_gettime(CLOCK_MONOTONIC, &time_spec) == -1))
			goto end;

		/* Cast before multiplying as tv_sec is 32 bits on 32 bits systems. */
		deadline_nanoseconds = (rt_un64)time_spec.tv_sec * 1000000000 + (rt_un64)time_spec.tv_nsec + (rt_un64)timeout_milliseconds * 1000000;

		poll_timeout = (int)timeout_milliseconds;

		while (RT_TRUE) {
			/* Returns the number of ready file descriptors, zero on timeout, -1 and sets errno on failure. */
			returned_value = poll(&poll_file_descriptor, 1, poll_timeout);
			if (returned_value > 0) {
				/* Something happened on the file descriptor, let's check that. */
				if (RT_UNLIKELY(!(poll_file_descriptor.revents & POLLIN))) {
					/* POLLNVAL if the file descriptor is invalid. */
					errno = (poll_file_descriptor.revents & POLLNVAL) ? EBADF : EIO;
					goto end;
				}
				/* The eventfd value is nonzero so read will not block (single waiter) and will set it to zero. */
				if (RT_UNLIKELY(read(event->file_descriptor, &value, sizeof(uint64_t)) != sizeof(uint64_t)))
					goto end;
				*signaled = RT_TRUE;
				break;
			} else if (!returned_value) {
				/* Zero means we reached the timeout. */
				*signaled = RT_FALSE;
				break;
			}

			/* An error occurred in poll, but we must ignore EINTR. */
			if (RT_UNLIKELY(errno != EINTR))
				goto end;

			/* We have been interrupted, we have to compute a new timeout for the remaining time. */

			/* clock_gettime returns -1 and sets errno in case of failure. */
			if (RT_UNLIKELY(clock_gettime(CLOCK_MONOTONIC, &time_spec) == -1))
				goto end;

			/* Cast before multiplying as tv_sec is 32 bits on 32 bits systems. */
			current_nanoseconds = (rt_un64)time_spec.tv_sec * 1000000000 + (rt_un64)time_spec.tv_nsec;

			if (current_nanoseconds >= deadline_nanoseconds) {
				/* Deadline reached, still check the event once. */
				poll_timeout = 0;
			} else {
				/* Round up to not return before the deadline. */
				poll_timeout = (int)((deadline_nanoseconds - current_nanoseconds + 999999) / 1000000);
			}
		}
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_event_reset(struct rt_event *event)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!ResetEvent(event->event_handle)))
		goto end;
#else
	/* Seems like the only way to reset is to re-create the eventfd. */
	if (RT_UNLIKELY(close(event->file_descriptor))) {
		event->file_descriptor = -1;
		goto end;
	}
	event->file_descriptor = eventfd(0, EFD_CLOEXEC);
	if (RT_UNLIKELY(event->file_descriptor == -1))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_event_free(struct rt_event *event)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!CloseHandle(event->event_handle)))
		goto end;
#else
	if (event->file_descriptor != -1) {
		/* The close function returns zero on success. */
		if (RT_UNLIKELY(close(event->file_descriptor)))
			goto end;
	}
#endif

	ret = RT_OK;
end:
	return ret;
}
