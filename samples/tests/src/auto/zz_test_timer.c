#include <rpr.h>

static rt_b timer_executed;

static rt_s zz_test_timer_callback(RT_UNUSED void *parameter)
{
	timer_executed = RT_TRUE;
	return RT_OK;
}

static rt_s zz_test_timer_do(struct rt_timer_manager *timer_manager)
{
	struct rt_timer timer;
	rt_b timer_created = RT_FALSE;
	rt_un i;
	rt_s ret = RT_FAILED;

	timer_executed = RT_FALSE;

	if (RT_UNLIKELY(!rt_timer_create(&timer, timer_manager, 100, &zz_test_timer_callback, RT_NULL)))
		goto end;
	timer_created = RT_TRUE;

	/* Wait one second checking regularly if the flag is set. */
	for (i = 0; i < 50; i++) {
		rt_sleep_sleep(20);
		if (timer_executed)
			break;
	}

	if (RT_UNLIKELY(!timer_executed))
		goto end;

	ret = RT_OK;
end:
	if (timer_created) {
		if (RT_UNLIKELY(!rt_timer_free(&timer)))
			ret = RT_FAILED;

		/* Check that there is no new execution. */
		timer_executed = RT_FALSE;
		rt_sleep_sleep(200);
		if (RT_UNLIKELY(timer_executed))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_test_timer(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	struct rt_timer_manager timer_manager;
	rt_b timer_manager_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_timer_manager_create(&timer_manager, &runtime_heap.heap)))
		goto end;
	timer_manager_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_timer_do(&timer_manager))) goto end;

	ret = RT_OK;
end:
	if (timer_manager_created) {
		if (RT_UNLIKELY(!rt_timer_manager_free(&timer_manager)))
			ret = RT_FAILED;
	}

	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
