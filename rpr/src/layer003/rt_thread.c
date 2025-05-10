#include "layer003/rt_thread.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

#ifndef RT_DEFINE_WINDOWS

/**
 * Wrapper callback for Linux that calls the user provided one.
 *
 * @param void_thread The rt_thread. It cannot be a custom struct allocated in rt_thread_create stack frame.
 */
static void *RT_CDECL rt_thread_linux_callback(void *void_thread)
{
	struct rt_thread *thread;

	thread = (struct rt_thread*)void_thread;

	/* Call user callback with user parameter and save exit code. */
	thread->exit_code = thread->thread_callback(thread->parameter);
	thread->exit_code_set = RT_TRUE;

	return RT_NULL;
}

#endif

rt_s rt_thread_create(struct rt_thread *thread, rt_thread_callback_t thread_callback, void *parameter)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	rt_un32 thread_id;
	thread->thread_handle = CreateThread(RT_NULL, 0, thread_callback, parameter, 0, &thread_id);
	ret = (thread->thread_handle != RT_NULL);
#else

	/* Save user callback and parameter. */
	thread->thread_callback = thread_callback;
	thread->parameter = parameter;

	/* Initialize exit_code_set */
	thread->exit_code_set = RT_FALSE;

	/* Start the thread. */
	/* pthread_create returns an errno. */
	error = pthread_create((pthread_t*)&thread->thread_pointer, RT_NULL, &rt_thread_linux_callback, thread);
	if (!error) {
		ret = RT_OK;
	} else {
		errno = error;
		ret = RT_FAILED;
	}
#endif
	return ret;
}

rt_s rt_thread_join(struct rt_thread *thread)
{
#ifdef RT_DEFINE_LINUX
	int error;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = (WaitForSingleObject(thread->thread_handle, INFINITE) != WAIT_FAILED);
#else

	/* The pthread_join function returns an errno. */
	error = pthread_join(thread->thread_pointer, RT_NULL);
	if (!error) {
		ret = RT_OK;
	} else {
		errno = error;
		ret = RT_FAILED;
	}
#endif
	return ret;
}

rt_s rt_thread_join_and_check(struct rt_thread *thread)
{
	rt_un32 thread_exit_code;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_thread_join(thread)))
		goto end;
	if (RT_UNLIKELY(!rt_thread_get_exit_code(thread, &thread_exit_code)))
		goto end;
	if (RT_UNLIKELY(!thread_exit_code)) {
		/* Unknown error in the joined thread which cannot set last error for this thread. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto end;
	}

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_thread_get_exit_code(struct rt_thread *thread, rt_un32 *exit_code)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = GetExitCodeThread(thread->thread_handle, exit_code);
#else

	ret = thread->exit_code_set;
	if (ret)
		*exit_code = thread->exit_code;
	else
		errno = ENODATA;
#endif
	return ret;
}

rt_s rt_thread_free(RT_LINUX_UNUSED struct rt_thread *thread)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = CloseHandle(thread->thread_handle);
#else
	ret = RT_OK;
#endif
	return ret;
}
