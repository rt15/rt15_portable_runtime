#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_manual_test_console_read_file(const rt_char *test_resources_dir, const rt_char *file_name, enum rt_encoding encoding)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_char buffer[RT_CHAR_QUARTER_BIG_STRING_SIZE];
	rt_un buffer_size;
	struct rt_encoding_info encoding_info;
	rt_s ret;

	file_path_size = rt_char_get_size(test_resources_dir);
	if (!rt_char_copy(test_resources_dir, file_path_size, file_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;
	if (!rt_char_append(file_name, rt_char_get_size(file_name), file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;
	if (!zz_read_text_file(file_path, encoding, buffer, RT_CHAR_QUARTER_BIG_STRING_SIZE, &buffer_size)) goto error;

	if (!rt_encoding_get_info(encoding, &encoding_info)) goto error;
	if (!rt_console_write_string(encoding_info.name)) goto error;
	if (!rt_console_write_string(_R(" file: OOOO"))) goto error;
	if (!rt_console_write_string(buffer)) goto error;
	if (!rt_console_write_string(_R("OOOO\n"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_manual_test_read_line()
{
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_char message[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un message_size;
	rt_s ret;

	if (!rt_console_write_string(_R("Please type a line:\n"))) goto error;

	if (!rt_console_read_line(buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size)) goto error;
	if (rt_char_get_size(buffer) != buffer_size) goto error;

	message_size = 1;
	if (!rt_char_copy(_R("\""), message_size, message, RT_CHAR_HALF_BIG_STRING_SIZE)) goto error;
	if (!rt_char_append(buffer, buffer_size, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size)) goto error;
	if (!rt_char_append(_R("\"\n"), 2, message, RT_CHAR_HALF_BIG_STRING_SIZE, &message_size)) goto error;
	if (!rt_console_write_string_with_size(message, message_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_console()
{
	rt_char test_resources_dir[RT_FILE_PATH_SIZE];
	rt_un test_resources_dir_size;
	rt_s ret;

	if (!zz_get_test_resources_dir(test_resources_dir, RT_FILE_PATH_SIZE, &test_resources_dir_size))
		goto error;

	if (!zz_manual_test_console_read_file(test_resources_dir, _R("latin1.txt"), RT_ENCODING_ISO_8859_15)) goto error;
	if (!zz_manual_test_console_read_file(test_resources_dir, _R("utf8.txt"), RT_ENCODING_UTF_8)) goto error;
	if (!rt_console_write_string(_R("Accented character in source file: OOOOÈOOOO\n"))) goto error;
	if (!zz_manual_test_read_line()) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
