#include "layer002/rt_critical_section.h"

#include "layer001/rt_os_headers.h"

/**
 *
 * <p>
 * Only Linux provides a static initializer for mutex.<br>
 * There is no static initializer for Windows critical sections.<br>
 * The consequence is that we need a constructor while it can be annoying in some case.<br>
 * Anyway we also need a constructor for Linux recursive critical sections.
 * </p>
 *
 * <p>
 * Fast initialization could be used in rt_critical_section_enter to avoid having to call rt_critical_section_create.<br>
 * However, the user would still have to manage the lifecycle of the critical section through rt_critical_section_free.<br>
 * So the overhead of fast initialization in rt_critical_section_enter would not worth the price.
 * </p>
 *
 * <p>
 * It should not be necessary to optimize rt_critical_section_enter with atomic operations to avoid kernel calls.<br>
 * EnterCriticalSection/pthread_mutex_lock should already use equivalent user mode strategies before calling the kernel.
 * </p>
 */
rt_s rt_critical_section_create(struct rt_critical_section *critical_section, RT_WINDOWS_UNUSED rt_b recursive)
{
#ifndef RT_DEFINE_WINDOWS
	pthread_mutexattr_t mutex_attributes;
	pthread_mutexattr_t *mutex_attributes_pointer;
	int error;
	rt_s ret;
#endif

#ifdef RT_DEFINE_WINDOWS
	/* InitializeCriticalSection cannot fail. */
	InitializeCriticalSection((PCRITICAL_SECTION)critical_section);
	return RT_OK;
#else
	if (recursive) {
		/* pthread_mutexattr_init returns an errno. */
		error = pthread_mutexattr_init(&mutex_attributes);
		if (error) goto error;

		/* pthread_mutexattr_settype returns an errno. */
		error = pthread_mutexattr_settype(&mutex_attributes, PTHREAD_MUTEX_RECURSIVE);
		if (error) goto error;

		mutex_attributes_pointer = &mutex_attributes;
	} else {
		mutex_attributes_pointer = RT_NULL;
	}

	/* pthread_mutex_init returns an errno. */
	error = pthread_mutex_init((pthread_mutex_t*)critical_section, mutex_attributes_pointer);
	if (error) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	errno = error;
	ret = RT_FAILED;
	goto free;
#endif
}

rt_s rt_critical_section_enter(struct rt_critical_section *critical_section)
{
	rt_s ret;
#ifdef RT_DEFINE_WINDOWS
	/* EnterCriticalSection cannot fail. */
	EnterCriticalSection((PCRITICAL_SECTION)critical_section);
	ret = RT_OK;
#else
	/* pthread_mutex_lock returns an errno. */
	int error = pthread_mutex_lock((pthread_mutex_t*)critical_section);
	if (!error) {
		ret = RT_OK;
	} else {
		errno = error;
		ret = RT_FAILED;
	}
#endif
	return ret;
}

rt_s rt_critical_section_leave(struct rt_critical_section *critical_section)
{
	rt_s ret;
#ifdef RT_DEFINE_WINDOWS
	/* LeaveCriticalSection cannot fail. */
	LeaveCriticalSection((PCRITICAL_SECTION)critical_section);
	ret = RT_OK;
#else
	/* pthread_mutex_unlock returns an errno. */
	int error = pthread_mutex_unlock((pthread_mutex_t*)critical_section);
	if (!error) {
		ret = RT_OK;
	} else {
		errno = error;
		ret = RT_FAILED;
	}
#endif
	return ret;
}

rt_s rt_critical_section_free(struct rt_critical_section *critical_section)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* DeleteCriticalSection cannot fail. */
	DeleteCriticalSection((PCRITICAL_SECTION)critical_section);
	ret = RT_OK;
#else
	/* pthread_mutex_destroy returns an errno. */
	int error = pthread_mutex_destroy((pthread_mutex_t*)critical_section);
	if (!error) {
		ret = RT_OK;
	} else {
		errno = error;
		ret = RT_FAILED;
	}
#endif
	return ret;
}
