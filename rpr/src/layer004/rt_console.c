#include "layer004/rt_console.h"

#include "layer001/rt_os_headers.h"
#include "layer003/rt_char.h"

rt_s rt_console_write_string(const rt_char *string)
{
	return rt_console_write_with_size(string, rt_char_get_size(string), RT_FALSE);
}

rt_s rt_console_write_string_with_size(const rt_char *string, rt_un size)
{
	return rt_console_write_with_size(string, size, RT_FALSE);
}

rt_s rt_console_write_error(const rt_char *error)
{
	return rt_console_write_with_size(error, rt_char_get_size(error), RT_TRUE);
}

rt_s rt_console_write_error_with_size(const rt_char *error, rt_un size)
{
	return rt_console_write_with_size(error, size, RT_TRUE);
}

rt_s rt_console_write(const rt_char *message, rt_b error)
{
	return rt_console_write_with_size(message, rt_char_get_size(message), error);
}

rt_s rt_console_write_with_size(const rt_char *message, rt_un size, rt_b error)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD written;
	rt_char8 oem_text[RT_CHAR_BIG_STRING_SIZE];
	HANDLE stream_handle;
#else
	int file_descriptor;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	/* Translate אשיט characters... Never fails if arguments are different. */
	/* TODO: Allocate a buffer from the heap if necessary. */
	CharToOemBuff(message, oem_text, (DWORD)size);

	if (error) {
		/* TODO: Check GetStdHandle result. */
		stream_handle = GetStdHandle(STD_ERROR_HANDLE);
	} else {
		/* TODO: Check GetStdHandle result. */
		stream_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	/* WriteConsoleA must not be used as it does not manage redirection to a file. */
	/* WriteFile returns zero and set last error in case of failure. */
	ret = WriteFile(stream_handle, oem_text, (DWORD)size, &written, NULL);
#else
	if (error)
		file_descriptor = 2;
	else
		file_descriptor = 1;

	/* write return -1 and set errno in case of failure. */
	ret = (write(file_descriptor, message, (size_t)size) != -1);
#endif
	return ret;
}
