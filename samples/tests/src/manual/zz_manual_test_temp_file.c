#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_manual_test_temp_file_do(const rt_char *tmp_dir, rt_un tmp_dir_size, rt_b with_parent_path)
{
	struct rt_file file1;
	rt_b file1_created = RT_FALSE;
	rt_b delete_file1 = RT_FALSE;
	rt_char file1_path[RT_FILE_PATH_SIZE];
	rt_un file1_path_size;
	struct rt_file file2;
	rt_b file2_created = RT_FALSE;
	rt_b delete_file2 = RT_FALSE;
	rt_char file2_path[RT_FILE_PATH_SIZE];
	rt_un file2_path_size;
	enum rt_file_path_type type;
	rt_s ret = RT_FAILED;

	if (!with_parent_path) {
		if (RT_UNLIKELY(!rt_temp_file_create(&file1, _R("prefix"), file1_path, RT_FILE_PATH_SIZE, &file1_path_size)))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_temp_file_create_with_parent_path(&file1, _R("prefix"), tmp_dir, tmp_dir_size, file1_path, RT_FILE_PATH_SIZE, &file1_path_size)))
			goto end;
	}
	file1_created = RT_TRUE;
	delete_file1 = RT_TRUE;

	if (RT_UNLIKELY(rt_char_get_size(file1_path) != file1_path_size)) goto end;
	if (RT_UNLIKELY(!rt_file_path_get_type(file1_path, &type))) goto end;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_FILE)) goto end;

	if (RT_UNLIKELY(!rt_console_write_str(file1_path)))
		goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R("\n"))))
		goto end;

	if (!with_parent_path) {
		if (RT_UNLIKELY(!rt_temp_file_create(&file2, _R("prefix"), file2_path, RT_FILE_PATH_SIZE, &file2_path_size)))
			goto end;
	} else {
		if (RT_UNLIKELY(!rt_temp_file_create_with_parent_path(&file2, _R("prefix"), tmp_dir, tmp_dir_size, file2_path, RT_FILE_PATH_SIZE, &file2_path_size)))
			goto end;
	}
	file2_created = RT_TRUE;
	delete_file2 = RT_TRUE;

	if (RT_UNLIKELY(rt_char_get_size(file2_path) != file2_path_size)) goto end;
	if (RT_UNLIKELY(!rt_file_path_get_type(file2_path, &type))) goto end;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_FILE)) goto end;

	if (RT_UNLIKELY(!rt_console_write_str(file2_path)))
		goto end;
	if (RT_UNLIKELY(!rt_console_write_str(_R("\n"))))
		goto end;

	ret = RT_OK;
end:
	if (file2_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file2.io_device)))
			ret = RT_FAILED;
	}
	if (delete_file2) {
		if (RT_UNLIKELY(!rt_file_system_delete_file(file2_path)))
			ret = RT_FAILED;
	}
	if (file1_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file1.io_device)))
			ret = RT_FAILED;
	}
	if (delete_file1) {
		if (RT_UNLIKELY(!rt_file_system_delete_file(file1_path)))
			ret = RT_FAILED;
	}

	return ret;
}

rt_s zz_manual_test_temp_file(void)
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size))) goto end;

	if (RT_UNLIKELY(!rt_console_write_str(_R("Temp files:\n")))) goto end;

	if (RT_UNLIKELY(!zz_manual_test_temp_file_do(tmp_dir, tmp_dir_size, RT_FALSE))) goto end;
	if (RT_UNLIKELY(!zz_manual_test_temp_file_do(tmp_dir, tmp_dir_size, RT_TRUE))) goto end;

	ret = RT_OK;
end:
	return ret;
}
