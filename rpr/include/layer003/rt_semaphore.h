#ifndef RT_SEMAPHORE_H
#define RT_SEMAPHORE_H

#include "layer000/rt_types.h"

/**
 * @file
 * Semaphores.
 * 
 * <p>
 * The semaphore maximum value is the number of threads that can acquire it.
 * When you attempt to acquire a semaphore, you wait until its value is greater than zero.
 * When you acquire it, you decrement its value by one.
 * And when you release it, you increment its value by one.
 * </p> 
 * 
 * <p>
 * If a name is provided, then the semaphore is shared between processes.<br>
 * But beware that unlike shared mutexes, semaphores leak in case of process crash before calling ReleaseSemaphore/sem_post.
 * </p>
 */

struct rt_semaphore {
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
#else
	rt_char8 semaphore[32];   /* Contains the semaphore in case of anonymous semaphore. */
	void **semaphore_address; /* Contains the address of the semaphore. */
	rt_b named;
#endif
};

/**
 * @param name Must not be <tt>RT_NULL</tt> for a shared semaphore.
 */
RT_API rt_s rt_semaphore_create(struct rt_semaphore *semaphore, const rt_char *name, rt_un32 value);

RT_API rt_s rt_semaphore_acquire(struct rt_semaphore *semaphore);

RT_API rt_s rt_semaphore_release(struct rt_semaphore *semaphore);

RT_API rt_s rt_semaphore_free(struct rt_semaphore *semaphore);

/**
 * Can be called once to get ride of the Linux semaphore after all processes are done.<br>
 * Applies to shared semaphores only.
 */
RT_API rt_s rt_semaphore_destroy(const rt_char *name);

#endif /* RT_SEMAPHORE_H */
