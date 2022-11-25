#include <rpr.h>

static struct rt_fast_initialization fast_initialization = RT_FAST_INITIALIZATION_STATIC_INIT;
static struct rt_event event;

static rt_b thread_finished = RT_FALSE;

static rt_un32 RT_STDCALL zz_test_fast_initialization_thread_callback(RT_UNUSED void *parameter)
{
	rt_s ret;

	if (!rt_event_wait_for(&event)) goto error;
	if (rt_fast_initialization_is_required(&fast_initialization)) goto error;
	if (!rt_fast_initialization_is_done(&fast_initialization)) goto error;

	thread_finished = RT_TRUE;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_fast_initialization_with_thread()
{
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret;

	if (!rt_event_create(&event))
		goto error;
	event_created = RT_TRUE;

	if (!rt_thread_create(&thread, &zz_test_fast_initialization_thread_callback, RT_NULL))
		goto error;
	thread_created = RT_TRUE;

	if (rt_fast_initialization_is_required(&fast_initialization)) {

		if (!rt_event_signal(&event)) {
			rt_fast_initialization_notify_done(&fast_initialization);
			goto error;
		}

		rt_sleep_sleep(50);

		if (thread_finished) {
			rt_fast_initialization_notify_done(&fast_initialization);
			goto error;
		}

		rt_fast_initialization_notify_done(&fast_initialization);
	}

	rt_sleep_sleep(50);

	if (!thread_finished)
		goto error;

	if (!rt_fast_initialization_is_done(&fast_initialization))
		goto error;

	if (!rt_thread_join_and_check(&thread))
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (!rt_thread_free(&thread) && ret)
			goto error;
	}
	if (event_created) {
		event_created = RT_FALSE;
		if (!rt_event_free(&event) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_fast_initialization()
{
	rt_s ret;

	if (!zz_test_fast_initialization_with_thread()) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
