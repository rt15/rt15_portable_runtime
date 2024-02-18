#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_manual_test_console_io_device(void)
{
	struct rt_io_device console_input;
	rt_b console_input_created = RT_FALSE;
	struct rt_input_stream *input_stream;
	struct rt_io_device console_output;
	rt_b console_output_created = RT_FALSE;
	struct rt_output_stream *output_stream;
	rt_b is_console;
	rt_char8 buffer[200];
	rt_un bytes_read;
	rt_s ret;

	if (!rt_io_device_create_from_console_input(&console_input)) {
		if (RT_UNLIKELY(!rt_console_write_str(_R("Failed to create console I/O device (normal in case of gdb).\n"))))
			goto error;
	} else {
		console_input_created = RT_TRUE;

		input_stream = &console_input.input_stream;

		if (RT_UNLIKELY(!rt_io_device_create_from_console_output(&console_output))) goto error;
		console_output_created = RT_TRUE;

		output_stream = &console_output.output_stream;

		if (RT_UNLIKELY(!rt_io_device_is_console(&console_input, &is_console))) goto error;
		if (RT_UNLIKELY(!is_console)) goto error;

		if (RT_UNLIKELY(!rt_io_device_is_console(&console_output, &is_console))) goto error;
		if (RT_UNLIKELY(!is_console)) goto error;

		if (RT_UNLIKELY(!rt_console_write_str(_R("Reading from console io device:\n")))) goto error;
		if (RT_UNLIKELY(!input_stream->read(input_stream, buffer, 200, &bytes_read))) goto error;
		rt_char8_trim(RT_FALSE, RT_TRUE, buffer, &bytes_read);
		if (RT_UNLIKELY(!rt_console_write_str(_R("Output into console io device: \"")))) goto error;
		if (RT_UNLIKELY(!output_stream->write(output_stream, buffer, bytes_read))) goto error;
		if (RT_UNLIKELY(!rt_console_write_str(_R("\"\n")))) goto error;

		/* Check rt_io_device_kernel_flush. */
		if (RT_UNLIKELY(!rt_io_device_kernel_flush(&console_output))) goto error;
	}

	ret = RT_OK;
free:
	if (console_output_created) {
		console_output_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&console_output) && ret))
			goto error;
	}
	if (console_input_created) {
		console_input_created = RT_FALSE;
		if (RT_UNLIKELY(!rt_io_device_free(&console_input) && ret))
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_console_read_file(const rt_char *test_resources_dir, const rt_char *file_name, enum rt_encoding encoding)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	struct rt_encoding_info encoding_info;
	rt_s ret;

	file_path_size = rt_char_get_size(test_resources_dir);
	if (RT_UNLIKELY(!rt_char_copy(test_resources_dir, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(file_name, rt_char_get_size(file_name), file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!zz_read_text_file(file_path, encoding, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size))) goto error;

	if (RT_UNLIKELY(!rt_encoding_get_info(encoding, &encoding_info))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(encoding_info.name))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R(" file: OOOO")))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(buffer))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("OOOO\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_read_line(void)
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Please type a line:\n")))) goto error;

	if (RT_UNLIKELY(!rt_console_read_line(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(rt_char_get_size(buffer) != buffer_size)) goto error;

	message_size = 1;
	if (RT_UNLIKELY(!rt_char_copy(_R("\""), message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append(buffer, buffer_size, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("\"\n"), 2, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(message, message_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_read_char(void)
{
	rt_char character;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Please type a character:\n")))) goto error;

	if (RT_UNLIKELY(!rt_console_read_char(&character))) goto error;

	message_size = 1;
	if (RT_UNLIKELY(!rt_char_copy(_R("\""), message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(character, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("\" ("), 3, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_n(character, 10, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R(")\n"), 2, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(message, message_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_set_color(void)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BLACK))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Black\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BLUE))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Blue\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_GREEN))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Green\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_CYAN))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Cyan\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_RED))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Red\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_MAGENTA))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Magenta\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_YELLOW))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Yellow\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_WHITE))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("White\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_BLACK))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright black\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_BLUE))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright blue\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_GREEN))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright green\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_CYAN))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright cyan\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_RED))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright red\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_MAGENTA))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright magenta\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_YELLOW))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright yellow\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_set_color(RT_CONSOLE_COLOR_BRIGHT_WHITE))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("Bright white\n")))) goto error;
	if (RT_UNLIKELY(!rt_console_reset_color())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_console(void)
{
	rt_char test_resources_dir[RT_FILE_PATH_SIZE];
	rt_un test_resources_dir_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_get_test_resources_dir(test_resources_dir, RT_FILE_PATH_SIZE, &test_resources_dir_size)))
		goto error;

	if (RT_UNLIKELY(!zz_manual_test_console_io_device())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_console_read_file(test_resources_dir, _R("latin1.txt"), RT_ENCODING_ISO_8859_15))) goto error;
	if (RT_UNLIKELY(!zz_manual_test_console_read_file(test_resources_dir, _R("utf8.txt"), RT_ENCODING_UTF_8))) goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Accented character in source file: OOOOÈOOOO\n")))) goto error;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Writing nothing: OOOO")))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("")))) goto error;
	if (RT_UNLIKELY(!rt_console_write_str(_R("OOOO\n")))) goto error;

	if (RT_UNLIKELY(!zz_manual_test_read_line())) goto error;
	if (RT_UNLIKELY(!zz_manual_test_read_char())) goto error;

	if (RT_UNLIKELY(!zz_manual_test_set_color())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
