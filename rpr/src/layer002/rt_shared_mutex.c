#include "layer002/rt_shared_mutex.h"

#include "layer001/rt_os_headers.h"

rt_s rt_shared_mutex_create(struct rt_shared_mutex *shared_mutex, const rt_char *name)
{
#ifdef RT_DEFINE_LINUX
	int shared_memory_file_descriptor;
	rt_b shared_memory_file_descriptor_created = RT_FALSE;
	pthread_mutex_t *mutex;
	pthread_mutexattr_t mutex_attributes;
	rt_b create = RT_TRUE;
	rt_b shared_memory_created = RT_FALSE;
	rt_b memory_mapped = RT_FALSE;
	rt_b mutex_attributes_created = RT_FALSE;
	int error;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* The handle cannot be inherited to child processes. */
	/* In case of error, the function returns null and sets last error. */
	shared_mutex->handle = (rt_h)CreateMutex(RT_NULL, RT_FALSE, name);
	if (RT_UNLIKELY(!shared_mutex->handle))
		goto end;
#else
	/* We try to create a new file descriptor first, but use an existing one if it already exists. */
	/* FD_CLOEXEC is set on the file descriptor. */
	/* shm_open returns -1 and sets errno in case of error. */
	shared_memory_file_descriptor = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0666);
	if (shared_memory_file_descriptor == -1) {
		if (errno == EEXIST) {
			create = RT_FALSE;
			/* FD_CLOEXEC is set on the file descriptor. */
			/* shm_open returns -1 and sets errno in case of error. */
			shared_memory_file_descriptor = shm_open(name, O_RDWR, 0666);
			if (RT_UNLIKELY(shared_memory_file_descriptor == -1))
				goto end;
		} else
			goto end;
	}
	shared_memory_created = RT_TRUE;
	shared_memory_file_descriptor_created = RT_TRUE;

	/* Set the size of the shared memory. */
	/* ftruncate returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(ftruncate(shared_memory_file_descriptor, sizeof(pthread_mutex_t)) == -1))
		goto end;

	/* On error, mmap returns MAP_FAILED and sets errno. */
	mutex = mmap(RT_NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
	if (RT_UNLIKELY(mutex == MAP_FAILED))
		goto end;
	memory_mapped = RT_TRUE;

	if (create) {
		/* pthread_mutexattr_init returns an errno in case of error. */
		error = pthread_mutexattr_init(&mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}
		mutex_attributes_created = RT_TRUE;

		/* pthread_mutexattr_setpshared returns an errno in case of error. */
		error = pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_SHARED);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}

		/* pthread_mutexattr_setrobust returns an errno in case of error. */
		error = pthread_mutexattr_setrobust(&mutex_attributes, PTHREAD_MUTEX_ROBUST);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}

		/* pthread_mutex_init returns an errno in case of error. */
		error = pthread_mutex_init(mutex, &mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}

	}
	shared_mutex->mutex = mutex;

#endif
	ret = RT_OK;
end:
#ifdef RT_DEFINE_LINUX
	if (mutex_attributes_created) {
		/* pthread_mutexattr_destroy returns an errno in case of error. */
		error = pthread_mutexattr_destroy(&mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			ret = RT_FAILED;
		}
	}
	if (shared_memory_file_descriptor_created) {
		/* close returns -1 and sets errno in case of error. */
		if (RT_UNLIKELY(close(shared_memory_file_descriptor)))
			ret = RT_FAILED;
	}

	if (RT_UNLIKELY(!ret)) {
		if (memory_mapped) {
			munmap(mutex, sizeof(pthread_mutex_t));
		}
		if (shared_memory_file_descriptor_created) {
			close(shared_memory_file_descriptor);
		}
		if (shared_memory_created) {
			shm_unlink(name);
		}
	}
#endif
	return ret;
}

rt_s rt_shared_mutex_acquire(struct rt_shared_mutex *shared_mutex)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(WaitForSingleObject(shared_mutex->handle, INFINITE) == WAIT_FAILED))
		goto end;
#else
	/* pthread_mutex_lock returns an errno in case of error. */
	error = pthread_mutex_lock(shared_mutex->mutex);
	if (RT_UNLIKELY(error)) {
		if (error == EOWNERDEAD) {
			/* The owner of the robust mutex died. We repair the mutex. We own the mutex, by the way. */
			error = pthread_mutex_consistent(shared_mutex->mutex);
			if (RT_UNLIKELY(error)) {
				errno = error;
				goto end;
			}
		} else {
			errno = error;
			goto end;
		}
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_shared_mutex_release(struct rt_shared_mutex *shared_mutex)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!ReleaseMutex(shared_mutex->handle)))
		goto end;
#else
	/* pthread_mutex_unlock returns an errno in case of error. */
	error = pthread_mutex_unlock(shared_mutex->mutex);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_shared_mutex_free(struct rt_shared_mutex *shared_mutex)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(!CloseHandle((HANDLE)shared_mutex->handle)))
		ret = RT_FAILED;
#else
	/* munmap returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(munmap(shared_mutex->mutex, sizeof(pthread_mutex_t)) == -1))
		ret = RT_FAILED;
#endif

	return ret;
}

rt_s rt_shared_mutex_destroy(RT_WINDOWS_UNUSED const rt_char *name)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_LINUX
	/* shm_unlink returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(shm_unlink(name) == -1))
		ret = RT_FAILED;
#endif
	return ret;
}
