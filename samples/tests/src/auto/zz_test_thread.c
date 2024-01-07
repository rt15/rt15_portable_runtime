#include <rpr.h>

struct zz_test_thread_parameter {
	rt_un value;
	struct rt_thread_local_storage *thread_local_storage;
};

static rt_un32 RT_STDCALL zz_test_thread_callback(void *parameter)
{
	struct zz_test_thread_parameter *local_parameter = (struct zz_test_thread_parameter*)parameter;
	void* data;
	rt_un32 exit_code;

	rt_sleep_sleep(500);
	if (local_parameter->value != 12)
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_get(local_parameter->thread_local_storage, &data)))
		goto error;

	if (data)
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_set(local_parameter->thread_local_storage, (void*)2)))
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_get(local_parameter->thread_local_storage, &data)))
		goto error;

	if ((rt_un)data != 2)
		goto error;

	exit_code = 42;
free:
	return exit_code;

error:
	exit_code = 1;
	goto free;
}

rt_s zz_test_thread()
{
	struct rt_chrono chrono;
	rt_un duration;
	struct rt_thread_local_storage thread_local_storage;
	rt_b thread_local_storage_created = RT_FALSE;
	void *data;
	struct zz_test_thread_parameter parameter;
	rt_b thread_created = RT_FALSE;
	struct rt_thread thread;
	rt_un32 exit_code;
	rt_s ret;

	if (RT_UNLIKELY(!rt_chrono_create(&chrono)))
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_create(&thread_local_storage)))
		goto error;
	thread_local_storage_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_local_storage_get(&thread_local_storage, &data)))
		goto error;

	if (data)
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_set(&thread_local_storage, (void*)1)))
		goto error;

	parameter.value = 12;
	parameter.thread_local_storage = &thread_local_storage;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_thread_callback, &parameter)))
		goto error;
	thread_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto error;

	if (RT_UNLIKELY(!rt_chrono_get_duration(&chrono, &duration)))
		goto error;

	if (RT_UNLIKELY(duration < 250000 || duration > 1000000))
		goto error;

	if (RT_UNLIKELY(!rt_thread_get_exit_code(&thread, &exit_code)))
		goto error;

	if (RT_UNLIKELY(exit_code != 42))
		goto error;

	if (RT_UNLIKELY(!rt_thread_local_storage_get(&thread_local_storage, &data)))
		goto error;

	if ((rt_un)data != 1)
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (thread_local_storage_created) {
		thread_local_storage_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_local_storage_free(&thread_local_storage) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
