#ifndef RT_EVENT_H
#define RT_EVENT_H

#include "layer000/rt_types.h"

/**
 * @file
 * Simple events usable within a single process.<br>
 * Only a single thread can wait for a single event.<br>
 * The event can be reused by multiple sequential calls to <tt>rt_event_wait_for</tt>/<tt>rt_event_signal</tt>.
 * You can call <tt>rt_event_signal</tt> before <tt>rt_event_wait_for</tt>. In that case <tt>rt_event_wait_for</tt> will not block.
 */

struct rt_event {
#ifdef RT_DEFINE_WINDOWS
	rt_h event_handle;
#else
	rt_n32 file_descriptor;
#endif
};

rt_s rt_event_create(struct rt_event *event);

rt_s rt_event_signal(struct rt_event *event);

rt_s rt_event_wait_for(struct rt_event *event);

rt_s rt_event_free(struct rt_event *event);

#endif /* RT_EVENT_H */
