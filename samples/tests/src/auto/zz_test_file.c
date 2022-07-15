#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_check_simple_file(const rt_char *file_path, const rt_char8 *expected)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	enum rt_file_path_type type;
	rt_char8 buffer[256];
	rt_un bytes_read;
	rt_s ret;

	if (!rt_file_path_get_type(file_path, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	/* RT_FILE_MODE_READ, existing file. */
	if (!rt_file_create(&file, file_path, RT_FILE_MODE_READ)) goto error;
	file_created = RT_TRUE;

	if (!rt_io_device_read(&file.io_device.input_stream, buffer, 256, &bytes_read)) goto error;
	if (!rt_char8_equals(buffer, rt_char8_get_size(expected), expected, bytes_read)) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret) goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_write_simple_file(struct rt_file *file, const rt_char8 *string)
{
	rt_s ret;

	if (!rt_io_device_write(&file->io_device.output_stream, string, rt_char8_get_size(string))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_no_existing_file(const rt_char *file_path, enum rt_file_mode mode)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_s ret;

	if (!rt_file_system_delete_file_if_exists(file_path)) goto error;

	if (!rt_file_create(&file, file_path, mode)) goto error;
	file_created = RT_TRUE;

	if (!zz_write_simple_file(&file, "Hello, world!")) goto error;

	file_created = RT_FALSE;
	if (!rt_io_device_free(&file.io_device)) goto error;

	if (!zz_check_simple_file(file_path, "Hello, world!")) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret) goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_simple_file(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_un64 file_size;
	rt_un64 offset;
	rt_s ret;

	/* Create file path to simple_file.txt. */
	if (!rt_char_copy(tmp_dir, tmp_dir_size, file_path, RT_FILE_PATH_SIZE)) goto error;
	file_path_size = tmp_dir_size;
	if (!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;
	if (!rt_char_append(_R("simple_file.txt"), 15, file_path, RT_FILE_PATH_SIZE, &file_path_size)) goto error;

	if (!rt_file_system_delete_file_if_exists(file_path)) goto error;

	/* RT_FILE_MODE_READ, no existing file. */
	if (rt_file_create(&file, file_path, RT_FILE_MODE_READ)) goto error;

	/* RT_FILE_MODE_NEW, no existing file. */
	if (!zz_test_no_existing_file(file_path, RT_FILE_MODE_NEW)) goto error;

	/* RT_FILE_MODE_NEW, existing file. */
	if (rt_file_create(&file, file_path, RT_FILE_MODE_NEW)) goto error;

	/* RT_FILE_MODE_READ_WRITE, no existing file. */
	if (!zz_test_no_existing_file(file_path, RT_FILE_MODE_READ_WRITE)) goto error;

	/* RT_FILE_MODE_READ_WRITE, existing file. */
	if (!rt_file_create(&file, file_path, RT_FILE_MODE_READ_WRITE)) goto error;
	file_created = RT_TRUE;

	if (!rt_file_get_pointer(&file, &offset)) goto error;
	if (offset != 0) goto error;

	if (!rt_file_get_size(&file, &file_size)) goto error;
	if (file_size != 13) goto error;

	if (!rt_file_get_pointer(&file, &offset)) goto error;
	if (offset != 0) goto error;

	if (!rt_file_set_pointer(&file, 0, RT_FILE_POSITION_END)) goto error;
	if (!zz_write_simple_file(&file, " Hello!")) goto error;

	if (!rt_file_get_pointer(&file, &offset)) goto error;
	if (offset != 20) goto error;

	if (!rt_file_set_pointer(&file, 10, RT_FILE_POSITION_BEGIN)) goto error;
	if (!rt_file_set_pointer(&file, -3, RT_FILE_POSITION_CURRENT)) goto error;
	if (!rt_file_get_pointer(&file, &offset)) goto error;
	if (offset != 7) goto error;
	if (!zz_write_simple_file(&file, "W")) goto error;

	file_created = RT_FALSE;
	if (!rt_io_device_free(&file.io_device)) goto error;

	if (!zz_check_simple_file(file_path, "Hello, World! Hello!")) goto error;

	/* RT_FILE_MODE_TRUNCATE, no existing file. */
	if (!zz_test_no_existing_file(file_path, RT_FILE_MODE_TRUNCATE)) goto error;

	/* RT_FILE_MODE_TRUNCATE, existing file. */
	if (!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE)) goto error;
	file_created = RT_TRUE;

	if (!rt_file_get_size(&file, &file_size)) goto error;
	if (file_size != 0) goto error;
	if (!rt_file_set_pointer(&file, 0, RT_FILE_POSITION_END)) goto error;
	if (!zz_write_simple_file(&file, "Hello, world!")) goto error;

	file_created = RT_FALSE;
	if (!rt_io_device_free(&file.io_device)) goto error;

	if (!zz_check_simple_file(file_path, "Hello, world!")) goto error;

	ret = RT_OK;
free:
	if (file_created) {
		file_created = RT_FALSE;
		if (!rt_io_device_free(&file.io_device) && ret) goto error;
	}
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_file()
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret;

	if (!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size)) goto error;

	if (!zz_test_simple_file(tmp_dir, tmp_dir_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
