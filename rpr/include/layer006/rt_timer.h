#ifndef RT_TIMER_H
#define RT_TIMER_H

#include "layer000/rt_types.h"

#include "layer002/rt_critical_section.h"
#include "layer002/rt_event.h"
#include "layer003/rt_thread.h"
#include "layer005/rt_sortable_array.h"

/**
 * @file
 * Allows to execute code at a specified frequency.
 *
 * <p>
 * All callbacks are called by a single thread managed by the timer manager.
 * </p>
 */

 /**
  * <p>
  * If a callback fails, all timers are stopped.<br>
  * The error will be notified on the call to <tt>rt_timer_manager_free</tt>.
  * </p>
  */
typedef rt_s (*rt_timer_callback_t)(void *parameter);

struct rt_timer_manager {
	struct rt_sortable_array sortable_array;
	struct rt_critical_section critical_section;
	struct rt_thread thread;
	rt_b stop;
	struct rt_event event;
};

struct rt_timer {
	struct rt_timer_manager *timer_manager;
	rt_un period_ms;
	rt_timer_callback_t callback;
	void *parameter;
	rt_un64 last_execution;
	rt_un64 next_execution;
};

RT_API rt_s rt_timer_manager_create(struct rt_timer_manager *timer_manager, struct rt_heap *heap);

/**
 * Add a new timer to given <tt>timer_manager</tt>.
 */
RT_API rt_s rt_timer_create(struct rt_timer *timer, struct rt_timer_manager *timer_manager, rt_un period_ms, rt_timer_callback_t callback, void *parameter);

/**
 * Stop a timer.
 * 
 * <p>
 * Can be called from the timer callback or any other thread.
 * </p>
 * 
 * <p>
 * If called during the callback execution, it will wait for it to finish before removing it.
 * </p>
 */
RT_API rt_s rt_timer_free(struct rt_timer *timer);

RT_API rt_s rt_timer_manager_free(struct rt_timer_manager *timer_manager);

#endif /* RT_TIMER_H */
