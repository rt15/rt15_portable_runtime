#ifndef RT_SHARED_MUTEX_H
#define RT_SHARED_MUTEX_H

#include "layer000/rt_types.h"

/**
 * @file
 * Shared mutexes can be used for inter-process synchronization.
 * 
 * <p>
 * If want to use a mutex in a single process, use rt_critical_section instead.
 * </p>
 * 
 * <p>
 * Under Linux, a shared memory is created and a robust mutex is stored into it.<br>
 * The shared memory is listed into /dev/shm, and is not removed after all the processes exit, unless one of them calls rt_shared_mutex_destroy.
 * <p>
 */

struct rt_shared_mutex {
#ifdef RT_DEFINE_WINDOWS
    rt_h handle;
#else
    void *mutex;
#endif
};

/**
 * @param name The name has to start with a slash.
 */
RT_API rt_s rt_shared_mutex_create(struct rt_shared_mutex *shared_mutex, const rt_char *name);

RT_API rt_s rt_shared_mutex_acquire(struct rt_shared_mutex *shared_mutex);

RT_API rt_s rt_shared_mutex_release(struct rt_shared_mutex *shared_mutex);

RT_API rt_s rt_shared_mutex_free(struct rt_shared_mutex *shared_mutex);

/**
 * Can be called once to get ride of the Linux shared memory after all processes are done. 
 */
RT_API rt_s rt_shared_mutex_destroy(const rt_char *name);

#endif /* RT_SHARED_MUTEX_H */
