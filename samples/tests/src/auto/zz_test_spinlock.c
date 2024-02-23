#include <rpr.h>

static rt_un counter64 = 0;
static rt_un spinlock64 = 0;

static rt_un counter32 = 0;
static rt_un spinlock32 = 0;

static rt_un32 RT_STDCALL zz_test_spinlock_callback64(RT_UNUSED void *parameter)
{
	struct rt_event *event = (struct rt_event*)parameter;
	rt_s ret;

	if (counter64) goto error;

	if (RT_UNLIKELY(!rt_event_signal(event))) goto error;

	RT_SPINLOCK_ACQUIRE(spinlock64);

	if (counter64 != 1) {
		RT_SPINLOCK_RELEASE(spinlock64);
		rt_event_signal(event);
		goto error;
	}

	counter64 = 2;

	if (RT_UNLIKELY(!rt_event_signal(event))) {
		RT_SPINLOCK_RELEASE(spinlock64);
		goto error;
	}

	/* We keep the lock for a while. */
	rt_sleep_sleep(100);

	if (counter64 != 2) {
		RT_SPINLOCK_RELEASE(spinlock64);
		goto error;
	}

	counter64 = 3;

	RT_SPINLOCK_RELEASE(spinlock64);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_spinlock64(void)
{
	struct rt_event event;
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_event_create(&event))) goto error;
	event_created = RT_TRUE;

	RT_SPINLOCK_ACQUIRE(spinlock64);

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_spinlock_callback64, &event))) goto error;
	thread_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_wait_for(&event))) {
		RT_SPINLOCK_RELEASE(spinlock64);
		goto error;
	}

	/* We keep the lock for a while. */
	rt_sleep_sleep(100);

	if (counter64) {
		RT_SPINLOCK_RELEASE(spinlock64);
		goto error;
	}
	counter64 = 1;

	RT_SPINLOCK_RELEASE(spinlock64);

	if (RT_UNLIKELY(!rt_event_wait_for(&event))) goto error;

	RT_SPINLOCK_ACQUIRE(spinlock64);
	if (counter64 != 3) {
		RT_SPINLOCK_RELEASE(spinlock64);
		goto error;
	}
	RT_SPINLOCK_RELEASE(spinlock64);

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread))) goto error;

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

static rt_un32 RT_STDCALL zz_test_spinlock_callback32(RT_UNUSED void *parameter)
{
	struct rt_event *event = (struct rt_event*)parameter;
	rt_s ret;

	if (counter32) goto error;

	if (RT_UNLIKELY(!rt_event_signal(event))) goto error;

	RT_SPINLOCK_ACQUIRE32(spinlock32);

	if (counter32 != 1) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		rt_event_signal(event);
		goto error;
	}

	counter32 = 2;

	if (RT_UNLIKELY(!rt_event_signal(event))) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		goto error;
	}

	/* We keep the lock for a while. */
	rt_sleep_sleep(100);

	if (counter32 != 2) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		goto error;
	}

	counter32 = 3;

	RT_SPINLOCK_RELEASE32(spinlock32);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_spinlock32(void)
{
	struct rt_event event;
	rt_b event_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_event_create(&event))) goto error;
	event_created = RT_TRUE;

	RT_SPINLOCK_ACQUIRE32(spinlock32);

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_spinlock_callback32, &event))) goto error;
	thread_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_wait_for(&event))) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		goto error;
	}

	/* We keep the lock for a while. */
	rt_sleep_sleep(100);

	if (counter32) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		goto error;
	}
	counter32 = 1;

	RT_SPINLOCK_RELEASE32(spinlock32);

	if (RT_UNLIKELY(!rt_event_wait_for(&event))) goto error;

	RT_SPINLOCK_ACQUIRE32(spinlock32);
	if (counter32 != 3) {
		RT_SPINLOCK_RELEASE32(spinlock32);
		goto error;
	}
	RT_SPINLOCK_RELEASE32(spinlock32);

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread))) goto error;

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

rt_s zz_test_spinlock(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_spinlock64())) goto error;
	if (RT_UNLIKELY(!zz_test_spinlock32())) goto error;

	ret = RT_OK;
free:
	return ret;
	
error:
	ret = RT_FAILED;
	goto free;
}
