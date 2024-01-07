#include "layer002/rt_get_process_id.h"

#include "layer001/rt_os_headers.h"

#ifdef RT_DEFINE_WINDOWS

rt_s rt_get_process_id(rt_un *process_id)
{
	DWORD local_process_id;

	/* GetCurrentProcessId cannot fail. */
	local_process_id = GetCurrentProcessId();

	*process_id = local_process_id;

	return RT_OK;
}

#else

rt_s rt_get_process_id(rt_un *process_id)
{
	pid_t local_process_id;

	/* getpid is always successful. */
	local_process_id = getpid();

	*process_id = local_process_id;

	return RT_OK;
}

#endif
