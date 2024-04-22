#include "layer002/rt_system_info.h"

#include "layer001/rt_os_headers.h"

rt_s rt_system_info_get_logical_cpu_count(rt_un *logical_cpu_count)
{
#ifdef RT_DEFINE_WINDOWS
	SYSTEM_INFO system_info;
#else
	long result;
	rt_s ret;
#endif

#ifdef RT_DEFINE_WINDOWS

	/* GetSystemInfo cannot fail. */
	GetSystemInfo(&system_info);
	*logical_cpu_count = system_info.dwNumberOfProcessors;
	return RT_OK;

#else

	/* sysconf returns -1 and sets errno in case of issue. */
	result = sysconf(_SC_NPROCESSORS_ONLN);
	if (result == -1)
		goto error;

	*logical_cpu_count = (rt_un)result;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}
