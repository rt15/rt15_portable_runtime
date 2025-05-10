#include <rpr.h>

static rt_un read_state;
static rt_un write_state;

static rt_un32 RT_STDCALL zz_test_read_callback(void *parameter)
{
	struct rt_readers_writer_mutex *readers_writer_mutex = (struct rt_readers_writer_mutex*)parameter;
	rt_s ret = RT_FAILED;

	read_state = 1;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(readers_writer_mutex)))
		goto end;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(readers_writer_mutex)))
		goto end;

	read_state = 2;

	ret = RT_OK;
end:
	return ret;
}

static rt_un32 RT_STDCALL zz_test_write_callback(void *parameter)
{
	struct rt_readers_writer_mutex *readers_writer_mutex = (struct rt_readers_writer_mutex*)parameter;
	rt_s ret = RT_FAILED;

	write_state = 1;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_write(readers_writer_mutex)))
		goto end;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_write(readers_writer_mutex)))
		goto end;

	write_state = 2;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_multiple_reads(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto end;
	readers_writer_mutex_created = RT_TRUE;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(&readers_writer_mutex)))
			goto end;
	}
	
	if (readers_writer_mutex.readers_count != 4)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_read_callback, &readers_writer_mutex)))
		goto end;
	thread_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto end;
	
	if (RT_UNLIKELY(read_state != 2))
		goto end;

	if (readers_writer_mutex.readers_count != 4)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(&readers_writer_mutex)))
			goto end;
	}

	if (readers_writer_mutex.readers_count)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (readers_writer_mutex_created) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_read_during_write(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto end;
	readers_writer_mutex_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_write(&readers_writer_mutex)))
		goto end;
	
	if (readers_writer_mutex.readers_count)
		goto end;
	if (!readers_writer_mutex.writer)
		goto end;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_read_callback, &readers_writer_mutex)))
		goto end;
	thread_created = RT_TRUE;

	rt_sleep_sleep(100);

	/* The thread should wait */
	if (RT_UNLIKELY(read_state != 1))
		goto end;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_write(&readers_writer_mutex)))
		goto end;

	rt_sleep_sleep(100);

	if (RT_UNLIKELY(read_state != 2))
		goto end;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto end;
	
	if (readers_writer_mutex.readers_count)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (readers_writer_mutex_created) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_write_during_read(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_un i;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto end;
	readers_writer_mutex_created = RT_TRUE;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(&readers_writer_mutex)))
			goto end;
	}
	
	if (readers_writer_mutex.readers_count != 4)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_write_callback, &readers_writer_mutex)))
		goto end;
	thread_created = RT_TRUE;

	rt_sleep_sleep(100);

	/* The thread should wait */
	if (RT_UNLIKELY(write_state != 1))
		goto end;

	if (readers_writer_mutex.readers_count != 4)
		goto end;
	if (!readers_writer_mutex.writer)
		goto end;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(&readers_writer_mutex)))
			goto end;
	}

	rt_sleep_sleep(100);

	if (RT_UNLIKELY(write_state != 2))
		goto end;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto end;
	
	if (readers_writer_mutex.readers_count)
		goto end;
	if (readers_writer_mutex.writer)
		goto end;

	ret = RT_OK;
end:
	if (thread_created) {
		if (RT_UNLIKELY(!rt_thread_free(&thread)))
			ret = RT_FAILED;
	}
	if (readers_writer_mutex_created) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_test_readers_writer_mutex(void)
{	
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_test_multiple_reads())) goto end;
	if (RT_UNLIKELY(!zz_test_read_during_write())) goto end;
	if (RT_UNLIKELY(!zz_test_write_during_read())) goto end;

	ret = RT_OK;
end:
	return ret;
}
