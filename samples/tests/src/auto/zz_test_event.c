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
	rt_un32 result = RT_FAILED;

	event = (struct rt_event*)parameter;

	if (RT_UNLIKELY(!rt_event_wait_for(event)))
		goto end;

	thread_status = ZZ_THREAD_STATUS_SIGNAL_1;

	if (RT_UNLIKELY(!rt_event_wait_for(event)))
		goto end;

	thread_status = ZZ_THREAD_STATUS_SIGNAL_2;

	result = RT_OK;
end:
	return result;
}

static rt_s zz_test_event_already_signaled(void)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	rt_s ret = RT_FAILED;

	thread_status = ZZ_THREAD_STATUS_INIT;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto end;
	event_created = RT_TRUE;

	/* Signal the event even before creating the thread. */
	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto end;

	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_INIT))
		goto end;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &event)))
		goto end;
	thread_created = RT_TRUE;

	/* Let other thread deals with the first event which is already set. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_1))
		goto end;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto end;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_2))
		goto end;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (event_created) {
		if (RT_UNLIKELY(!rt_event_free(&event)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_event_not_signaled_yet(rt_b reset)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	rt_s ret = RT_FAILED;

	thread_status = ZZ_THREAD_STATUS_INIT;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto end;
	event_created = RT_TRUE;

	if (reset) {
		if (RT_UNLIKELY(!rt_event_signal(&event)))
			goto end;

		if (RT_UNLIKELY(!rt_event_reset(&event)))
			goto end;
	}

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &event)))
		goto end;
	thread_created = RT_TRUE;

	/* Let other thread wait for the event. */
	rt_sleep_sleep(10);

	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_INIT))
		goto end;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto end;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_1))
		goto end;

	if (RT_UNLIKELY(!rt_event_signal(&event)))
		goto end;

	/* Wait for thread to set status flag. */
	rt_sleep_sleep(10);
	if (RT_UNLIKELY(thread_status != ZZ_THREAD_STATUS_SIGNAL_2))
		goto end;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (event_created) {
		if (RT_UNLIKELY(!rt_event_free(&event)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_event(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_event_already_signaled())) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_TRUE))) goto end;

	ret = RT_OK;
end:
	return ret;
}
