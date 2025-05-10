#include <rpr.h>

static struct rt_fast_initialization fast_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;
static struct rt_event event;

static rt_b thread_finished = RT_FALSE;

static rt_un32 RT_STDCALL zz_test_fast_initialization_thread_callback(RT_UNUSED void *parameter)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_event_wait_for(&event))) goto end;
	if (RT_UNLIKELY(rt_fast_initialization_is_required(&fast_initialization))) goto end;
	if (RT_UNLIKELY(!rt_fast_initialization_is_done(&fast_initialization))) goto end;

	thread_finished = RT_TRUE;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_fast_initialization_with_thread(void)
{
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto end;
	event_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_fast_initialization_thread_callback, RT_NULL)))
		goto end;
	thread_created = RT_TRUE;

	if (rt_fast_initialization_is_required(&fast_initialization)) {

		if (RT_UNLIKELY(!rt_event_signal(&event))) {
			rt_fast_initialization_notify_done(&fast_initialization);
			goto end;
		}

		rt_sleep_sleep(50);

		if (RT_UNLIKELY(thread_finished)) {
			rt_fast_initialization_notify_done(&fast_initialization);
			goto end;
		}

		rt_fast_initialization_notify_done(&fast_initialization);
	}

	rt_sleep_sleep(50);

	if (RT_UNLIKELY(!thread_finished))
		goto end;

	if (RT_UNLIKELY(!rt_fast_initialization_is_done(&fast_initialization)))
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

rt_s zz_test_fast_initialization(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_fast_initialization_with_thread())) goto end;

	ret = RT_OK;
end:
	return ret;
}
