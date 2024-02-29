#include "layer002/rt_error.h"

#include "layer001/rt_os_headers.h"

/* The count of errors types defined in rt_eror.h. */
#define RT_ERROR_ERRORS_COUNT 12

#ifdef RT_DEFINE_WINDOWS
static const rt_un32 rt_error_os_errors[RT_ERROR_ERRORS_COUNT] = { ERROR_SUCCESS,
								   ERROR_INSUFFICIENT_BUFFER,
								   ERROR_BAD_ARGUMENTS,
								   ERROR_NOT_ENOUGH_MEMORY,
								   ERROR_FILE_EXISTS,
								   ERROR_ALREADY_EXISTS,
								   ERROR_INVALID_FUNCTION,
								   ERROR_ARITHMETIC_OVERFLOW,
								   ERROR_IO_PENDING,
								   WSAEWOULDBLOCK,
								   ERROR_FILE_NOT_FOUND,
								   ERROR_PATH_NOT_FOUND
								 };
#else
static const rt_n32 rt_error_os_errors[RT_ERROR_ERRORS_COUNT] = { 0,
								  ERANGE,
								  EINVAL,
								  ENOMEM,
								  EEXIST,
								  EEXIST,
								  EPERM,
								  EOVERFLOW,
								  EWOULDBLOCK,
								  EWOULDBLOCK,
								  ENOENT,
								  ENOENT
								};
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

rt_b rt_error_would_block(void)
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

void rt_error_set_last_from_errno(rt_n32 errno_value)
{
	switch (errno_value) {
	case 0:
		rt_error_set_last(RT_ERROR_SUCCESS);
		break;
	case ERANGE:
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		break;
	case EINVAL:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		break;
	case ENOMEM:
		rt_error_set_last(RT_ERROR_NOT_ENOUGH_MEMORY);
		break;
	case EEXIST:
		rt_error_set_last(RT_ERROR_ALREADY_EXISTS);
		break;
	case EOVERFLOW:
		rt_error_set_last(RT_ERROR_ARITHMETIC_OVERFLOW);
		break;
	case EWOULDBLOCK:
		rt_error_set_last(RT_ERROR_WOULD_BLOCK);
		break;
	case ENOENT:
		rt_error_set_last(RT_ERROR_FILE_NOT_FOUND);
		break;
	default:
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		break;
	}
}
