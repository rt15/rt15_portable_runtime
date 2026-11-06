#include <rpr.h>

enum zz_thread_status {
	ZZ_THREAD_STATUS_INIT,
	ZZ_THREAD_STATUS_SIGNAL_1,
	ZZ_THREAD_STATUS_SIGNAL_2
};

static enum zz_thread_status thread_status;

struct zz_test_event_thread_parameter {
	struct rt_event *event;
	rt_b use_timeout;
	rt_un32 timeout_milliseconds;
};

static rt_un32 RT_STDCALL zz_test_event_thread_callback(void *parameter)
{
	struct zz_test_event_thread_parameter *test_event_thread_parameter = (struct zz_test_event_thread_parameter*)parameter;
	struct rt_event *event = test_event_thread_parameter->event;
	rt_b signaled;
	rt_un32 result = RT_FAILED;

	if (test_event_thread_parameter->use_timeout) {
		if (RT_UNLIKELY(!rt_event_wait_with_timeout(event, test_event_thread_parameter->timeout_milliseconds, &signaled)))
			goto end;
		if (RT_UNLIKELY(!signaled))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_event_wait(event)))
			goto end;
	}

	thread_status = ZZ_THREAD_STATUS_SIGNAL_1;

	if (test_event_thread_parameter->use_timeout) {
		if (RT_UNLIKELY(!rt_event_wait_with_timeout(event, test_event_thread_parameter->timeout_milliseconds, &signaled)))
			goto end;
		if (RT_UNLIKELY(!signaled))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_event_wait(event)))
			goto end;
	}

	thread_status = ZZ_THREAD_STATUS_SIGNAL_2;

	result = RT_OK;
end:
	return result;
}

static rt_s zz_test_event_already_signaled(rt_b use_timeout)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	struct zz_test_event_thread_parameter test_event_thread_parameter;
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

	test_event_thread_parameter.event = &event;
	test_event_thread_parameter.use_timeout = use_timeout;
	test_event_thread_parameter.timeout_milliseconds = 1000;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &test_event_thread_parameter)))
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

static rt_s zz_test_event_not_signaled_yet(rt_b reset, rt_b use_timeout)
{
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	struct rt_event event;
	struct zz_test_event_thread_parameter test_event_thread_parameter;
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

	test_event_thread_parameter.event = &event;
	test_event_thread_parameter.use_timeout = use_timeout;
	test_event_thread_parameter.timeout_milliseconds = 1000;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_event_thread_callback, &test_event_thread_parameter)))
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

static rt_s zz_test_event_timeout(void)
{
	struct rt_event event;
	rt_b event_created = RT_FALSE;
	rt_b signaled;
	struct rt_chrono chrono;
	rt_un duration;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto end;
	event_created = RT_TRUE;

	/* Not signaled yet, zero timeout. */
	if (RT_UNLIKELY(!rt_event_wait_with_timeout(&event, 0, &signaled))) goto end;
	if (RT_UNLIKELY(signaled)) goto end;

	/* Not signaled yet, with timeout. */
	if (RT_UNLIKELY(!rt_chrono_create(&chrono))) goto end;
	if (RT_UNLIKELY(!rt_event_wait_with_timeout(&event, 100, &signaled))) goto end;
	if (RT_UNLIKELY(!rt_chrono_get_duration(&chrono, &duration))) goto end;
	if (RT_UNLIKELY(signaled)) goto end;
	/* Duration is in microseconds while timeout is in milliseconds. */
	if (RT_UNLIKELY(duration < 50000)) goto end;

	/* Signaling. */
	if (RT_UNLIKELY(!rt_event_signal(&event))) goto end;

	/* Signaled already, zero timeout. */
	if (RT_UNLIKELY(!rt_event_wait_with_timeout(&event, 0, &signaled))) goto end;
	if (RT_UNLIKELY(!signaled)) goto end;

	/* Signal should have been consumed. */
	if (RT_UNLIKELY(!rt_event_wait_with_timeout(&event, 0, &signaled))) goto end;
	if (RT_UNLIKELY(signaled)) goto end;

	/* Reset discards the signal. */
	if (RT_UNLIKELY(!rt_event_signal(&event))) goto end;
	if (RT_UNLIKELY(!rt_event_reset(&event))) goto end;
	if (RT_UNLIKELY(!rt_event_wait_with_timeout(&event, 0, &signaled))) goto end;
	if (RT_UNLIKELY(signaled)) goto end;

	ret = RT_OK;
end:
	if (event_created) {
		if (RT_UNLIKELY(!rt_event_free(&event)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_test_event(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_event_already_signaled(RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_FALSE, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_TRUE, RT_FALSE))) goto end;

	if (RT_UNLIKELY(!zz_test_event_already_signaled(RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_FALSE, RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_event_not_signaled_yet(RT_TRUE, RT_TRUE))) goto end;

	if (RT_UNLIKELY(!zz_test_event_timeout())) goto end;

	ret = RT_OK;
end:
	return ret;
}
