#include "layer006/rt_error_message.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer005/rt_console.h"

#ifdef RT_DEFINE_WINDOWS

rt_s rt_error_message_append(rt_un32 error_number, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un message_size;
	rt_s ret = RT_FAILED;

	/* FormatMessage returns the number of characters written into the buffer, not counting the terminating zero. */
	/* FormatMessage returns zero and sets last error in case of error. */
	message_size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				     NULL, error_number,
				     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				     &buffer[*buffer_size], (DWORD)(buffer_capacity - *buffer_size), NULL);

	if (RT_UNLIKELY(!message_size))
		goto end;

	/* Remove trailing end of lines. */
	*buffer_size += message_size;
	rt_char_trim(RT_FALSE, RT_TRUE, buffer, buffer_size);

	ret = RT_OK;
end:
	return ret;
}

#else

static rt_s rt_error_message_append(rt_n32 error_number, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char *message;
	rt_s ret = RT_FAILED;

	/* strerror_r is the thread safe version of strerror. */
	message = strerror_r(error_number, &buffer[*buffer_size], buffer_capacity - *buffer_size);
	if (RT_UNLIKELY(!message)) {
		/* Some standards says that message cannot be RT_NULL, some others not. */
		goto end;
	}
	/* In the GNU strerror_r, the buffer might not be really used. */
	if (RT_UNLIKELY(!rt_char_append(message, rt_char_get_size(message), buffer, buffer_capacity, buffer_size)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

#endif

rt_s rt_error_message_append_last(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	return rt_error_message_append(GetLastError(), buffer, buffer_capacity, buffer_size);
#else
	return rt_error_message_append(errno, buffer, buffer_capacity, buffer_size);
#endif
}

rt_s rt_error_message_write_last(const rt_char *prefix)
{
	rt_char buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	if (prefix) {
		buffer_size = rt_char_get_size(prefix);
		if (RT_UNLIKELY(!rt_char_copy(prefix, buffer_size, buffer, RT_CHAR_BIG_STRING_SIZE)))
			goto end;
	} else {
		buffer_size = 0;
	}
	if (RT_UNLIKELY(!rt_error_message_append_last(buffer, RT_CHAR_BIG_STRING_SIZE , &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_error_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}
