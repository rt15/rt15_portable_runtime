#ifndef RT_CONDITION_VARIABLE_H
#define RT_CONDITION_VARIABLE_H

#include "layer000/rt_types.h"
#include "layer002/rt_critical_section.h"
#include "layer002/rt_event.h"

/**
 * @file
 * Condition variables can be used to make multiple threads wait for a particular event.<br>
 * You can use <tt>signal</tt> to release one thread or <tt>broadcast</tt> to release all the threads.
 *
 * <p>
 * Beware of spurious wake ups: <tt>rt_condition_variable_wait</tt> can return while the expected condition is not met.<br>
 * As a result, the condition must always be checked in a loop, while holding the critical section:
 * </p>
 * <pre>
 * rt_critical_section_enter(&critical_section);
 * while (!condition) {
 *   rt_condition_variable_wait(&condition_variable);
 * }
 * // Use the condition here, the critical section is still acquired.
 * rt_critical_section_leave(&critical_section);
 * </pre>
 *
 * <p>
 * The condition must be modified only while holding the critical section, otherwise a wake up could be lost.
 * </p>
 */

struct rt_condition_variable {
	struct rt_critical_section *critical_section;
#ifdef RT_DEFINE_WINDOWS
	rt_h semaphore_handle;
	rt_un waiters_count;
	struct rt_critical_section waiters_count_lock;
	rt_b was_broadcast;
	struct rt_event waiters_done_event;
#else
	rt_char8 data[48];
#endif
};

RT_API rt_s rt_condition_variable_create(struct rt_condition_variable *condition_variable, struct rt_critical_section *critical_section);

/**
 * Makes the calling thread wait until it is signaled.
 * 
 * <p>
 * The critical section must be acquired by the caller before calling this function.<br>
 * This function releases the critical section while waiting.<br>
 * But the critical section is acquired back upon returning.
 * </p>
 *
 * <p>
 * This function can return while the condition is not met (spurious wake up).<br>
 * It must be called in a loop that checks the condition, see the file documentation.
 * </p>
 */
RT_API rt_s rt_condition_variable_wait(struct rt_condition_variable *condition_variable);

/**
 * Releases a waiting thread.
 *
 * <p>
 * The critical section must be acquired by the caller before calling this function.
 * </p>
 *
 * <p>
 * If no thread is waiting, nothing is done.
 * </p>
 */
RT_API rt_s rt_condition_variable_signal(struct rt_condition_variable *condition_variable);

/**
 * Releases all waiting threads.
 *
 * <p>
 * The critical section must be acquired by the caller before calling this function.
 * </p>
 *
 * <p>
 * If no thread is waiting, nothing is done.
 * </p>
 */
RT_API rt_s rt_condition_variable_broadcast(struct rt_condition_variable *condition_variable);

RT_API rt_s rt_condition_variable_free(struct rt_condition_variable *condition_variable);

#endif /* RT_CONDITION_VARIABLE_H */
