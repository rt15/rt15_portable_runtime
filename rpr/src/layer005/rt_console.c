#include "layer005/rt_console.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_char8.h"
#include "layer004/rt_static_heap.h"

rt_s rt_console_write_str(const rt_char *str)
{
	return rt_console_write_with_size(str, rt_char_get_size(str), RT_FALSE);
}

rt_s rt_console_write_str_with_size(const rt_char *str, rt_un size)
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
	void *heap_buffer = RT_NULL;
	rt_char8 *oem_text;
	DWORD written;
	rt_s ret;

	if (size) {
		/* GetConsoleOutputCP returns zero and sets last error in case of issue. */
		console_code_page = GetConsoleOutputCP();
		if (RT_UNLIKELY(!console_code_page))
			goto error;

		if (error) {
			stream_handle = GetStdHandle(STD_ERROR_HANDLE);
		} else {
			stream_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		/* GetStdHandle returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
		if (RT_UNLIKELY(stream_handle == INVALID_HANDLE_VALUE))
			goto error;

		/* Trying with the buffer from the stack. */
		/* WideCharToMultiByte does not write a terminating zero if an input size is provided. */
		/* Returns zero and sets last error in case of issue. */
		oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, stack_buffer, RT_CHAR8_BIG_STRING_SIZE, RT_NULL, RT_NULL);
		if (!oem_text_size) {
			if (RT_UNLIKELY(GetLastError() != ERROR_INSUFFICIENT_BUFFER)) {
				goto error;
			} else {
				/* Stack buffer is too small. Asking for the required size. */
				/* WideCharToMultiByte does not write a terminating zero if an input size is provided. */
				/* Returns zero and sets last error in case of issue. */
				oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, RT_NULL, 0, RT_NULL, RT_NULL);
				if (RT_UNLIKELY(!oem_text_size))
					goto error;

				if (RT_UNLIKELY(!rt_static_heap_alloc((void**)&heap_buffer, oem_text_size)))
					goto error;

				oem_text_size = WideCharToMultiByte(console_code_page, 0, message, (int)size, heap_buffer, oem_text_size, RT_NULL, RT_NULL);
				if (RT_UNLIKELY(!oem_text_size))
					goto error;

				oem_text = heap_buffer;
			}
		} else {
			oem_text = stack_buffer;
		}

		/* WriteConsoleA must not be used as it does not manage redirection to a file. */
		/* WriteFile returns zero and sets last error in case of failure. */
		if (RT_UNLIKELY(!WriteFile(stream_handle, oem_text, (DWORD)oem_text_size, &written, NULL)))
			goto error;
	}

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (RT_UNLIKELY(!rt_static_heap_free((void**)&heap_buffer) && ret))
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

		/* write returns -1 and sets errno in case of failure. */
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
	if (RT_UNLIKELY(!rt_static_heap_alloc_if_needed(local_buffer, RT_CHAR8_BIG_STRING_SIZE, &heap_buffer, &heap_buffer_capacity, (void**)&read_buffer, buffer_capacity)))
		goto error;

	/* Makes sure that the read buffer is fully initialized as we will fully convert it because of OemToCharBuff. */
	RT_MEMORY_ZERO(read_buffer, buffer_capacity);

	read_buffer_size = 0;
	while (read_buffer_size < buffer_capacity - 1) {
		/* ReadFile returns zero and sets last error in case of issue. */
		if (RT_UNLIKELY(!ReadFile(std_input_handle, &read_character, 1, &number_of_chars_read, RT_NULL)))
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
	if (RT_UNLIKELY(read_buffer_size == buffer_capacity - 1)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	/* OemToCharBuff returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!OemToCharBuff(read_buffer, buffer, (DWORD)buffer_capacity)))
		goto error;

	*buffer_size = rt_char_get_size(buffer);

	ret = RT_OK;
free:
	if (heap_buffer) {
		if (RT_UNLIKELY(!rt_static_heap_free(&heap_buffer) && ret))
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

	if (RT_UNLIKELY(buffer_capacity < 2)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	std_input_handle = GetStdHandle(STD_INPUT_HANDLE);

	/* GetStdHandle returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
	if (RT_UNLIKELY(std_input_handle == INVALID_HANDLE_VALUE))
		goto error;

	/* GetConsoleMode returns zero in case of issue. It fails if the input handle is not a console. */
	if (GetConsoleMode(std_input_handle, &old_mode)) {

		/* SetConsoleMode returns zero and sets last error in case of issue. */
		if (RT_UNLIKELY(!SetConsoleMode(std_input_handle, ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)))
			goto error;
		restore_old_mode = RT_TRUE;

		/* ReadConsole returns zero and sets last error in case of issue. */
		if (RT_UNLIKELY(!ReadConsole(std_input_handle, buffer, (DWORD)buffer_capacity, &number_of_chars_read, RT_NULL)))
			goto error;

		/* If the buffer was too small. */
		if (RT_UNLIKELY(number_of_chars_read == buffer_capacity && buffer[number_of_chars_read - 1] != _R('\n'))) {
			/* Keep reading until LF is found to discard characters (FlushConsoleInputBuffer cannot be used with ReadConsole). */
			do {
				if (RT_UNLIKELY(!ReadConsole(std_input_handle, buffer, (DWORD)buffer_capacity, &number_of_chars_read, RT_NULL)))
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
		if (RT_UNLIKELY(!rt_console_read_line_from_file(std_input_handle, buffer, buffer_capacity, buffer_size)))
			goto error;
	}

	ret = RT_OK;
free:
	if (restore_old_mode) {
		restore_old_mode = RT_FALSE;
		/* SetConsoleMode returns zero and sets last error in case of issue. */
		if (RT_UNLIKELY(!SetConsoleMode(std_input_handle, old_mode) && ret))
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
		if (RT_UNLIKELY(size >= buffer_capacity)) {
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

#ifdef RT_DEFINE_WINDOWS

rt_s rt_console_read_char(rt_char *character)
{
	rt_h std_input_handle;
	DWORD old_mode;
	rt_b restore_old_mode = RT_FALSE;
	DWORD number_of_chars_read;
	rt_s ret;

	std_input_handle = GetStdHandle(STD_INPUT_HANDLE);

	/* GetStdHandle returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
	if (RT_UNLIKELY(std_input_handle == INVALID_HANDLE_VALUE)) goto error;

	/* GetConsoleMode returns zero in case of issue. */
	if (RT_UNLIKELY(!GetConsoleMode(std_input_handle, &old_mode))) goto error;

	/* SetConsoleMode returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!SetConsoleMode(std_input_handle, 0))) goto error;
	restore_old_mode = RT_TRUE;

	/* ReadConsole returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!ReadConsole(std_input_handle, character, 1, &number_of_chars_read, RT_NULL))) goto error;

	if (RT_UNLIKELY(!number_of_chars_read)) {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}

	ret = RT_OK;
free:
	if (restore_old_mode) {
		restore_old_mode = RT_FALSE;
		/* SetConsoleMode returns zero and sets last error in case of issue. */
		if (RT_UNLIKELY(!SetConsoleMode(std_input_handle, old_mode) && ret))
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#else

rt_s rt_console_read_char(rt_char *character)
{
	struct termios terminal_attributes;
	struct termios old_terminal_attributes;
	rt_b restore_old_terminal_attributes = RT_FALSE;
	int getchar_result;
	int file_descriptor_flags;
	rt_b restore_old_file_descriptor_flags = RT_FALSE;
	char local_character;
	rt_s ret;

	/* Retrieve the terminal attributes. */
	/* Zero is the standard input. */
	/* The tcgetattr returns non-zero and sets errno in case of error. */
	if (RT_UNLIKELY(tcgetattr(0, &terminal_attributes)))
		goto error;

	/* Backup the attributes. */
	memcpy(&old_terminal_attributes, &terminal_attributes, sizeof(old_terminal_attributes));

	/* Prepare new attributes. */
	terminal_attributes.c_lflag &= ~(ICANON | ECHO);
	terminal_attributes.c_cc[VTIME] = 0;
	terminal_attributes.c_cc[VMIN] = 1;

	/* Configure the terminal with the new attributes. */
	/* The tcsetattr returns non-zero and sets errno in case of error. */
	if (RT_UNLIKELY(tcsetattr(0, TCSANOW, &terminal_attributes)))
		goto error;
	restore_old_terminal_attributes = RT_TRUE;

	/* The getchar function returns EOF in case of error (or in case of end of file). */
	getchar_result = getchar();
	if (RT_UNLIKELY(getchar_result == EOF && !feof(stdin)))
		goto error;

	*character = getchar_result;

	/* Get current stdin file descriptor flags. */
	/* In case of error -1 is returned and errno is set. */
	file_descriptor_flags = fcntl(0, F_GETFL);
	if (RT_UNLIKELY(file_descriptor_flags == -1))
		goto error;

	/* Set stdin as non-blocking. */
	if (RT_UNLIKELY(fcntl(0, F_SETFL, file_descriptor_flags | O_NONBLOCK) == -1))
		goto error;
	restore_old_file_descriptor_flags = RT_TRUE;

	/* Discard (most of) possible trailing UTF-8 characters. */
	while (RT_TRUE) {
		local_character = getchar();
		if (local_character == EOF || local_character == _R('\n')) {
			break;
		}
	}

	ret = RT_OK;
free:

	/* Restore blocking stdin using initial flags. */
	if (restore_old_file_descriptor_flags) {
		restore_old_file_descriptor_flags = RT_FALSE;
		if (RT_UNLIKELY(fcntl(0, F_SETFL, file_descriptor_flags) == -1 && ret))
			goto error;
	}

	/* Restore old terminal attributes. */
	if (restore_old_terminal_attributes) {
		restore_old_terminal_attributes = RT_FALSE;
		if (RT_UNLIKELY(tcsetattr(0, TCSANOW, &old_terminal_attributes) && ret))
			goto error;
	}

	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif

#ifdef RT_DEFINE_WINDOWS

rt_s rt_console_clear(void)
{
	rt_h std_output_handle;
	COORD cursor_position;
	DWORD written;
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	rt_s ret;

	std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	/* GetStdHandle returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
	if (RT_UNLIKELY(std_output_handle == INVALID_HANDLE_VALUE))
		goto error;

	cursor_position.X = 0;
	cursor_position.Y = 0;

	/* Get the current console screen buffer information. */
	/* Returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!GetConsoleScreenBufferInfo(std_output_handle, &screen_buffer_info)))
		goto error;

	/* Fill the entire console screen with blank spaces. */
	/* Returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!FillConsoleOutputCharacter(std_output_handle, ' ', screen_buffer_info.dwSize.X * screen_buffer_info.dwSize.Y, cursor_position, &written)))
		goto error;

	/* Set the cursor position to the top left corner. */
	/* Returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!SetConsoleCursorPosition(std_output_handle, cursor_position)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_console_set_windows_color(rt_un windows_color)
{
	rt_h std_output_handle;
	rt_s ret;

	std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	/* GetStdHandle returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
	if (RT_UNLIKELY(std_output_handle == INVALID_HANDLE_VALUE))
		goto error;

	/* Returns zero and sets last error in case of issue. */
	if (RT_UNLIKELY(!SetConsoleTextAttribute(std_output_handle, (WORD)windows_color)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_console_set_color(enum rt_console_color console_color)
{
	return rt_console_set_windows_color(console_color);
}

rt_s rt_console_reset_color(void)
{
	return rt_console_set_windows_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

#else

rt_s rt_console_clear(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str_with_size("\033[2J", 4)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_console_set_color(enum rt_console_color console_color)
{
	char *str;
	rt_s ret;

	switch (console_color) {
	case RT_CONSOLE_COLOR_BLACK:
		str = "\033[30m";
		break;
	case RT_CONSOLE_COLOR_RED:
		str = "\033[31m";
		break;
	case RT_CONSOLE_COLOR_GREEN:
		str = "\033[32m";
		break;
	case RT_CONSOLE_COLOR_YELLOW:
		str = "\033[33m";
		break;
	case RT_CONSOLE_COLOR_BLUE:
		str = "\033[34m";
		break;
	case RT_CONSOLE_COLOR_MAGENTA:
		str = "\033[35m";
		break;
	case RT_CONSOLE_COLOR_CYAN:
		str = "\033[36m";
		break;
	case RT_CONSOLE_COLOR_WHITE:
		str = "\033[37m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_BLACK:
		str = "\033[90m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_RED:
		str = "\033[91m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_GREEN:
		str = "\033[92m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_YELLOW:
		str = "\033[93m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_BLUE:
		str = "\033[94m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_MAGENTA:
		str = "\033[95m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_CYAN:
		str = "\033[96m";
		break;
	case RT_CONSOLE_COLOR_BRIGHT_WHITE:
		str = "\033[97m";
		break;
	}

	if (RT_UNLIKELY(!rt_console_write_str_with_size(str, 5)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_console_reset_color(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str_with_size("\033[0m", 4)))
		goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif
