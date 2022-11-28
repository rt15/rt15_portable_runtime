#include "layer005/rt_console.h"

#include "layer001/rt_memory.h"
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

#ifdef RT_DEFINE_WINDOWS

static rt_s rt_console_read_line_from_file(rt_h std_input_handle, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char8 local_buffer[RT_CHAR8_BIG_STRING_SIZE];
	void *heap_buffer = RT_NULL;
	rt_un heap_buffer_capacity = 0;
	rt_char8 *read_buffer;
	rt_un read_buffer_size;
	rt_char8 read_character;
	DWORD number_of_chars_read;
	rt_s ret;

	/* Allocate a temporary buffer for the file content before using OemToCharBuff. */
	if (!rt_static_heap_alloc_if_needed(local_buffer, RT_CHAR8_BIG_STRING_SIZE, &heap_buffer, &heap_buffer_capacity, (void**)&read_buffer, buffer_capacity))
		goto error;

	/* Makes sure that the read buffer is fully initialized as we will fully convert it because of OemToCharBuff. */
	RT_MEMORY_ZERO(read_buffer, buffer_capacity);

	read_buffer_size = 0;
	while (read_buffer_size < buffer_capacity - 1) {
		/* ReadFile returns zero and set last error in case of issue. */
		if (!ReadFile(std_input_handle, &read_character, 1, &number_of_chars_read, RT_NULL))
			goto error;

		/* If end of line or end of file. */
		if (read_character == '\n' || !number_of_chars_read) {
			break;
		} else if (read_character != '\r') {
			read_buffer[read_buffer_size] = read_character;
			read_buffer_size++;
		}
	}

	/* If the buffer was not big enough. */
	if (read_buffer_size == buffer_capacity - 1) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	/* OemToCharBuff returns zero and set last error in case of issue. */
	if (!OemToCharBuff(read_buffer, buffer, (DWORD)buffer_capacity))
		goto error;

	*buffer_size = rt_char_get_size(buffer);

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (!rt_static_heap_free(&heap_buffer) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_console_read_line(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_h std_input_handle;
	DWORD old_mode;
	rt_b restore_old_mode = RT_FALSE;
	DWORD number_of_chars_read;
	rt_s ret;

	if (buffer_capacity < 2) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	std_input_handle = GetStdHandle(STD_INPUT_HANDLE);

	/* GetStdHandle returns INVALID_HANDLE_VALUE and set last error in case of issue. */
	if (std_input_handle == INVALID_HANDLE_VALUE)
		goto error;

	/* GetConsoleMode returns zero in case of issue. It fails if the input handle is not a console. */
	if (GetConsoleMode(std_input_handle, &old_mode)) {

		/* SetConsoleMode returns zero and set last error in case of issue. */
		if (!SetConsoleMode(std_input_handle, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT))
			goto error;
		restore_old_mode = RT_TRUE;

		/* ReadConsole returns zero and set last error in case of issue. */
		if (!ReadConsole(std_input_handle, buffer, (DWORD)buffer_capacity, &number_of_chars_read, RT_NULL))
			goto error;

		/* If the buffer was too small. */
		if (number_of_chars_read == buffer_capacity && buffer[number_of_chars_read - 1] != _R('\n')) {
			/* Keep reading until LF is found to discard characters (FlushConsoleInputBuffer cannot be used with ReadConsole). */
			do {
				if (!ReadConsole(std_input_handle, buffer, (DWORD)buffer_capacity, &number_of_chars_read, RT_NULL))
					goto error;
			} while (buffer[number_of_chars_read - 1] != _R('\n'));

			/* The buffer was too small. */
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;

		} else {
			/* Skip CR/LF. */
			number_of_chars_read -= 2;
			/* Terminating zero character. */
			buffer[number_of_chars_read] = 0;

			*buffer_size = number_of_chars_read;
		}
	} else {
		if (!rt_console_read_line_from_file(std_input_handle, buffer, buffer_capacity, buffer_size))
			goto error;
	}

	ret = RT_OK;
free:
	if (restore_old_mode) {
		restore_old_mode = RT_FALSE;
		/* SetConsoleMode returns zero and set last error in case of issue. */
		if (!SetConsoleMode(std_input_handle, old_mode) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#else

rt_s rt_console_read_line(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	int character;
	rt_un size;
	rt_s ret;

	size = 0;
	do {
		if (size >= buffer_capacity) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}

		character = getchar();
		if (character == EOF || character == '\n') {
			character = 0;
		}
		buffer[size] = character;
		size++;
	} while (character);

	/* Does not count terminating zero. */
	*buffer_size = size - 1;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif
