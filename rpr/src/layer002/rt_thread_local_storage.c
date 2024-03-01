#include "layer002/rt_thread_local_storage.h"

#include "layer001/rt_os_headers.h"

#ifdef RT_DEFINE_WINDOWS

rt_s rt_thread_local_storage_create(struct rt_thread_local_storage *thread_local_storage)
{
	DWORD index;
	rt_s ret;

	/* TlsAlloc returns TLS_OUT_OF_INDEXES and sets last error in case of issues. */
	index = TlsAlloc();
	if (RT_UNLIKELY(index == TLS_OUT_OF_INDEXES))
		goto error;

	thread_local_storage->index = index;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_local_storage_set(struct rt_thread_local_storage *thread_local_storage, void *data)
{
	rt_s ret;

	/* TlsSetValue returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!TlsSetValue(thread_local_storage->index, data)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_local_storage_get(struct rt_thread_local_storage *thread_local_storage, void **data)
{
	DWORD last_error;
	void *local_data;
	rt_s ret;

	last_error = GetLastError();

	/* TlsGetValue returns zero in case of issue, or if the value was zero. */
	local_data = TlsGetValue(thread_local_storage->index);
	if (RT_UNLIKELY(!local_data && GetLastError() != ERROR_SUCCESS))
		goto error;

	/* As TlsGetValue might update last error, we restore it. */
	SetLastError(last_error);

	*data = local_data;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_local_storage_free(struct rt_thread_local_storage *thread_local_storage)
{
	rt_s ret;

	/* TlsSetValue returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!TlsFree(thread_local_storage->index)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#else

rt_s rt_thread_local_storage_create(struct rt_thread_local_storage *thread_local_storage)
{
	int error;
	pthread_key_t index;
	rt_s ret;

	/* pthread_key_create returns zero in case of success, and an error code in case of issue. */
	error = pthread_key_create(&index, RT_NULL);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto error;
	}

	thread_local_storage->index = index;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_local_storage_set(struct rt_thread_local_storage *thread_local_storage, void *data)
{
	int error;
	rt_s ret;

	/* pthread_setspecific returns zero in case of success, and an error code in case of issue. */
	error = pthread_setspecific(thread_local_storage->index, data);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_local_storage_get(struct rt_thread_local_storage *thread_local_storage, void **data)
{
	void *local_data;

	/* There is no way to check the result of this function. */
	local_data = pthread_getspecific(thread_local_storage->index);
	*data = local_data;

	return RT_OK;
}

rt_s rt_thread_local_storage_free(struct rt_thread_local_storage *thread_local_storage)
{
	int error;
	rt_s ret;

	/* pthread_key_delete returns zero in case of success, and an error code in case of issue. */
	error = pthread_key_delete(thread_local_storage->index);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif
