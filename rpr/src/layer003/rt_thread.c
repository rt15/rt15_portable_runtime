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
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	rt_un32 thread_id;
	thread->thread_handle = CreateThread(RT_NULL, 0, thread_callback, parameter, 0, &thread_id);
	ret = (thread->thread_handle != RT_NULL);
#else /* NOT RT_DEFINE_WINDOWS */

	/* Save user callback and parameter. */
	thread->thread_callback = thread_callback;
	thread->parameter = parameter;

	/* Initialize exit_code_set */
	thread->exit_code_set = RT_FALSE;

	/* Start the thread. The pthread_create function return -1 in case of failure and set errno. */
	ret = !pthread_create((pthread_t*)&thread->thread_pointer, RT_NULL, &rt_thread_linux_callback, thread);
#endif
	return ret;
}

rt_s rt_thread_join(struct rt_thread *thread)
{
#ifndef RT_DEFINE_WINDOWS
	int returned_value;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = (WaitForSingleObject(thread->thread_handle, INFINITE) != WAIT_FAILED);
#else /* NOT RT_DEFINE_WINDOWS */

	/* The pthread_join function returns an errno. */
	returned_value = pthread_join(thread->thread_pointer, RT_NULL);
	if (returned_value) {
		errno = returned_value;
		ret = RT_FAILED;
	} else {
		ret = RT_OK;
	}
#endif
	return ret;
}

rt_s rt_thread_join_and_check(struct rt_thread *thread)
{
	rt_un32 thread_exit_code;
	rt_s ret;

	if (!rt_thread_join(thread))
		goto error;
	if (!rt_thread_get_exit_code(thread, &thread_exit_code))
		goto error;
	if (!thread_exit_code) {
		/* Unknown error in the joined thread which cannot set last error for this thread. */
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_thread_get_exit_code(struct rt_thread *thread, rt_un32 *exit_code)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = GetExitCodeThread(thread->thread_handle, exit_code);
#else /* NOT RT_DEFINE_WINDOWS */

	ret = thread->exit_code_set;
	if (ret)
		*exit_code = thread->exit_code;
	else
		errno = ENODATA;
#endif
	return ret;
}

rt_s rt_thread_free(struct rt_thread *thread)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	ret = CloseHandle(thread->thread_handle);
#else /* NOT RT_DEFINE_WINDOWS */
	ret = RT_OK;
#endif
	return ret;
}
