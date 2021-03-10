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
