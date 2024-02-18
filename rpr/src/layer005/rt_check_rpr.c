#include "layer005/rt_check_rpr.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_chrono.h"
#include "layer002/rt_critical_section.h"
#include "layer003/rt_thread.h"
#include "layer004/rt_uuid.h"

static rt_s rt_check_flags(void)
{
#if defined(RT_DEFINE_32) && defined(RT_DEFINE_64)
#error Both RT_DEFINE_32 and RT_DEFINE_64 are defined.
#endif

#if !defined(RT_DEFINE_32) && !defined(RT_DEFINE_64)
#error Either RT_DEFINE_32 or RT_DEFINE_64 must be defined.
#endif

#if defined(RT_DEFINE_VC) && defined(RT_DEFINE_GCC)
#error Both RT_DEFINE_VC and RT_DEFINE_GCC are defined.
#endif

#if !defined(RT_DEFINE_VC) && !defined(RT_DEFINE_GCC)
#error Either RT_DEFINE_VC or RT_DEFINE_GCC must be defined.
#endif

#if defined(RT_DEFINE_WINDOWS) && defined(RT_DEFINE_LINUX)
#error Both RT_DEFINE_WINDOWS and RT_DEFINE_LINUX are defined.
#endif

#if !defined(RT_DEFINE_WINDOWS) && !defined(RT_DEFINE_LINUX)
#error Either RT_DEFINE_WINDOWS or RT_DEFINE_LINUX must be defined.
#endif
	return RT_OK;
}

static rt_s rt_check_types(void)
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

	if (RT_UNLIKELY(sizeof(void*) != pointer_size)) goto error;

	if (RT_UNLIKELY(sizeof(rt_char8) != 1)) goto error;
	if (RT_UNLIKELY(sizeof(rt_uchar8) != 1)) goto error;
	if (RT_UNLIKELY(sizeof(rt_un16) != 2)) goto error;
	if (RT_UNLIKELY(sizeof(rt_n16) != 2)) goto error;

	if (RT_UNLIKELY(sizeof(rt_char) != char_size)) goto error;
	if (RT_UNLIKELY(sizeof(rt_uchar) != char_size)) goto error;

	if (RT_UNLIKELY(sizeof(rt_b) != 4)) goto error;
	if (RT_UNLIKELY(sizeof(rt_s) != 4)) goto error;

	if (RT_UNLIKELY(sizeof(rt_n32) != 4)) goto error;
	if (RT_UNLIKELY(sizeof(rt_un32) != 4)) goto error;

	if (RT_UNLIKELY(sizeof(rt_n64) != 8)) goto error;
	if (RT_UNLIKELY(sizeof(rt_un64) != 8)) goto error;

	if (RT_UNLIKELY(sizeof(rt_n) != pointer_size)) goto error;
	if (RT_UNLIKELY(sizeof(rt_un) != pointer_size)) goto error;

	if (RT_UNLIKELY(sizeof(rt_h) != pointer_size)) goto error;

	if (RT_UNLIKELY(FD_SETSIZE != 1024)) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(sizeof(rt_uchar8) != sizeof(BYTE))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n16) != sizeof(WORD))) goto error;

	if (RT_UNLIKELY(sizeof(rt_n32) != sizeof(INT))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n32) != sizeof(BOOL))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n32) != sizeof(INT32))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n32) != sizeof(LONG))) goto error;

	if (RT_UNLIKELY(sizeof(rt_un32) != sizeof(DWORD))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un32) != sizeof(UINT))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un32) != sizeof(ULONG))) goto error;

	if (RT_UNLIKELY(sizeof(rt_un64) != sizeof(LARGE_INTEGER))) goto error;

	if (RT_UNLIKELY(sizeof(rt_n) != sizeof(LRESULT))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n) != sizeof(LONG_PTR))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n) != sizeof(LPARAM))) goto error;

	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(UINT_PTR))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(ULONG_PTR))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(DWORD_PTR))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(WPARAM))) goto error;
	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(SOCKET))) goto error;

	if (RT_UNLIKELY(sizeof(rt_h) != sizeof(HANDLE))) goto error;

	/* UUID. */
	if (RT_UNLIKELY(sizeof(struct rt_uuid) != sizeof(UUID))) goto error;

#ifdef RT_DEFINE_32
	if (RT_UNLIKELY(sizeof(fd_set) != sizeof(rt_un32) + sizeof(rt_un) * FD_SETSIZE)) goto error;
#else
	if (RT_UNLIKELY(sizeof(fd_set) != sizeof(rt_un32) + sizeof(rt_un) * FD_SETSIZE + 4 /* padding. */)) goto error;
#endif
#else
	/* _FILE_OFFSET_BITS should be set to 64 even in 32 bits case. */
	if (RT_UNLIKELY(sizeof(rt_n64) != sizeof(off_t))) goto error;
	if (RT_UNLIKELY(sizeof(rt_n) != sizeof(ssize_t))) goto error;
	if (RT_UNLIKELY(sizeof(fd_set) != sizeof(rt_n) * FD_SETSIZE / 64 /* __FD_SETSIZE / __NFDBITS */)) goto error;

	/* Under VC, in 32 bits, time_t is 64 bits unless _USE_32BIT_TIME_T is used. */
	/* Under Linux, it is 32 bits if the glibc is 32 bits. */
	if (RT_UNLIKELY(sizeof(rt_n) != sizeof(time_t))) goto error;

	/* UUID. */
	if (RT_UNLIKELY(sizeof(struct rt_uuid) != sizeof(uuid_t))) goto error;

	/* Linux thread local storage key. */
	if (RT_UNLIKELY(sizeof(rt_un32) != sizeof(pthread_key_t))) goto error;

	/* Used in rt_get_process_id. */
	if (RT_UNLIKELY(sizeof(rt_un) < sizeof(pid_t))) goto error;
#endif

	/* Socket address structures. */
	if (RT_UNLIKELY(sizeof(struct in_addr) != 4)) goto error;
	if (RT_UNLIKELY(sizeof(struct in6_addr) != 16)) goto error;
	if (RT_UNLIKELY(sizeof(struct sockaddr) != 16)) goto error;
	if (RT_UNLIKELY(sizeof(struct sockaddr_in) != 16)) goto error;
	if (RT_UNLIKELY(sizeof(struct sockaddr_in6) != 28)) goto error;

	if (RT_UNLIKELY(sizeof(rt_un) != sizeof(size_t))) goto error;
	/* socklen_t is signed under Windows and unsigned under Linux. */
	if (RT_UNLIKELY(sizeof(rt_n32) != sizeof(socklen_t))) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_signedness(void)
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

	if (RT_UNLIKELY(rt_n16_variable > 0)) goto error;
	if (RT_UNLIKELY(rt_n32_variable > 0)) goto error;
	if (RT_UNLIKELY(rt_n64_variable > 0)) goto error;
	if (RT_UNLIKELY(rt_n_variable > 0)) goto error;

#ifdef RT_DEFINE_GCC
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
	if (RT_UNLIKELY(rt_uchar8_variable < 0)) goto error;
	if (RT_UNLIKELY(rt_un16_variable < 0)) goto error;
	if (RT_UNLIKELY(rt_uchar_variable < 0)) goto error;
	if (RT_UNLIKELY(rt_un32_variable < 0)) goto error;
	if (RT_UNLIKELY(rt_un64_variable < 0)) goto error;
	if (RT_UNLIKELY(rt_un_variable < 0)) goto error;
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

static rt_s rt_check_thread(void)
{
	rt_s ret;
	struct rt_thread thread;
#ifdef RT_DEFINE_WINDOWS
	HANDLE ref_thread;
#else
	pthread_t ref_thread;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(sizeof(thread.thread_handle) != sizeof(ref_thread))) goto error;
	if (RT_UNLIKELY(sizeof(thread) != sizeof(HANDLE))) goto error;
#else
	if (RT_UNLIKELY(sizeof(thread.thread_pointer) != sizeof(ref_thread))) goto error;
	if (RT_UNLIKELY(sizeof(thread.thread_pointer) != sizeof(pthread_t))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_chrono(void)
{
	rt_s ret;
	struct rt_chrono chrono;
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER ref_chrono;
#else
	struct timespec ref_chrono;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(sizeof(chrono.start_counter) != sizeof(ref_chrono))) goto error;
	if (RT_UNLIKELY(sizeof(chrono) != sizeof(LARGE_INTEGER))) goto error;
#else
	if (RT_UNLIKELY(sizeof(chrono.seconds) != sizeof(ref_chrono.tv_sec))) goto error;
	if (RT_UNLIKELY(sizeof(chrono.nano_seconds) != sizeof(ref_chrono.tv_nsec))) goto error;
	if (RT_UNLIKELY(sizeof(chrono) != sizeof(struct timespec))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_check_critical_section(void)
{
	rt_s ret;
	struct rt_critical_section critical_section;
#ifdef RT_DEFINE_WINDOWS
	CRITICAL_SECTION ref_critical_section;
#else
	pthread_mutex_t ref_critical_section;
#endif

#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(sizeof(critical_section.debug_info) != sizeof(ref_critical_section.DebugInfo))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section.lock_count) != sizeof(ref_critical_section.LockCount))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section.recursion_count) != sizeof(ref_critical_section.RecursionCount))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section.owning_thread_handle) != sizeof(ref_critical_section.OwningThread))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section.lock_semaphore_handle) != sizeof(ref_critical_section.LockSemaphore))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section.spin_count) != sizeof(ref_critical_section.SpinCount))) goto error;

	if (RT_UNLIKELY(sizeof(struct rt_critical_section) != sizeof(CRITICAL_SECTION))) goto error;
#else
	/* __SIZEOF_PTHREAD_MUTEX_T from /usr/include/bits/pthreadtypes.h. */
	if (RT_UNLIKELY(sizeof(critical_section.data) != sizeof(ref_critical_section))) goto error;
	if (RT_UNLIKELY(sizeof(critical_section) != sizeof(pthread_mutex_t))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_check_rpr(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_check_flags())) goto error;
	if (RT_UNLIKELY(!rt_check_types())) goto error;
	if (RT_UNLIKELY(!rt_check_signedness())) goto error;
	if (RT_UNLIKELY(!rt_check_thread())) goto error;
	if (RT_UNLIKELY(!rt_check_chrono())) goto error;
	if (RT_UNLIKELY(!rt_check_critical_section())) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
