#include <rpr.h>

static rt_s zz_test_get_current_dir()
{
	rt_char current_dir[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_string(_R("Current directory = \"")))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_current_dir(current_dir, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(current_dir, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string(_R("\"\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_executable_path()
{
	rt_char executable_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_string(_R("Executable path = \"")))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_executable_path(executable_path, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(executable_path, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string(_R("\"\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_temp_dir()
{
	rt_char temp_dir[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_string(_R("Temp directory = \"")))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_temp_dir(temp_dir, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(temp_dir, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string(_R("\"\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_application_data_dir()
{
	rt_char application_data_dir[RT_FILE_PATH_SIZE];
	rt_un application_data_dir_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_string(_R("Application data directory = \"")))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_application_data_dir(_R("CodeBlocks"), application_data_dir, RT_FILE_PATH_SIZE, &application_data_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(application_data_dir, application_data_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string(_R("\"\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_home_dir()
{
	rt_char home_dir[RT_FILE_PATH_SIZE];
	rt_un home_dir_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_console_write_string(_R("Home directory = \"")))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_home_dir(home_dir, RT_FILE_PATH_SIZE, &home_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string_with_size(home_dir, home_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_console_write_string(_R("\"\n")))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_manual_test_file_path()
{
	rt_s ret;

	if (RT_UNLIKELY(!zz_test_get_current_dir())) goto error;
	if (RT_UNLIKELY(!zz_test_get_executable_path())) goto error;
	if (RT_UNLIKELY(!zz_test_get_temp_dir())) goto error;
	if (RT_UNLIKELY(!zz_test_get_application_data_dir())) goto error;
	if (RT_UNLIKELY(!zz_test_get_home_dir())) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
