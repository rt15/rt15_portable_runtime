#include "layer002/rt_named_mutex.h"

#include "layer001/rt_os_headers.h"

rt_s rt_named_mutex_create(struct rt_named_mutex *named_mutex, const rt_char *name)
{
#ifdef RT_DEFINE_LINUX
	int shared_memory_file_descriptor;
	pthread_mutex_t *mutex;
	pthread_mutexattr_t mutex_attributes;
	rt_b create = RT_TRUE;
	rt_b shared_memory_created = RT_FALSE;
	rt_b memory_mapped = RT_FALSE;
	rt_b mutex_attributes_created = RT_FALSE;
	int error;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* The handle cannot be inherited to child processes. */
	/* In case of error, the function returns null and set last error. */
	named_mutex->handle = (rt_h)CreateMutex(RT_NULL, RT_FALSE, name);
	if (RT_UNLIKELY(!named_mutex->handle))
		goto error;
#else
	/* We try to create a new file descriptor first, but use an existing one if it already exists. */
	/* FD_CLOEXEC is set on the file descriptor. */
	/* shm_open returns -1 and set errno in case of error. */
	shared_memory_file_descriptor = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0666);
	if (shared_memory_file_descriptor == -1) {
		if (errno == EEXIST) {
			create = RT_FALSE;
			/* FD_CLOEXEC is set on the file descriptor. */
			/* shm_open returns -1 and set errno in case of error. */
			shared_memory_file_descriptor = shm_open(name, O_RDWR, 0666);
			if (RT_UNLIKELY(shared_memory_file_descriptor == -1))
				goto error;
		} else
			goto error;
	}
	shared_memory_created = RT_TRUE;

	/* Set the size of the shared memory. */
	/* ftruncate returns -1 and set errno in case of error. */
	if (RT_UNLIKELY(ftruncate(shared_memory_file_descriptor, sizeof(pthread_mutex_t)) == -1)) {
		goto error;
	}

	/* On error, mmap returns MAP_FAILED and set errno. */
	mutex = mmap(RT_NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
	if (RT_UNLIKELY(mutex == MAP_FAILED)) {
		goto error;
	}
	memory_mapped = RT_TRUE;

	if (create) {
		/* pthread_mutexattr_init returns an errno in case of error. */
		error = pthread_mutexattr_init(&mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto error;
		}
		mutex_attributes_created = RT_TRUE;

		/* pthread_mutexattr_setpshared returns an errno in case of error. */
		error = pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_SHARED);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto error;
		}

		/* pthread_mutexattr_setrobust returns an errno in case of error. */
		error = pthread_mutexattr_setrobust(&mutex_attributes, PTHREAD_MUTEX_ROBUST);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto error;
		}

		/* pthread_mutex_init returns an errno in case of error. */
		error = pthread_mutex_init(mutex, &mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto error;
		}

	}
	named_mutex->mutex = mutex;

#endif
	ret = RT_OK;
free:
#ifdef RT_DEFINE_LINUX
	if (mutex_attributes_created) {
		mutex_attributes_created = RT_FALSE;
		/* pthread_mutexattr_destroy returns an errno in case of error. */
		error = pthread_mutexattr_destroy(&mutex_attributes);
		if (RT_UNLIKELY(error && ret)) {
			errno = error;
			goto error;
		}
	}
#endif
	return ret;

error:
#ifdef RT_DEFINE_LINUX
	if (memory_mapped) {
		memory_mapped = RT_FALSE;
		munmap(mutex, sizeof(pthread_mutex_t));
	}
	if (shared_memory_created) {
		shared_memory_created = RT_FALSE;
		shm_unlink(name);
	}
#endif
	ret = RT_FAILED;
	goto free;
}

rt_s rt_named_mutex_acquire(struct rt_named_mutex *named_mutex)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(WaitForSingleObject(named_mutex->handle, INFINITE) == WAIT_FAILED))
		goto error;
#else
	/* pthread_mutex_lock returns an errno in case of error. */
	error = pthread_mutex_lock(named_mutex->mutex);
	if (RT_UNLIKELY(error)) {
		if (error == EOWNERDEAD) {
			/* The owner of the robust mutex died. We repair the mutex. We own the mutex, by the way. */
			error = pthread_mutex_consistent(named_mutex->mutex);
			if (RT_UNLIKELY(error)) {
				errno = error;
				goto error;
			}
		} else {
			errno = error;
			goto error;
		}
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_named_mutex_release(struct rt_named_mutex *named_mutex)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!ReleaseMutex(named_mutex->handle)))
		goto error;
#else
	/* pthread_mutex_unlock returns an errno in case of error. */
	error = pthread_mutex_unlock(named_mutex->mutex);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_named_mutex_free(struct rt_named_mutex *named_mutex)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!CloseHandle((HANDLE)named_mutex->handle)))
		ret = RT_FAILED;
#else
	/* munmap returns -1 and set errno in case of error. */
	if (RT_UNLIKELY(munmap(named_mutex->mutex, sizeof(pthread_mutex_t)) == -1))
		ret = RT_FAILED;
#endif

	return ret;
}
