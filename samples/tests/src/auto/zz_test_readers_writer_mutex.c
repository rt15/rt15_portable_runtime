#include <rpr.h>

static rt_un read_state;
static rt_un write_state;

static rt_un32 RT_STDCALL zz_test_read_callback(void *parameter)
{
	struct rt_readers_writer_mutex *readers_writer_mutex = (struct rt_readers_writer_mutex*)parameter;
	rt_s ret;

	read_state = 1;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(readers_writer_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(readers_writer_mutex)))
		goto error;

	read_state = 2;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_un32 RT_STDCALL zz_test_write_callback(void *parameter)
{
	struct rt_readers_writer_mutex *readers_writer_mutex = (struct rt_readers_writer_mutex*)parameter;
	rt_s ret;

	write_state = 1;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_write(readers_writer_mutex)))
		goto error;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_write(readers_writer_mutex)))
		goto error;

	write_state = 2;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_multiple_reads(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto error;
	readers_writer_mutex_created = RT_TRUE;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(&readers_writer_mutex)))
			goto error;
	}
	
	if (readers_writer_mutex.readers_count != 4)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_read_callback, &readers_writer_mutex)))
		goto error;
	thread_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto error;
	
	if (RT_UNLIKELY(read_state != 2))
		goto error;

	if (readers_writer_mutex.readers_count != 4)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(&readers_writer_mutex)))
			goto error;
	}

	if (readers_writer_mutex.readers_count)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (readers_writer_mutex_created) {
		readers_writer_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_read_during_write(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_s ret;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto error;
	readers_writer_mutex_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_write(&readers_writer_mutex)))
		goto error;
	
	if (readers_writer_mutex.readers_count)
		goto error;
	if (!readers_writer_mutex.writer)
		goto error;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_read_callback, &readers_writer_mutex)))
		goto error;
	thread_created = RT_TRUE;

	rt_sleep_sleep(100);

	/* The thread should wait */
	if (RT_UNLIKELY(read_state != 1))
		goto error;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_release_write(&readers_writer_mutex)))
		goto error;

	rt_sleep_sleep(100);

	if (RT_UNLIKELY(read_state != 2))
		goto error;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto error;
	
	if (readers_writer_mutex.readers_count)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (readers_writer_mutex_created) {
		readers_writer_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_write_during_read(void)
{
	struct rt_readers_writer_mutex readers_writer_mutex;
	rt_b readers_writer_mutex_created = RT_FALSE;
	struct rt_thread thread;
	rt_b thread_created = RT_FALSE;
	rt_un i;
	rt_s ret;

	if (RT_UNLIKELY(!rt_readers_writer_mutex_create(&readers_writer_mutex)))
		goto error;
	readers_writer_mutex_created = RT_TRUE;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_acquire_read(&readers_writer_mutex)))
			goto error;
	}
	
	if (readers_writer_mutex.readers_count != 4)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	if (RT_UNLIKELY(!rt_thread_create(&thread, &zz_test_write_callback, &readers_writer_mutex)))
		goto error;
	thread_created = RT_TRUE;

	rt_sleep_sleep(100);

	/* The thread should wait */
	if (RT_UNLIKELY(write_state != 1))
		goto error;

	if (readers_writer_mutex.readers_count != 4)
		goto error;
	if (!readers_writer_mutex.writer)
		goto error;

	for (i = 0; i < 4; i++) {
		if (RT_UNLIKELY(!rt_readers_writer_mutex_release_read(&readers_writer_mutex)))
			goto error;
	}

	rt_sleep_sleep(100);

	if (RT_UNLIKELY(write_state != 2))
		goto error;

	if (RT_UNLIKELY(!rt_thread_join(&thread)))
		goto error;
	
	if (readers_writer_mutex.readers_count)
		goto error;
	if (readers_writer_mutex.writer)
		goto error;

	ret = RT_OK;
free:
	if (thread_created) {
		thread_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_thread_free(&thread) && ret))
			goto error;
	}
	if (readers_writer_mutex_created) {
		readers_writer_mutex_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_readers_writer_mutex_free(&readers_writer_mutex) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_readers_writer_mutex(void)
{	
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_multiple_reads())) goto error;
	if (RT_UNLIKELY(!zz_test_read_during_write())) goto error;
	if (RT_UNLIKELY(!zz_test_write_during_read())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
