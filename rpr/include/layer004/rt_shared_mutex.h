#ifndef RT_SHARED_MUTEX_H
#define RT_SHARED_MUTEX_H

#include "layer000/rt_types.h"
#include "layer002/rt_critical_section.h"
#include "layer002/rt_event.h"

/**
 * @file
 * Shared mutex.
 * 
 * <p>
 * A shared mutex allows either multiple readers or a single writer to access a resource.
 * </p>
 */

struct rt_shared_mutex {
	struct rt_critical_section critical_section;
	struct rt_event event;
	rt_un readers_count;
	rt_b writer;
	rt_un spinlock;
};

RT_API rt_s rt_shared_mutex_create(struct rt_shared_mutex *shared_mutex);

RT_API rt_s rt_shared_mutex_acquire_read(struct rt_shared_mutex *shared_mutex);
RT_API rt_s rt_shared_mutex_release_read(struct rt_shared_mutex *shared_mutex);

RT_API rt_s rt_shared_mutex_acquire_write(struct rt_shared_mutex *shared_mutex);
RT_API rt_s rt_shared_mutex_release_write(struct rt_shared_mutex *shared_mutex);

RT_API rt_s rt_shared_mutex_free(struct rt_shared_mutex *shared_mutex);

#endif /* RT_SHARED_MUTEX_H */
