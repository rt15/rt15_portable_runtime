#include "layer002/rt_error.h"

#include "layer001/rt_os_headers.h"

/* The count of errors types defined in rt_Error.h. */
#define RT_ERROR_ERRORS_COUNT 9

#ifdef RT_DEFINE_WINDOWS
const rt_un32 rt_error_os_errors[RT_ERROR_ERRORS_COUNT] = { ERROR_SUCCESS, ERROR_INSUFFICIENT_BUFFER, ERROR_BAD_ARGUMENTS, ERROR_NOT_ENOUGH_MEMORY, ERROR_ALREADY_EXISTS, ERROR_INVALID_FUNCTION, ERROR_ARITHMETIC_OVERFLOW, ERROR_IO_PENDING, WSAEWOULDBLOCK };
#else
const rt_n32 rt_error_os_errors[RT_ERROR_ERRORS_COUNT] = { 0, ERANGE, EINVAL, ENOMEM, EEXIST, EPERM, EOVERFLOW, EWOULDBLOCK, EWOULDBLOCK };
#endif

void rt_error_set_last(enum rt_error error)
{
	rt_un right_error;

	if (error < RT_ERROR_ERRORS_COUNT)
		right_error = error;
	else
		right_error = RT_ERROR_BAD_ARGUMENTS;

#ifdef RT_DEFINE_WINDOWS
	SetLastError(rt_error_os_errors[right_error]);
#else
	errno = rt_error_os_errors[right_error];
#endif
}

rt_b error_would_block()
{
#ifdef RT_DEFINE_WINDOWS
	DWORD error;
#endif
	rt_b ret;

#ifdef RT_DEFINE_WINDOWS
	error = GetLastError();
	ret = ((error == WSAEWOULDBLOCK) || (error == ERROR_IO_PENDING));
#else
	ret = (errno == EWOULDBLOCK);
#endif
	return ret;
}
