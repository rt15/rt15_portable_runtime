#ifndef RT_THREAD_H
#define RT_THREAD_H

#include "layer000/rt_types.h"

/**
 * @file
 * Operating system threads.
 *
 * <p>
 * pthread_suspend is (or was?) not implemented in Linux.
 * </p>
 */

typedef rt_un32 (RT_STDCALL *rt_thread_callback_t)(void *parameter);

struct rt_thread {
#ifdef RT_DEFINE_WINDOWS
	rt_h thread_handle;
#else
	/* pthread_t is in fact a pointer on a struct. */
	rt_un thread_pointer;
	/* User callback. */
	rt_thread_callback_t thread_callback;
	/* User parameter. */
	void *parameter;
	rt_un32 exit_code;
	rt_b exit_code_set;
#endif
};


rt_s rt_thread_create(struct rt_thread *thread, rt_thread_callback_t thread_callback, void *parameter);

rt_s rt_thread_join(struct rt_thread *thread);

/**
 * Join with a thread and check that exit code is non-zero.
 *
 * <p>
 * Basically calls <tt>rt_thread_join</tt> then <tt>rt_thread_get_exit_code</tt>.
 * </p
 */
rt_s rt_thread_join_and_check(struct rt_thread *thread);

/**
 * <p>
 * Must not be called after <tt>rt_thread_free</tt>.
 * </p>
 */
rt_s rt_thread_get_exit_code(struct rt_thread *thread, rt_un32 *exit_code);

rt_s rt_thread_free(struct rt_thread *thread);

#endif /* RT_THREAD_H */
