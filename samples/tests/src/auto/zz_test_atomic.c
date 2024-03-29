#include <rpr.h>

#define ZZ_TEST_ATOMIC_THREADS_COUNT 8

/* For heavy testing, use 800000. */
#define ZZ_TEST_ATOMIC_LOOP_SIZE 20000

static rt_un counter = 0;
static rt_un lock = 0;

static rt_un32 RT_STDCALL zz_test_atomic_callback(RT_UNUSED void *parameter)
{
	rt_un counter_value;
	rt_s ret;

	while (RT_TRUE) {
		if (RT_ATOMIC_TRY_TO_ACQUIRE(lock)) {

			counter_value = counter;
			if (counter_value > ZZ_TEST_ATOMIC_LOOP_SIZE) {
				RT_ATOMIC_RELEASE(lock);
				break;
			}
			rt_sleep_yield();

			if (RT_UNLIKELY(counter_value != counter)) {
				RT_ATOMIC_RELEASE(lock);
				goto error;
			}
			counter = counter_value + 1;
			rt_sleep_yield();

			if (RT_UNLIKELY(counter != counter_value + 1)) {
				RT_ATOMIC_RELEASE(lock);
				goto error;
			}

			RT_ATOMIC_RELEASE(lock);
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}


static rt_s zz_test_atomic_with_threads(void)
{
	rt_b threads_created[ZZ_TEST_ATOMIC_THREADS_COUNT];
	struct rt_thread threads[ZZ_TEST_ATOMIC_THREADS_COUNT];
	rt_un i;
	rt_s ret;

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		threads_created[i] = RT_FALSE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_atomic_callback, RT_NULL)))
			goto error;
		threads_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
			goto error;
	}

	ret = RT_OK;
free:
	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (threads_created[i]) {
			threads_created[i] = RT_FALSE;
			if (RT_UNLIKELY(!rt_thread_free(&threads[i]) && ret))
				goto error;
		}
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_atomic_basics(void)
{
	rt_n var;
	rt_n32 var32;
	rt_s ret;

	var = 12;
	if (RT_UNLIKELY(RT_ATOMIC_LOAD(var) != 12)) goto error;
	RT_ATOMIC_STORE(var, 13);
	if (RT_UNLIKELY(var != 13)) goto error;

	var32 = 12;
	if (RT_UNLIKELY(RT_ATOMIC_LOAD32(var32) != 12)) goto error;
	RT_ATOMIC_STORE32(var32, 13);
	if (RT_UNLIKELY(var32 != 13)) goto error;

	var = 0;
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	RT_ATOMIC_RELEASE(var);
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto error;
	RT_ATOMIC_RELEASE(var);

	var32 = 0;
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	RT_ATOMIC_RELEASE32(var32);
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto error;
	RT_ATOMIC_RELEASE32(var32);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_atomic(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_atomic_basics())) goto error;
	if (RT_UNLIKELY(!zz_test_atomic_with_threads())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
