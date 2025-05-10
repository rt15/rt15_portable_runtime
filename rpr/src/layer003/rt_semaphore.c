#include "layer003/rt_semaphore.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_semaphore_create(struct rt_semaphore *semaphore, const rt_char *name, rt_un32 value)
{
#ifdef RT_DEFINE_WINDOWS
	HANDLE handle;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* The handle is not inherited. */
	/* Returns null and sets last error in case of issue. */
	handle = CreateSemaphore(RT_NULL, value, value, name);
	if (RT_UNLIKELY(!handle))
		goto end;
	
	semaphore->handle = handle;
#else
	if (name) {
		semaphore->named = RT_TRUE;
		semaphore->semaphore_address = (void**)sem_open(name, O_CREAT, 0666, value);
		if (RT_UNLIKELY((void*)semaphore->semaphore_address == SEM_FAILED))
			goto end;
	} else {
		semaphore->named = RT_FALSE;
		semaphore->semaphore_address = (void**)&semaphore->semaphore;
		/* Returns -1 and sets errno in case of issue. */
		if (RT_UNLIKELY(sem_init((sem_t*)semaphore->semaphore_address, 0, value) == -1))
			goto end;
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_semaphore_acquire(struct rt_semaphore *semaphore)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	returned_value = WaitForSingleObject(semaphore->handle, INFINITE);
	if (returned_value != WAIT_OBJECT_0) {
		/* If returned value is WAIT_FAILED, then last error is set. */
		if (RT_UNLIKELY(returned_value != WAIT_FAILED)) {
			/* Set an arbitrary last error for WAIT_TIMEOUT/WAIT_ABANDONED cases. */
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		}
		goto end;
	}
#else
	/* Returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(sem_wait((sem_t*)semaphore->semaphore_address) == -1))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_semaphore_release(struct rt_semaphore *semaphore)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!ReleaseSemaphore(semaphore->handle, 1, RT_NULL)))
		goto end;
#else
	/* Returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(sem_post((sem_t*)semaphore->semaphore_address) == -1))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_semaphore_free(struct rt_semaphore *semaphore)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Returns zero and set last error in case of issue. */
	if (RT_UNLIKELY(!CloseHandle(semaphore->handle)))
		goto end;
#else
	if (semaphore->named) {
		/* Returns -1 and sets errno in case of issue. */
		if (RT_UNLIKELY(sem_close((sem_t*)semaphore->semaphore_address) == -1))
			goto end;
	} else {
		/* Returns -1 and sets errno in case of issue. */
		if (RT_UNLIKELY(sem_destroy((sem_t*)semaphore->semaphore_address) == -1))
			goto end;
	}
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_semaphore_destroy(RT_WINDOWS_UNUSED const rt_char *name)
{
#ifdef RT_DEFINE_LINUX
	rt_s ret = RT_OK;

	if (RT_UNLIKELY(sem_unlink(name) == -1))
		ret = RT_FAILED;

	return ret;
#endif

#ifdef RT_DEFINE_WINDOWS
	return RT_OK;
#endif
}
