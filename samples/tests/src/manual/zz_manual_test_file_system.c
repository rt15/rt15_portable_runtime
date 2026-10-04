#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_manual_test_file_system_get_modification_time(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_n modification_time;
	struct rt_time_info time_info;
	rt_char buffer[RT_CHAR_HALF_BIG_STRING_SIZE];
	rt_un buffer_size;
	rt_s ret = RT_FAILED;

	file_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("manual_file_info.txt"), 20, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;

	/* Write the file, replacing possible file from previous test. */
	if (RT_UNLIKELY(!rt_small_file_write(file_path, RT_SMALL_FILE_MODE_TRUNCATE, "Hello, world!", 13))) goto end;

	/* Get modification date and time. */
	if (RT_UNLIKELY(!rt_file_system_get_modification_time(file_path, &modification_time))) goto end;

	if (RT_UNLIKELY(!rt_time_info_create_local(&time_info, modification_time)))
		goto end;

	buffer_size = 25;
	if (RT_UNLIKELY(!rt_char_copy(_R("File modification time = "), buffer_size, buffer, RT_CHAR_HALF_BIG_STRING_SIZE))) goto end;
	if (RT_UNLIKELY(!zz_append_time_info(&time_info, RT_NULL, buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append_char(_R('\n'), buffer, RT_CHAR_HALF_BIG_STRING_SIZE, &buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_console_write_str_with_size(buffer, buffer_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s zz_manual_test_file_system(void)
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size))) goto end;

	if (RT_UNLIKELY(!zz_manual_test_file_system_get_modification_time(tmp_dir, tmp_dir_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}
