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
#ifdef RT_DEFINE_LINUX
	pthread_mutexattr_t mutex_attributes;
	pthread_mutexattr_t *mutex_attributes_pointer;
	rt_b mutex_attributes_created = RT_FALSE;
	rt_b mutex_created = RT_FALSE;
	int error;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* Unlike InitializeCriticalSection, which can raise a STATUS_NO_MEMORY exception before Vista, this function reports failures. */
	/* Returns zero and sets last error in case of failure. Cannot fail since Vista. */
	if (RT_UNLIKELY(!InitializeCriticalSectionAndSpinCount((PCRITICAL_SECTION)critical_section, 0)))
		goto end;

	ret = RT_OK;
end:
	return ret;
#else
	if (recursive) {
		/* pthread_mutexattr_init returns an errno. */
		error = pthread_mutexattr_init(&mutex_attributes);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}
		mutex_attributes_created = RT_TRUE;

		/* pthread_mutexattr_settype returns an errno. */
		error = pthread_mutexattr_settype(&mutex_attributes, PTHREAD_MUTEX_RECURSIVE);
		if (RT_UNLIKELY(error)) {
			errno = error;
			goto end;
		}

		mutex_attributes_pointer = &mutex_attributes;
	} else {
		mutex_attributes_pointer = RT_NULL;
	}

	/* pthread_mutex_init returns an errno. */
	error = pthread_mutex_init((pthread_mutex_t*)critical_section, mutex_attributes_pointer);
	if (RT_UNLIKELY(error)) {
		errno = error;
		goto end;
	}
	mutex_created = RT_TRUE;

	ret = RT_OK;
end:
	if (mutex_attributes_created) {
		/* pthread_mutexattr_destroy returns an errno. */
		error = pthread_mutexattr_destroy(&mutex_attributes);
		if (RT_UNLIKELY(error) && ret) {
			errno = error;
			ret = RT_FAILED;
		}
	}

	if (RT_UNLIKELY(!ret)) {
		if (mutex_created)
			pthread_mutex_destroy((pthread_mutex_t*)critical_section);
	}

	return ret;
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
