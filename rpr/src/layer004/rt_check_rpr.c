#include "layer004/rt_check_rpr.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_chrono.h"
#include "layer002/rt_critical_section.h"
#include "layer003/rt_thread.h"

static rt_s rt_check_thread()
{
	rt_s ret;
	struct rt_thread thread;
#ifdef RT_DEFINE_WINDOWS
	HANDLE ref_thread;
#else
	pthread_t ref_thread;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (sizeof(thread.thread_handle) != sizeof(ref_thread)) goto error;
	if (sizeof(thread) != sizeof(HANDLE)) goto error;
#else
	if (sizeof(thread.thread_pointer) != sizeof(ref_thread)) goto error;
	if (sizeof(thread.thread_pointer) != sizeof(pthread_t)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_chrono()
{
	rt_s ret;
	struct rt_chrono chrono;
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER ref_chrono;
#else
	struct timespec ref_chrono;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (sizeof(chrono.start_counter) != sizeof(ref_chrono)) goto error;
	if (sizeof(chrono) != sizeof(LARGE_INTEGER)) goto error;
#else
	if (sizeof(chrono.seconds) != sizeof(ref_chrono.tv_sec)) goto error;
	if (sizeof(chrono.nano_seconds) != sizeof(ref_chrono.tv_nsec)) goto error;
	if (sizeof(chrono) != sizeof(struct timespec)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_critical_section()
{
	rt_s ret;
	struct rt_critical_section critical_section;
#ifdef RT_DEFINE_WINDOWS
	CRITICAL_SECTION ref_critical_section;
#else
	pthread_mutex_t ref_critical_section;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (sizeof(critical_section.debug_info) != sizeof(ref_critical_section.DebugInfo)) goto error;
	if (sizeof(critical_section.lock_count) != sizeof(ref_critical_section.LockCount)) goto error;
	if (sizeof(critical_section.recursion_count) != sizeof(ref_critical_section.RecursionCount)) goto error;
	if (sizeof(critical_section.owning_thread_handle) != sizeof(ref_critical_section.OwningThread)) goto error;
	if (sizeof(critical_section.lock_semaphore_handle) != sizeof(ref_critical_section.LockSemaphore)) goto error;
	if (sizeof(critical_section.spin_count) != sizeof(ref_critical_section.SpinCount)) goto error;

	if (sizeof(struct rt_critical_section) != sizeof(CRITICAL_SECTION)) goto error;
#else
	/* __SIZEOF_PTHREAD_MUTEX_T from /usr/include/bits/pthreadtypes.h. */
	if (sizeof(critical_section.data) != sizeof(ref_critical_section)) goto error;
	if (sizeof(critical_section) != sizeof(pthread_mutex_t)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_types()
{
	rt_un pointer_size;
	rt_un char_size;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	char_size = 2;
#else
	char_size = 1;
#endif

#ifdef RT_DEFINE_32
	pointer_size = 4;
#else
	pointer_size = 8;
#endif

	if (sizeof(void*) != pointer_size) goto error;

	if (sizeof(rt_char8) != 1) goto error;
	if (sizeof(rt_uchar8) != 1) goto error;
	if (sizeof(rt_un16) != 2) goto error;
	if (sizeof(rt_n16) != 2) goto error;

	if (sizeof(rt_char) != char_size) goto error;
	if (sizeof(rt_uchar) != char_size) goto error;

	if (sizeof(rt_b) != 4) goto error;
	if (sizeof(rt_s) != 4) goto error;

	if (sizeof(rt_n32) != 4) goto error;
	if (sizeof(rt_un32) != 4) goto error;

	if (sizeof(rt_n64) != 8) goto error;
	if (sizeof(rt_un64) != 8) goto error;

	if (sizeof(rt_n) != pointer_size) goto error;
	if (sizeof(rt_un) != pointer_size) goto error;

	if (sizeof(rt_h) != pointer_size) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (sizeof(rt_uchar8) != sizeof(BYTE)) goto error;
	if (sizeof(rt_n16) != sizeof(WORD)) goto error;

	if (sizeof(rt_n32) != sizeof(INT)) goto error;
	if (sizeof(rt_n32) != sizeof(INT32)) goto error;
	if (sizeof(rt_n32) != sizeof(LONG)) goto error;

	if (sizeof(rt_un32) != sizeof(DWORD)) goto error;
	if (sizeof(rt_un32) != sizeof(UINT)) goto error;
	if (sizeof(rt_un32) != sizeof(ULONG)) goto error;

	if (sizeof(rt_un64) != sizeof(LARGE_INTEGER)) goto error;

	if (sizeof(rt_n) != sizeof(LRESULT)) goto error;
	if (sizeof(rt_n) != sizeof(LONG_PTR)) goto error;
	if (sizeof(rt_n) != sizeof(LPARAM)) goto error;

	if (sizeof(rt_un) != sizeof(UINT_PTR)) goto error;
	if (sizeof(rt_un) != sizeof(ULONG_PTR)) goto error;
	if (sizeof(rt_un) != sizeof(DWORD_PTR)) goto error;
	if (sizeof(rt_un) != sizeof(WPARAM)) goto error;

	if (sizeof(rt_h) != sizeof(HANDLE)) goto error;
#else
	/* _FILE_OFFSET_BITS should be set to 64 even in 32 bits case. */
	if (sizeof(rt_n64) != sizeof(off_t)) goto error;

	if (sizeof(rt_n) != sizeof(ssize_t)) goto error;
	if (sizeof(rt_n) != sizeof(time_t)) goto error;

	if (sizeof(rt_un) != sizeof(size_t)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_signedness()
{
	rt_uchar8 rt_uchar8_variable = -1;
	rt_un16 rt_un16_variable = -1;
	rt_n16 rt_n16_variable = -1;
	rt_uchar rt_uchar_variable = -1;
	rt_n32 rt_n32_variable = -1;
	rt_un32 rt_un32_variable = -1;
	rt_n64 rt_n64_variable = -1;
	rt_un64 rt_un64_variable = -1;
	rt_n rt_n_variable = -1;
	rt_un rt_un_variable = -1;
	rt_s ret;

	if (rt_n16_variable > 0) goto error;
	if (rt_n32_variable > 0) goto error;
	if (rt_n64_variable > 0) goto error;
	if (rt_n_variable > 0) goto error;

#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
	if (rt_uchar8_variable < 0) goto error;
	if (rt_un16_variable < 0) goto error;
	if (rt_uchar_variable < 0) goto error;
	if (rt_un32_variable < 0) goto error;
	if (rt_un64_variable < 0) goto error;
	if (rt_un_variable < 0) goto error;
#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic pop
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_check_rpr()
{
	rt_s ret;

	if (!rt_check_types()) goto error;
	if (!rt_check_signedness()) goto error;
	if (!rt_check_critical_section()) goto error;
	if (!rt_check_chrono()) goto error;
	if (!rt_check_thread()) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
