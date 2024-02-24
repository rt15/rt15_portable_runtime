#ifndef RT_CRITICAL_SECTION_H
#define RT_CRITICAL_SECTION_H

#include "layer000/rt_types.h"

/**
 * @file
 * Critical section implementation for Windows and Linux.
 *
 * <p>
 * Can be used as a mutex.
 * </p>
 * 
 * <p>
 * A critical section is a portion of instructions that can be executed by only one thread at a given time.
 * </p>
 */

struct rt_critical_section {
#ifdef RT_DEFINE_WINDOWS
	void    *debug_info;
	rt_n32  lock_count;
	rt_n32  recursion_count;
	rt_h    owning_thread_handle;
	rt_h    lock_semaphore_handle;
	rt_un   spin_count;
#else
#ifdef RT_DEFINE_32
	rt_char8 data[32];
#else
	rt_char8 data[40];
#endif
#endif
};

/**
 *
 * <p>
 * You can choose to create a recursive or non-recursive critical section.<br>
 * A recursive critical section can be recursively executed by the same thread.<br>
 * Under Linux, recursive critical sections are slower than non-recursive critical sections.<br>
 * Windows only manages recursive critical sections (<tt>recursive</tt> is ignored).
 * </p>
 */
RT_API rt_s rt_critical_section_create(struct rt_critical_section *critical_section, rt_b recursive);
RT_API rt_s rt_critical_section_enter(struct rt_critical_section *critical_section);
RT_API rt_s rt_critical_section_leave(struct rt_critical_section *critical_section);
RT_API rt_s rt_critical_section_free(struct rt_critical_section *critical_section);

#endif /* RT_CRITICAL_SECTION_H */
