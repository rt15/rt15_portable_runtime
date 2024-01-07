#include "layer002/rt_thread_local_storage.h"

#include "layer001/rt_os_headers.h"

#ifdef RT_DEFINE_WINDOWS

rt_s rt_thread_local_storage_create(struct rt_thread_local_storage *thread_local_storage)
{
	DWORD tls_index;
	rt_s ret;

	/* TlsAlloc returns TLS_OUT_OF_INDEXES and set last error in case of issues. */
	tls_index = TlsAlloc();
	if (RT_UNLIKELY(tls_index == TLS_OUT_OF_INDEXES))
		goto error;

	thread_local_storage->tls_index = tls_index;

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

	/* TlsSetValue returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!TlsSetValue(thread_local_storage->tls_index, data)))
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
	void *local_data;
	rt_s ret;

	/* TlsGetValue returns zero in case of issue, or if the value was zero. */
	local_data = TlsGetValue(thread_local_storage->tls_index);
	if (RT_UNLIKELY(!local_data && GetLastError() != ERROR_SUCCESS))
		goto error;

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

	/* TlsSetValue returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!TlsFree(thread_local_storage->tls_index)))
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
	pthread_key_t key;
	rt_s ret;

	/* pthread_key_create returns zero in case of success, and an error code in case of issue. */
	error = pthread_key_create(&key, RT_NULL);
	if (error) {
		errno = error;
		goto error;
	}

	thread_local_storage->key = key;

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
	error = pthread_setspecific(thread_local_storage->key, data);
	if (error) {
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
	local_data = pthread_getspecific(thread_local_storage->key);
	*data = local_data;

	return RT_OK;
}

rt_s rt_thread_local_storage_free(struct rt_thread_local_storage *thread_local_storage)
{
	int error;
	rt_s ret;

	/* pthread_key_delete returns zero in case of success, and an error code in case of issue. */
	error = pthread_key_delete(thread_local_storage->key);
	if (error) {
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
