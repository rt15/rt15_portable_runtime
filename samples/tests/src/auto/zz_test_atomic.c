#include <rpr.h>

#define ZZ_TEST_ATOMIC_THREADS_COUNT 8

/* For heavy testing, use 800000. */
#define ZZ_TEST_ATOMIC_LOOP_SIZE 20000

#define ZZ_TEST_ATOMIC_MAX_INCREMENT 1000000

static rt_un counter = 0;
static rt_un lock = 0;

static rt_n incremented_value = -1;
static rt_n32 incremented_values[ZZ_TEST_ATOMIC_MAX_INCREMENT];

static rt_un32 RT_STDCALL zz_test_atomic_try_to_acquire_callback(RT_UNUSED void *parameter)
{
	rt_un counter_value;
	rt_s ret = RT_FAILED;

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
				goto end;
			}
			counter = counter_value + 1;
			rt_sleep_yield();

			if (RT_UNLIKELY(counter != counter_value + 1)) {
				RT_ATOMIC_RELEASE(lock);
				goto end;
			}

			RT_ATOMIC_RELEASE(lock);
		}
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_un32 RT_STDCALL zz_test_atomic_increment_and_get_callback(RT_UNUSED void *parameter)
{
	rt_n counter_value;
	rt_s ret = RT_FAILED;

	while (RT_TRUE) {
		counter_value = RT_ATOMIC_INCREMENT_AND_GET(incremented_value);
		if (counter_value >= ZZ_TEST_ATOMIC_MAX_INCREMENT)
			break;
		if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(incremented_values[counter_value])))
			goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_atomic_try_to_acquire(void)
{
	rt_b threads_created[ZZ_TEST_ATOMIC_THREADS_COUNT];
	struct rt_thread threads[ZZ_TEST_ATOMIC_THREADS_COUNT];
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		threads_created[i] = RT_FALSE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_atomic_try_to_acquire_callback, RT_NULL)))
			goto end;
		threads_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
			goto end;
	}

	ret = RT_OK;
end:
	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (threads_created[i]) {
			if (RT_UNLIKELY(!rt_thread_free(&threads[i])))
				ret = RT_FAILED;
		}
	}

	return ret;
}

static rt_s zz_test_atomic_increment_and_get(void)
{
	rt_b threads_created[ZZ_TEST_ATOMIC_THREADS_COUNT];
	struct rt_thread threads[ZZ_TEST_ATOMIC_THREADS_COUNT];
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < ZZ_TEST_ATOMIC_MAX_INCREMENT; i++)
		incremented_values[i] = 0;

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		threads_created[i] = RT_FALSE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_atomic_increment_and_get_callback, RT_NULL)))
			goto end;
		threads_created[i] = RT_TRUE;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
			goto end;
	}

	for (i = 0; i < ZZ_TEST_ATOMIC_MAX_INCREMENT; i++) {
		if (RT_UNLIKELY(!incremented_values[i]))
			goto end;
	}

	ret = RT_OK;
end:
	for (i = 0; i < ZZ_TEST_ATOMIC_THREADS_COUNT; i++) {
		if (threads_created[i]) {
			if (RT_UNLIKELY(!rt_thread_free(&threads[i])))
				ret = RT_FAILED;
		}
	}

	return ret;
}

static rt_s zz_test_atomic_basics(void)
{
	rt_n var;
	rt_n32 var32;
	rt_s ret = RT_FAILED;

	var = 12;
	if (RT_UNLIKELY(RT_ATOMIC_LOAD(var) != 12)) goto end;
	RT_ATOMIC_STORE(var, 13);
	if (RT_UNLIKELY(var != 13)) goto end;

	var32 = 12;
	if (RT_UNLIKELY(RT_ATOMIC_LOAD32(var32) != 12)) goto end;
	RT_ATOMIC_STORE32(var32, 13);
	if (RT_UNLIKELY(var32 != 13)) goto end;

	var = 0;
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	RT_ATOMIC_RELEASE(var);
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE(var))) goto end;
	RT_ATOMIC_RELEASE(var);

	var32 = 0;
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	RT_ATOMIC_RELEASE32(var32);
	if (RT_UNLIKELY(!RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_TRY_TO_ACQUIRE32(var32))) goto end;
	RT_ATOMIC_RELEASE32(var32);

	var = 0;
	if (RT_UNLIKELY(RT_ATOMIC_INCREMENT_AND_GET(var) != 1)) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_INCREMENT_AND_GET(var) != 2)) goto end;

	var32 = 0;
	if (RT_UNLIKELY(RT_ATOMIC_INCREMENT_AND_GET32(var32) != 1)) goto end;
	if (RT_UNLIKELY(RT_ATOMIC_INCREMENT_AND_GET32(var32) != 2)) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_test_atomic(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_atomic_basics())) goto end;
	if (RT_UNLIKELY(!zz_test_atomic_try_to_acquire())) goto end;
	if (RT_UNLIKELY(!zz_test_atomic_increment_and_get())) goto end;

	ret = RT_OK;
end:
	return ret;
}
