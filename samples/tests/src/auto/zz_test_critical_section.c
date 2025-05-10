#include <rpr.h>

static struct rt_critical_section critical_section;
static struct rt_event event;

enum zz_test_state {
	ZZ_TEST_STATE_MAIN_WAITING,
	ZZ_TEST_STATE_THREAD_DONE,
	ZZ_TEST_STATE_MAIN_DONE,
	ZZ_TEST_STATE_THREAD_DONE2
};

static enum zz_test_state test_state;

static rt_un32 RT_STDCALL zz_test_critical_section_thread_callback(void *parameter)
{
	rt_b recursive = *(rt_b*)parameter;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&critical_section)))
		goto end;

	/* Let the main thread wait for the critical section. */
	if (RT_UNLIKELY(!rt_event_signal(&event))) {
		rt_critical_section_leave(&critical_section);
		goto end;
	}

	/* If the critical section is recursive, we should be able to enter it again. */
	if (recursive) {
		if (RT_UNLIKELY(!rt_critical_section_enter(&critical_section))) {
			rt_critical_section_leave(&critical_section);
			goto end;
		}
		if (RT_UNLIKELY(!rt_critical_section_leave(&critical_section))) {
			rt_critical_section_leave(&critical_section);
			goto end;
		}
	}

	rt_sleep_sleep(10);

	/* The main thread should be waiting for the critical section. */
	if (RT_UNLIKELY(test_state != ZZ_TEST_STATE_MAIN_WAITING)) {
		rt_critical_section_leave(&critical_section);
		goto end;
	}

	test_state = ZZ_TEST_STATE_THREAD_DONE;

	if (RT_UNLIKELY(!rt_critical_section_leave(&critical_section)))
		goto end;

	/* Wait for the main thread to enter the critical section. */
	if (RT_UNLIKELY(!rt_event_wait_for(&event)))
		goto end;

	if (RT_UNLIKELY(!rt_critical_section_enter(&critical_section)))
		goto end;

	if (RT_UNLIKELY(test_state != ZZ_TEST_STATE_MAIN_DONE)) {
		rt_critical_section_leave(&critical_section);
		goto end;
	}

	if (RT_UNLIKELY(!rt_critical_section_leave(&critical_section)))
		goto end;

	rt_sleep_sleep(10);
	test_state = ZZ_TEST_STATE_THREAD_DONE2;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_critical_section_with_thread(rt_b recursive)
{
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_b critical_section_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	test_state = ZZ_TEST_STATE_MAIN_WAITING;

	if (RT_UNLIKELY(!rt_critical_section_create(&critical_section, recursive)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_event_create(&event)))
		goto end;
	event_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_critical_section_thread_callback, &recursive)))
		goto end;
	thread_created = RT_TRUE;

	/* Wait for the thread to enter the critical section. */
	if (RT_UNLIKELY(!rt_event_wait_for(&event)))
		goto end;

	/* Wait for the thread to leave the critical section. */
	if (RT_UNLIKELY(!rt_critical_section_enter(&critical_section)))
		goto end;

	/* The thread should have terminated its critical section. */
	if (RT_UNLIKELY(test_state != ZZ_TEST_STATE_THREAD_DONE)) {
		rt_critical_section_leave(&critical_section);
		goto end;
	}

	/* Let the thread try to re-enter in the critical section. */
	if (RT_UNLIKELY(!rt_event_signal(&event))) {
		rt_critical_section_leave(&critical_section);
		goto end;
	}

	/* Make the thread wait for the critical section. */
	rt_sleep_sleep(10);
	test_state = ZZ_TEST_STATE_MAIN_DONE;

	/* Let the thread enter the critical section. */
	if (RT_UNLIKELY(!rt_critical_section_leave(&critical_section)))
		goto end;

	if (RT_UNLIKELY(!rt_thread_join_and_check(&thread)))
		goto end;

	if (RT_UNLIKELY(test_state != ZZ_TEST_STATE_THREAD_DONE2))
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
	if (critical_section_created) {
		if (RT_UNLIKELY(!rt_critical_section_free(&critical_section)))
			ret = RT_FAILED;
	}

	return ret;
}


static rt_s zz_test_critical_section_without_thread(rt_b recursive)
{
	struct rt_critical_section critical_section;
	rt_b critical_section_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_create(&critical_section, recursive)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_critical_section_enter(&critical_section)))
		goto end;

	if (RT_UNLIKELY(!rt_critical_section_leave(&critical_section)))
		goto end;

	ret = RT_OK;
end:
	if (critical_section_created) {
		if (RT_UNLIKELY(!rt_critical_section_free(&critical_section)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_critical_section(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_critical_section_without_thread(RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_critical_section_without_thread(RT_TRUE))) goto end;
	if (RT_UNLIKELY(!zz_test_critical_section_with_thread(RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_critical_section_with_thread(RT_TRUE))) goto end;

	ret = RT_OK;
end:
	return ret;
}
