#include <rpr.h>

#define ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT 10
#define ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT 5
#define ZZ_TEST_THREAD_SAFE_RING_VALUES_COUNT 100000

static struct rt_thread_safe_ring thread_safe_ring;
static rt_n32 incremented_value = -1;
static rt_n32 incremented_values[ZZ_TEST_THREAD_SAFE_RING_VALUES_COUNT];

static rt_un32 RT_STDCALL zz_test_thread_safe_ring_producer_callback(RT_UNUSED void *parameter)
{
	rt_n32 value;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		value = RT_ATOMIC_INCREMENT_AND_GET32(incremented_value);
		if (value >= ZZ_TEST_THREAD_SAFE_RING_VALUES_COUNT)
			break;

		if (RT_UNLIKELY(!rt_thread_safe_ring_push(&thread_safe_ring, &value)))
			goto end;
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret))
		rt_thread_safe_ring_close(&thread_safe_ring);

	return ret;
}

static rt_un32 RT_STDCALL zz_test_thread_safe_ring_consumer_callback(RT_UNUSED void *parameter)
{
	rt_n32 value;
	rt_b closed;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		if (RT_UNLIKELY(!rt_thread_safe_ring_pop(&thread_safe_ring, &value, &closed)))
			goto end;

		if (closed)
			break;

		if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(incremented_values[value])))
			goto end;
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret))
		rt_thread_safe_ring_close(&thread_safe_ring);

	return ret;
}

/**
 * Join <tt>thread</tt> then check its exit code.<br>
 * <tt>joined</tt> is set as soon as the join succeeds so that the thread is not joined twice.
 */
static rt_s zz_test_thread_safe_ring_join(struct rt_thread *thread, rt_b *joined)
{
	rt_un32 exit_code;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_thread_join(thread)))
		goto end;
	*joined = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_get_exit_code(thread, &exit_code)))
		goto end;

	if (RT_UNLIKELY(!exit_code))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_thread_safe_ring_do(struct rt_heap *heap)
{
	struct rt_thread producer_threads[ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT];
	rt_b producer_threads_created[ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT];
	rt_b producer_threads_joined[ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT];
	struct rt_thread consumer_threads[ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT];
	rt_b consumer_threads_created[ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT];
	rt_b consumer_threads_joined[ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT];
	rt_b thread_safe_ring_created = RT_FALSE;
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_VALUES_COUNT; i++)
		incremented_values[i] = 0;

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT; i++) {
		producer_threads_created[i] = RT_FALSE;
		producer_threads_joined[i] = RT_FALSE;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT; i++) {
		consumer_threads_created[i] = RT_FALSE;
		consumer_threads_joined[i] = RT_FALSE;
	}

	if (RT_UNLIKELY(!rt_thread_safe_ring_create(&thread_safe_ring, 3, sizeof(rt_n32), heap)))
		goto end;
	thread_safe_ring_created = RT_TRUE;

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&producer_threads[i], &zz_test_thread_safe_ring_producer_callback, RT_NULL)))
			goto end;
		producer_threads_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&consumer_threads[i], &zz_test_thread_safe_ring_consumer_callback, RT_NULL)))
			goto end;
		consumer_threads_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!zz_test_thread_safe_ring_join(&producer_threads[i], &producer_threads_joined[i])))
			goto end;
	}

	if (RT_UNLIKELY(!rt_thread_safe_ring_close(&thread_safe_ring)))
		goto end;

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!zz_test_thread_safe_ring_join(&consumer_threads[i], &consumer_threads_joined[i])))
			goto end;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_VALUES_COUNT; i++) {
		if (RT_UNLIKELY(!incremented_values[i]))
			goto end;
	}

	ret = RT_OK;
end:
	if (RT_UNLIKELY(!ret) && thread_safe_ring_created)
		rt_thread_safe_ring_close(&thread_safe_ring);

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT; i++) {
		if (producer_threads_created[i] && !producer_threads_joined[i])
			rt_thread_join(&producer_threads[i]);
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT; i++) {
		if (consumer_threads_created[i] && !consumer_threads_joined[i])
			rt_thread_join(&consumer_threads[i]);
	}

	if (thread_safe_ring_created) {
		if (RT_UNLIKELY(!rt_thread_safe_ring_free(&thread_safe_ring)))
			ret = RT_FAILED;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_CONSUMER_THREADS_COUNT; i++) {
		if (consumer_threads_created[i])
			if (RT_UNLIKELY(!rt_thread_free(&consumer_threads[i])))
				ret = RT_FAILED;
	}

	for (i = 0; i < ZZ_TEST_THREAD_SAFE_RING_PRODUCER_THREADS_COUNT; i++) {
		if (producer_threads_created[i])
			if (RT_UNLIKELY(!rt_thread_free(&producer_threads[i])))
				ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_thread_safe_ring(void)
{
	struct rt_runtime_heap runtime_heap;
	rt_b runtime_heap_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_runtime_heap_create(&runtime_heap)))
		goto end;
	runtime_heap_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_test_thread_safe_ring_do(&runtime_heap.heap))) goto end;

	ret = RT_OK;
end:
	if (runtime_heap_created) {
		if (RT_UNLIKELY(!runtime_heap.heap.close(&runtime_heap.heap)))
			ret = RT_FAILED;
	}

	return ret;
}
