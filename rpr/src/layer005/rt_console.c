#include "layer005/rt_console.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"
#include "layer004/rt_static_heap.h"

rt_s rt_console_write_string(const rt_char *str)
{
	return rt_console_write_with_size(str, rt_char_get_size(str), RT_FALSE);
}

rt_s rt_console_write_string_with_size(const rt_char *str, rt_un size)
{
	return rt_console_write_with_size(str, size, RT_FALSE);
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
	UINT console_code_page;
	HANDLE stream_handle;
	rt_char8 stack_buffer[RT_CHAR8_BIG_STRING_SIZE];
	int oem_text_size;
	void* heap_buffer = RT_NULL;
	rt_char8 *oem_text;
	DWORD written;
	rt_s ret;

	if (size) {
		/* GetConsoleOutputCP returns zero and set last error in case of issue. */
		console_code_page = GetConsoleOutputCP();
		if (!console_code_page)
			goto error;

		if (error) {
			stream_handle = GetStdHandle(STD_ERROR_HANDLE);
		} else {
			stream_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		/* GetStdHandle returns INVALID_HANDLE_VALUE and set last error in case of issue. */
		if (stream_handle == INVALID_HANDLE_VALUE)
			goto error;

		/* Trying with the buffer from the stack. */
		/* WideCharToMultiByte does not write a terminating zero if an input size is provided. */
		/* Returns zero and set last error in case of issue. */
		oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, stack_buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL);
		if (!oem_text_size) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				goto error;
			} else {
				/* Stack buffer is too small. Asking for the required size. */
				/* WideCharToMultiByte does not write a terminating zero if an input size is provided. */
				/* Returns zero and set last error in case of issue. */
				oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, RT_NULL, 0, RT_NULL, RT_NULL);
				if (!oem_text_size)
					goto error;

				if (!rt_static_heap_alloc((void**)&heap_buffer, oem_text_size))
					goto error;

				oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, heap_buffer, oem_text_size, RT_NULL, RT_NULL);
				if (!oem_text_size)
					goto error;

				oem_text = heap_buffer;
			}
		} else {
			oem_text = stack_buffer;
		}

		/* WriteConsoleA must not be used as it does not manage redirection to a file. */
		/* WriteFile returns zero and set last error in case of failure. */
		if (!WriteFile(stream_handle, oem_text, (DWORD)oem_text_size, &written, NULL))
			goto error;
	}

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!rt_static_heap_free((void**)&heap_buffer) && ret)
			goto error;
	}

	return ret;

error:
	ret = RT_FAILED;
	goto free;
#else
	int file_descriptor;
	rt_s ret;

	if (size) {
		if (error)
			file_descriptor = 2;
		else
			file_descriptor = 1;

		/* write return -1 and set errno in case of failure. */
		ret = (write(file_descriptor, message, (size_t)size) != -1);
	} else {
		ret = RT_OK;
	}

	return ret;
#endif
}
