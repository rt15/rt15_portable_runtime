#include <rpr.h>

#define ZZ_TEST_SEMAPHORE_THREADS_COUNT 2

struct zz_test_semaphore_callback_parameter {
	struct rt_semaphore *semaphore;
	struct rt_event *event;
};

static rt_b zz_test_semaphore_main_thread_done;

static rt_un32 RT_STDCALL zz_test_semaphore_callback(void *parameter)
{
	struct zz_test_semaphore_callback_parameter *parameters = (struct zz_test_semaphore_callback_parameter*)parameter;
	struct rt_semaphore *semaphore = parameters->semaphore;
	struct rt_event *event = parameters->event;
	rt_b semaphore_acquired = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_semaphore_acquire(semaphore))) {
		rt_event_signal(event);
		goto error;
	}
	semaphore_acquired = RT_TRUE;

	/* Signal the main thread that it can try to go on. */
	if (RT_UNLIKELY(!rt_event_signal(event)))
		goto error;

	rt_sleep_sleep(100);

	/* Check that the main threads waited. */
	if (zz_test_semaphore_main_thread_done)
		goto error;

	/* Let some time for the other thread to finish. */
	rt_sleep_sleep(100);

	ret = RT_OK;
free:
	if (semaphore_acquired) {
		semaphore_acquired = RT_FALSE;
		if (RT_UNLIKELY(!rt_semaphore_release(semaphore) && ret))
			goto error;
	}

	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

/**
 * @param name If null, then a local anonymous semaphore is tested. Otherwise, a named/shared semaphore is tested.
 */
rt_s zz_test_semaphore_do(const rt_char *name)
{
	struct rt_semaphore semaphore;
	rt_b semaphore_created = RT_FALSE;
	rt_b named_semaphore_created = RT_FALSE;
	struct rt_event event[ZZ_TEST_SEMAPHORE_THREADS_COUNT];
	rt_b event_created[ZZ_TEST_SEMAPHORE_THREADS_COUNT] = { RT_FALSE, RT_FALSE };
	struct zz_test_semaphore_callback_parameter parameters[ZZ_TEST_SEMAPHORE_THREADS_COUNT];
	struct rt_thread threads[ZZ_TEST_SEMAPHORE_THREADS_COUNT];
	rt_b threads_created[ZZ_TEST_SEMAPHORE_THREADS_COUNT] = { RT_FALSE, RT_FALSE };
	rt_b threads_join_failed = RT_FALSE;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_semaphore_create(&semaphore, name, ZZ_TEST_SEMAPHORE_THREADS_COUNT)))
		goto error;
	semaphore_created = RT_TRUE;
	named_semaphore_created = RT_TRUE;

	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_event_create(&event[i])))
			goto error;
		event_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		parameters[i].semaphore = &semaphore;
		parameters[i].event = &event[i];
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_semaphore_callback, &parameters[i])))
			goto error;
		threads_created[i] = RT_TRUE;
	}

	/* Wait for the threads to acquire the semaphore. */
	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_event_wait_for(&event[i])))
			goto error;
	}

	/* We should wait here for the threads to release the semaphore. */
	if (RT_UNLIKELY(!rt_semaphore_acquire(&semaphore)))
		goto error;

	zz_test_semaphore_main_thread_done = RT_TRUE;

	if (RT_UNLIKELY(!rt_semaphore_release(&semaphore)))
		goto error;

	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
			threads_join_failed = RT_TRUE;
	}

	if (RT_UNLIKELY(threads_join_failed))
		goto error;

	ret = RT_OK;
free:

	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		if (threads_created[i]) {
			threads_created[i] = RT_FALSE;
			if (RT_UNLIKELY(!rt_thread_free(&threads[i]) && ret))
				goto error;
		}
	}

	for (i = 0; i < ZZ_TEST_SEMAPHORE_THREADS_COUNT; i++) {
		if (event_created[i]) {
			event_created[i] = RT_FALSE;
			if (RT_UNLIKELY(!rt_event_free(&event[i]) && ret))
				goto error;
		}
	}

	if (semaphore_created) {
		semaphore_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_semaphore_free(&semaphore) && ret))
			goto error;
	}

	if (named_semaphore_created && name) {
		named_semaphore_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_semaphore_destroy(name) && ret))
			goto error;
	}

	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_semaphore(void)
{
	rt_s ret;

	zz_test_semaphore_main_thread_done = RT_FALSE;
	if (RT_UNLIKELY(!zz_test_semaphore_do(RT_NULL))) goto error;
	zz_test_semaphore_main_thread_done = RT_FALSE;
	if (RT_UNLIKELY(!zz_test_semaphore_do(_R("/zz_test_semaphore")))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
