#ifndef RT_READERS_WRITER_MUTEX_H
#define RT_READERS_WRITER_MUTEX_H

#include "layer000/rt_types.h"
#include "layer002/rt_critical_section.h"
#include "layer002/rt_event.h"

/**
 * @file
 * Readers-writer mutex.
 * 
 * <p>
 * A readers-writer mutex allows either multiple readers or a single writer to access a resource.
 * </p>
 */

struct rt_readers_writer_mutex {
	struct rt_critical_section critical_section;
	struct rt_event event;
	rt_un readers_count;
	rt_b writer;
	rt_un spinlock;
};

RT_API rt_s rt_readers_writer_mutex_create(struct rt_readers_writer_mutex *readers_writer_mutex);

RT_API rt_s rt_readers_writer_mutex_acquire_read(struct rt_readers_writer_mutex *readers_writer_mutex);
RT_API rt_s rt_readers_writer_mutex_release_read(struct rt_readers_writer_mutex *readers_writer_mutex);

RT_API rt_s rt_readers_writer_mutex_acquire_write(struct rt_readers_writer_mutex *readers_writer_mutex);
RT_API rt_s rt_readers_writer_mutex_release_write(struct rt_readers_writer_mutex *readers_writer_mutex);

RT_API rt_s rt_readers_writer_mutex_free(struct rt_readers_writer_mutex *readers_writer_mutex);

#endif /* RT_READERS_WRITER_MUTEX_H */
