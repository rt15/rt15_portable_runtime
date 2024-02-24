#include "layer002/rt_event.h"

#include "layer001/rt_os_headers.h"

rt_s rt_event_create(struct rt_event *event)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* A non-manual-reset event object is used to avoid race condition. */
	event->event_handle = CreateEvent(RT_NULL, RT_FALSE, RT_FALSE, RT_NULL);
	if (RT_UNLIKELY(!event->event_handle))
		goto error;
#else
	/* Initial value is set to zero to ensure that read calls will block. */
	/* File descriptor is not inherited. */
	event->file_descriptor = eventfd(0, EFD_CLOEXEC);
	if (RT_UNLIKELY(event->file_descriptor == -1))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_event_signal(struct rt_event *event)
{
#ifdef RT_DEFINE_LINUX
	uint64_t value = 1;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!SetEvent(event->event_handle)))
		goto error;
#else
	/* Using write on an eventfd object add given value to current value. */
	/* Beware that on an (old?) Linux, write might release a single waiting thread, not all. */
	if (RT_UNLIKELY(write(event->file_descriptor, &value, sizeof(uint64_t)) != sizeof(uint64_t)))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_event_wait_for(struct rt_event *event)
{
#ifdef RT_DEFINE_LINUX
	uint64_t value;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(WaitForSingleObject(event->event_handle, INFINITE) != WAIT_OBJECT_0))
		goto error;
#else
	/* This call to read will block on a zero eventfd object value until it is set to nonzero. */
	/* Then read set the eventfd object to zero. */
	if (RT_UNLIKELY(read(event->file_descriptor, &value, sizeof(uint64_t)) != sizeof(uint64_t)))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_event_reset(struct rt_event *event)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!ResetEvent(event->event_handle)))
		goto error;
#else
	/* Seems like the only way to reset is to re-create the eventfd. */
	if (RT_UNLIKELY(close(event->file_descriptor))) {
		event->file_descriptor = -1;
		goto error;
	}
	event->file_descriptor = eventfd(0, EFD_CLOEXEC);
	if (RT_UNLIKELY(event->file_descriptor == -1))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_event_free(struct rt_event *event)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!CloseHandle(event->event_handle)))
		goto error;
#else
	if (event->file_descriptor != -1) {
		/* The close function returns zero on success. */
		if (RT_UNLIKELY(close(event->file_descriptor)))
			goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
