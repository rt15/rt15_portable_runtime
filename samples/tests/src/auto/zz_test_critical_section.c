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
	rt_s ret;

	if (!rt_critical_section_enter(&critical_section))
		goto error;

	/* Let the main thread wait for the critical section. */
	if (!rt_event_signal(&event)) {
		rt_critical_section_leave(&critical_section);
		goto error;
	}

	/* If the critical section is recursive, we should be able to enter it again. */
	if (recursive) {
		if (!rt_critical_section_enter(&critical_section)) {
			rt_critical_section_leave(&critical_section);
			goto error;
		}
		if (!rt_critical_section_leave(&critical_section)) {
			rt_critical_section_leave(&critical_section);
			goto error;
		}
	}

	rt_sleep_sleep(10);

	/* The main thread should be waiting for the critical section. */
	if (test_state != ZZ_TEST_STATE_MAIN_WAITING) {
		rt_critical_section_leave(&critical_section);
		goto error;
	}

	test_state = ZZ_TEST_STATE_THREAD_DONE;

	if (!rt_critical_section_leave(&critical_section))
		goto error;

	/* Wait for the main thread to enter the critical section. */
	if (!rt_event_wait_for(&event))
		goto error;

	if (!rt_critical_section_enter(&critical_section))
		goto error;

	if (test_state != ZZ_TEST_STATE_MAIN_DONE) {
		rt_critical_section_leave(&critical_section);
		goto error;
	}

	if (!rt_critical_section_leave(&critical_section))
		goto error;

	rt_sleep_sleep(10);
	test_state = ZZ_TEST_STATE_THREAD_DONE2;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_critical_section_with_thread(rt_b recursive)
{
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_b critical_section_created = RT_FALSE;
	rt_b event_created = RT_FALSE;
	rt_s ret;

	test_state = ZZ_TEST_STATE_MAIN_WAITING;

	if (!rt_critical_section_create(&critical_section, recursive))
		goto error;
	critical_section_created = RT_TRUE;

	if (!rt_event_create(&event))
		goto error;
	event_created = RT_TRUE;

	if (!rt_thread_create(&thread, &zz_test_critical_section_thread_callback, &recursive))
		goto error;
	thread_created = RT_TRUE;

	/* Wait for the thread to enter the critical section. */
	if (!rt_event_wait_for(&event))
		goto error;

	/* Wait for the thread to leave the critical section. */
	if (!rt_critical_section_enter(&critical_section))
		goto error;

	/* The thread should have terminated its critical section. */
	if (test_state != ZZ_TEST_STATE_THREAD_DONE) {
		rt_critical_section_leave(&critical_section);
		goto error;
	}

	/* Let the thread try to re-enter in the critical section. */
	if (!rt_event_signal(&event)) {
		rt_critical_section_leave(&critical_section);
		goto error;
	}

	/* Make the thread wait for the critical section. */
	rt_sleep_sleep(10);
	test_state = ZZ_TEST_STATE_MAIN_DONE;

	/* Let the thread enter the critical section. */
	if (!rt_critical_section_leave(&critical_section))
		goto error;

	if (!rt_thread_join_and_check(&thread))
		goto error;

	if (test_state != ZZ_TEST_STATE_THREAD_DONE2)
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
	if (critical_section_created) {
		critical_section_created = RT_FALSE;
		if (!rt_critical_section_free(&critical_section) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}


static rt_s zz_test_critical_section_without_thread(rt_b recursive)
{
	struct rt_critical_section critical_section;
	rt_b critical_section_created = RT_FALSE;
	rt_s ret;

	if (!rt_critical_section_create(&critical_section, recursive))
		goto error;
	critical_section_created = RT_TRUE;

	if (!rt_critical_section_enter(&critical_section))
		goto error;

	if (!rt_critical_section_leave(&critical_section))
		goto error;

	ret = RT_OK;
free:
	if (critical_section_created) {
		critical_section_created = RT_FALSE;
		if (!rt_critical_section_free(&critical_section) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_critical_section()
{
	rt_s ret;

	if (!zz_test_critical_section_without_thread(RT_FALSE)) goto error;
	if (!zz_test_critical_section_without_thread(RT_TRUE)) goto error;
	if (!zz_test_critical_section_with_thread(RT_FALSE)) goto error;
	if (!zz_test_critical_section_with_thread(RT_TRUE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
