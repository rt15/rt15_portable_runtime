#include <rpr.h>

enum zz_thread_status {
	ZZ_THREAD_STATUS_INIT,
	ZZ_THREAD_STATUS_SIGNAL_1,
	ZZ_THREAD_STATUS_SIGNAL_2
};

static enum zz_thread_status thread_status;

static rt_un32 RT_STDCALL zz_test_event_thread_callback(void *parameter)
{
	struct rt_event *event;
	rt_un32 result;

	event = (struct rt_event*)parameter;

	if (RT_UNLIKELY(!rt_event_wait_for(event)))
		goto error;

	thread_status = ZZ_THREAD_STATUS_SIGNAL_1;

	if (RT_UNLIKELY(!rt_event_wait_for(event)))
		goto error;

	thread_status = ZZ_THREAD_STATUS_SIGNAL_2;

	result = RT_OK;
free:
	return result;

error:
	result = RT_FAILED;
	goto free;
}

static rt_s zz_test_event_already_signaled(void)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	rt_s ret;

	thread_status = ZZ_THREAD_STATUS_INIT;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto error;
	event_created = RT_TRUE;

	/* Signal the event even before creating the thread. */
	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto error;

	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_INIT))
		goto error;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &event)))
		goto error;
	thread_created = RT_TRUE;

	/* Let other thread deals with the first event which is already set. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_1))
		goto error;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto error;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_2))
		goto error;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (event_created) {
		event_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_event_free(&event) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_event_not_signaled_yet(void)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	rt_s ret;

	thread_status = ZZ_THREAD_STATUS_INIT;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto error;
	event_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &event)))
		goto error;
	thread_created = RT_TRUE;

	/* Let other thread wait for the event. */
	rt_sleep_sleep(10);

	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_INIT))
		goto error;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto error;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_1))
		goto error;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto error;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_2))
		goto error;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (event_created) {
		event_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_event_free(&event) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_event(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_event_already_signaled())) goto error;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
