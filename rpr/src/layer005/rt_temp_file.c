#include "layer005/rt_temp_file.h"

#include "layer001/rt_os_headers.h"
#include "layer003/rt_char.h"
#include "layer004/rt_file_path.h"

rt_s rt_temp_file_create(struct rt_file *file, const rt_char *prefix, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_char temp_dir[RT_FILE_PATH_SIZE];
	rt_un temp_dir_size;
	rt_s ret = RT_FAILED;

	if (RT_UNLIKELY(!rt_file_path_get_temp_dir(temp_dir, RT_FILE_PATH_SIZE, &temp_dir_size)))
		goto end;

	if (RT_UNLIKELY(!rt_temp_file_create_with_parent_path(file, prefix, temp_dir, temp_dir_size, buffer, buffer_capacity, buffer_size)))
		goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_temp_file_create_with_parent_path(struct rt_file *file, const rt_char *prefix, const rt_char *parent_path, RT_WINDOWS_UNUSED rt_un parent_path_size, rt_char *buffer, RT_WINDOWS_UNUSED rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	/* GetTempFileName create an empty file. */
	/* Returns zero and sets last error in case of issue. */
	/* Size of buffer must be at least MAX_PATH . */
	if (RT_UNLIKELY(!GetTempFileName(parent_path, prefix, 0, buffer)))
		goto end;

	if (RT_UNLIKELY(!rt_file_create(file, buffer, RT_FILE_MODE_TRUNCATE)))
		goto end;

	*buffer_size = rt_char_get_size(buffer);
#else
	*buffer_size = parent_path_size;
	if (RT_UNLIKELY(!rt_char_copy(parent_path, parent_path_size, buffer, buffer_capacity))) goto end;
	if (RT_UNLIKELY(!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(prefix, rt_char_get_size(prefix), buffer, buffer_capacity, buffer_size))) goto end;
	if (RT_UNLIKELY(!rt_char_append(_R("XXXXXX"), 6, buffer, buffer_capacity, buffer_size))) goto end;

	/* Returns -1 and sets errno in case of error. */
	file->io_device.file_descriptor = mkostemp(buffer, O_CLOEXEC);
	if (RT_UNLIKELY(file->io_device.file_descriptor == -1))
		goto end;
#endif

	ret = RT_OK;
end:
	return ret;
}
