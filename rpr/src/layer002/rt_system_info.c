#include "layer002/rt_system_info.h"

#include "layer001/rt_os_headers.h"

rt_s rt_system_info_get_logical_cpu_count(rt_un *logical_cpu_count)
{
#ifdef RT_DEFINE_WINDOWS
	SYSTEM_INFO system_info;
#else
	long result;
	rt_s ret = RT_FAILED;
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
		goto end;

	*logical_cpu_count = (rt_un)result;

	ret = RT_OK;
end:
	return ret;
#endif
}
