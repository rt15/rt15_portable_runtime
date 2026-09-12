#ifndef RT_EVENT_H
#define RT_EVENT_H

#include "layer000/rt_types.h"

/**
 * @file
 * Simple events usable within a single process.<br>
 * Only a single thread can wait for a single event.<br>
 * The event can be reused by multiple sequential calls to <tt>rt_event_wait_for</tt>/<tt>rt_event_signal</tt>.<br>
 * You can call <tt>rt_event_signal</tt> before <tt>rt_event_wait_for</tt>. In that case <tt>rt_event_wait_for</tt> will not block.
 */

struct rt_event {
#ifdef RT_DEFINE_WINDOWS
	rt_h event_handle;
#else
	rt_n32 file_descriptor;
#endif
};

RT_API rt_s rt_event_create(struct rt_event *event);

/**
 * Signals the event, releasing the waiting thread if any.
 *
 * <p>
 * Signaling an already signaled event has no additional effect.
 * </p>
 */
RT_API rt_s rt_event_signal(struct rt_event *event);

/**
 * Wait for the event to be signaled.
 * 
 * <p>
 * Automatically update the event to not signaled.
 * </p>
 */
RT_API rt_s rt_event_wait_for(struct rt_event *event);

/**
 * Can be used to discard a signal.
 * 
 * <p>
 * Not needed to be called in the regular <tt>rt_event_signal</tt>/<tt>rt_event_signal</tt> usages.
 * </p>
 * 
 * <p>
 * Does nothing if the event is not signaled.
 * </p>
 */
RT_API rt_s rt_event_reset(struct rt_event *event);

RT_API rt_s rt_event_free(struct rt_event *event);

#endif /* RT_EVENT_H */
