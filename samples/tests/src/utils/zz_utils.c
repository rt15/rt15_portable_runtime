#include "zz_utils.h"

rt_s zz_get_tmp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_executable_path(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get bin directory. */
	if (!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get tests directory. */
	if (!rt_file_path_get_parent(buffer, buffer_capacity, buffer_size)) goto error;

	/* Get tmp directory. */
	if (!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size)) goto error;
	if (!rt_char_append(_R("tmp"), 3, buffer, buffer_capacity, buffer_size)) goto error;

	/* Checking the directory. */
	if (!rt_file_path_get_type(buffer, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_create_file(const rt_char *file_path, const rt_char8 *str)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_s ret;

	if (!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE)) goto error;
	file_created = RT_TRUE;
	if (!rt_io_device_write(&file.io_device.output_stream, str, rt_char8_get_size(str))) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_check_file_content(const rt_char *file_path, const rt_char8 *expected)
{
	rt_un64 file_size;
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	enum rt_file_path_type type;
	rt_char8 buffer[RT_CHAR_BIG_STRING_SIZE];
	rt_un bytes_read;
	rt_s ret;

	if (!rt_file_path_get_type(file_path, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	if (!rt_file_system_get_file_size(file_path, &file_size)) goto error;
	if (file_size != rt_char8_get_size(expected)) goto error;

	if (!rt_file_create(&file, file_path, RT_FILE_MODE_READ)) goto error;
	file_created = RT_TRUE;

	if (!rt_io_device_read(&file.io_device.input_stream, buffer, RT_CHAR_BIG_STRING_SIZE, &bytes_read)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(expected), expected, bytes_read)) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret)
			goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_b zz_char_equals_with_nulls(const rt_char *string1, const rt_char *string2)
{
	rt_b ret;

	if (!string1) {
		ret = (!string2);
	} else {
		if (!string2) {
			ret = RT_FALSE;
		} else {
			while (!(ret = *(rt_uchar*)string1 - *(rt_uchar*)string2) && *string2)
			{
				string1++;
				string2++;
			}
			ret = !ret;
		}
	}

	return ret;
}
