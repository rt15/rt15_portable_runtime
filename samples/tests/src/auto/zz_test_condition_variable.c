#include <rpr.h>

#define ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT 10

struct zz_test_condition_variable_parameter {
	struct rt_critical_section critical_section;
	struct rt_condition_variable condition_variable;
	rt_un queue_size;
	rt_un consumed;
};

static rt_un32 RT_STDCALL zz_test_condition_variable_callback(void *parameter)
{
	struct zz_test_condition_variable_parameter *condition_variable_parameter = (struct zz_test_condition_variable_parameter*)parameter;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_critical_section_enter(&condition_variable_parameter->critical_section)))
		goto end;

	while (!condition_variable_parameter->queue_size) {
		if (RT_UNLIKELY(!rt_condition_variable_wait(&condition_variable_parameter->condition_variable))) {
			rt_critical_section_leave(&condition_variable_parameter->critical_section);
			goto end;
		}
	}

	condition_variable_parameter->queue_size--;
	condition_variable_parameter->consumed++;

	if (RT_UNLIKELY(!rt_critical_section_leave(&condition_variable_parameter->critical_section)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_condition_variable_do(rt_b broadcast)
{
	struct zz_test_condition_variable_parameter parameter;
	rt_b critical_section_created = RT_FALSE;
	rt_b condition_variable_created = RT_FALSE;
	struct rt_thread threads[ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT];
	rt_b threads_created[ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT];
	rt_un i;
	rt_s ret = RT_FAILED;

	for (i = 0; i < ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT; i++) {
		threads_created[i] = RT_FALSE;
	}

	parameter.queue_size = 0;
	parameter.consumed = 0;

	if (RT_UNLIKELY(!rt_critical_section_create(&parameter.critical_section, RT_FALSE)))
		goto end;
	critical_section_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_condition_variable_create(&parameter.condition_variable, &parameter.critical_section)))
		goto end;
	condition_variable_created = RT_TRUE;

	for (i = 0; i < ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT; i++) {
		if (RT_UNLIKELY(!rt_thread_create(&threads[i], &zz_test_condition_variable_callback, &parameter)))
			goto end;
		threads_created[i] = RT_TRUE;
	}

	/* Wait for the threads to be blocked waiting for a signal. */
	rt_sleep_sleep(100);

	if (broadcast) {
		if (RT_UNLIKELY(!rt_critical_section_enter(&parameter.critical_section)))
			goto end;

		parameter.queue_size = ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT;

		if (RT_UNLIKELY(!rt_condition_variable_broadcast(&parameter.condition_variable))) {
			rt_critical_section_leave(&parameter.critical_section);
			goto end;
		}

		if (RT_UNLIKELY(!rt_critical_section_leave(&parameter.critical_section)))
			goto end;
	} else {
		for (i = 0; i < ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT; i++) {
			if (RT_UNLIKELY(!rt_critical_section_enter(&parameter.critical_section)))
				goto end;

			parameter.queue_size++;

			if (RT_UNLIKELY(!rt_condition_variable_signal(&parameter.condition_variable))) {
				rt_critical_section_leave(&parameter.critical_section);
				goto end;
			}

			if (RT_UNLIKELY(!rt_critical_section_leave(&parameter.critical_section)))
				goto end;
		}
	}

	ret = RT_OK;
end:
	for (i = 0; i < ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT; i++) {
		if (threads_created[i]) {
			if (RT_UNLIKELY(!rt_thread_join_and_check(&threads[i])))
				ret = RT_FAILED;
			if (RT_UNLIKELY(!rt_thread_free(&threads[i])))
				ret = RT_FAILED;
		}
	}

	if (RT_UNLIKELY(parameter.queue_size))
		ret = RT_FAILED;

	if (RT_UNLIKELY(parameter.consumed != ZZ_TEST_CONDITION_VARIABLE_THREADS_COUNT))
		ret = RT_FAILED;

	if (condition_variable_created) {
		if (RT_UNLIKELY(!rt_condition_variable_free(&parameter.condition_variable)))
			ret = RT_FAILED;
	}

	if (critical_section_created) {
		if (RT_UNLIKELY(!rt_critical_section_free(&parameter.critical_section)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_condition_variable(void)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_condition_variable_do(RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_test_condition_variable_do(RT_TRUE))) goto end;

	ret = RT_OK;
end:
	return ret;
}
