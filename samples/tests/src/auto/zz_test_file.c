#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_write_simple_file(struct rt_file *file, const rt_char8 *str)
{
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_io_device_write(&file->io_device.output_stream, str, rt_char8_get_size(str)))) goto end;

	/* Check rt_io_device_kernel_flush. */
	if (RT_UNLIKELY(!rt_io_device_kernel_flush(&file->io_device))) goto end;

	ret = RT_OK;
end:
	return ret;
}

static rt_s zz_test_no_existing_file(const rt_char *file_path, enum rt_file_mode mode)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(file_path))) goto end;

	if (RT_UNLIKELY(!rt_file_create(&file, file_path, mode))) goto end;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!zz_write_simple_file(&file, "Hello, world!"))) goto end;

	file_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_io_device_free(&file.io_device))) goto end;

	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, world!"))) goto end;

	ret = RT_OK;
end:
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}

	return ret;
}

static rt_s zz_test_simple_file(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	struct rt_file file;
	rt_b file_created = RT_FALSE;
	rt_b is_console;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_un64 file_size;
	rt_un64 offset;
	rt_s ret = RT_FAILED;

	/* Create file path to simple_file.txt. */
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, tmp_dir_size, file_path, RT_FILE_PATH_SIZE))) goto end;
	file_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("simple_file.txt"), 15, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto end;

	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(file_path))) goto end;

	/* RT_FILE_MODE_READ, no existing file. */
	if (RT_UNLIKELY(rt_file_create(&file, file_path, RT_FILE_MODE_READ))) goto end;

	/* RT_FILE_MODE_NEW, no existing file. */
	if (RT_UNLIKELY(!zz_test_no_existing_file(file_path, RT_FILE_MODE_NEW))) goto end;

	/* RT_FILE_MODE_NEW, existing file. */
	if (RT_UNLIKELY(rt_file_create(&file, file_path, RT_FILE_MODE_NEW))) goto end;

	/* RT_FILE_MODE_READ_WRITE, no existing file. */
	if (RT_UNLIKELY(!zz_test_no_existing_file(file_path, RT_FILE_MODE_READ_WRITE))) goto end;

	/* RT_FILE_MODE_READ_WRITE, existing file. */
	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_READ_WRITE))) goto end;
	file_created = RT_TRUE;

	/* Check rt_io_device_is_console. */
	if (RT_UNLIKELY(!rt_io_device_is_console(&file.io_device, &is_console))) goto end;
	if (RT_UNLIKELY(is_console)) goto end;

	if (RT_UNLIKELY(!rt_file_get_pointer(&file, &offset))) goto end;
	if (RT_UNLIKELY(offset != 0)) goto end;

	if (RT_UNLIKELY(!rt_file_get_size(&file, &file_size))) goto end;
	if (RT_UNLIKELY(file_size != 13)) goto end;

	if (RT_UNLIKELY(!rt_file_get_pointer(&file, &offset))) goto end;
	if (RT_UNLIKELY(offset != 0)) goto end;

	if (RT_UNLIKELY(!rt_file_set_pointer(&file, 0, RT_FILE_POSITION_END))) goto end;
	if (RT_UNLIKELY(!zz_write_simple_file(&file, " Hello!"))) goto end;

	if (RT_UNLIKELY(!rt_file_get_pointer(&file, &offset))) goto end;
	if (RT_UNLIKELY(offset != 20)) goto end;

	if (RT_UNLIKELY(!rt_file_set_pointer(&file, 10, RT_FILE_POSITION_BEGIN))) goto end;
	if (RT_UNLIKELY(!rt_file_set_pointer(&file, -3, RT_FILE_POSITION_CURRENT))) goto end;
	if (RT_UNLIKELY(!rt_file_get_pointer(&file, &offset))) goto end;
	if (RT_UNLIKELY(offset != 7)) goto end;
	if (RT_UNLIKELY(!zz_write_simple_file(&file, "W"))) goto end;

	file_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_io_device_free(&file.io_device))) goto end;

	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, World! Hello!"))) goto end;

	/* RT_FILE_MODE_TRUNCATE, no existing file. */
	if (RT_UNLIKELY(!zz_test_no_existing_file(file_path, RT_FILE_MODE_TRUNCATE))) goto end;

	/* RT_FILE_MODE_TRUNCATE, existing file. */
	if (RT_UNLIKELY(!rt_file_create(&file, file_path, RT_FILE_MODE_TRUNCATE))) goto end;
	file_created = RT_TRUE;

	if (RT_UNLIKELY(!rt_file_get_size(&file, &file_size))) goto end;
	if (RT_UNLIKELY(file_size != 0)) goto end;
	if (RT_UNLIKELY(!rt_file_set_pointer(&file, 0, RT_FILE_POSITION_END))) goto end;
	if (RT_UNLIKELY(!zz_write_simple_file(&file, "Hello, world!"))) goto end;

	file_created = RT_FALSE;
	if (RT_UNLIKELY(!rt_io_device_free(&file.io_device))) goto end;

	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, world!"))) goto end;

	ret = RT_OK;
end:
	if (file_created) {
		if (RT_UNLIKELY(!rt_io_device_free(&file.io_device)))
			ret = RT_FAILED;
	}
	return ret;
}

rt_s zz_test_file(void)
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size))) goto end;

	if (RT_UNLIKELY(!zz_test_simple_file(tmp_dir, tmp_dir_size))) goto end;

	ret = RT_OK;
end:
	return ret;
}
