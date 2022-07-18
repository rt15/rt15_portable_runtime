#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_empty_dir(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char empty_dir[RT_FILE_PATH_SIZE];
	rt_un empty_dir_size;
	rt_un64 file_size;
	rt_char sub_dir[RT_FILE_PATH_SIZE];
	rt_un sub_dir_size;
	enum rt_file_path_type type;
	rt_s ret;

	/* Create an empty_dir directory path. */
	if (!rt_char_copy(tmp_dir, tmp_dir_size, empty_dir, RT_FILE_PATH_SIZE)) goto error;
	empty_dir_size = tmp_dir_size;
	if (!rt_file_path_append_separator(empty_dir, RT_FILE_PATH_SIZE, &empty_dir_size)) goto error;
	if (!rt_char_append(_R("empty_dir"), 9, empty_dir, RT_FILE_PATH_SIZE, &empty_dir_size)) goto error;

	/* Create a sub directory path. */
	if (!rt_char_copy(empty_dir, empty_dir_size, sub_dir, RT_FILE_PATH_SIZE)) goto error;
	sub_dir_size = empty_dir_size;
	if (!rt_file_path_append_separator(sub_dir, RT_FILE_PATH_SIZE, &sub_dir_size)) goto error;
	if (!rt_char_append(_R("sub"), 3, sub_dir, RT_FILE_PATH_SIZE, &sub_dir_size)) goto error;

	/* Clean possible directory from previous test. */
	if (!rt_file_system_delete_dir_if_exists(sub_dir)) goto error;
	if (!rt_file_system_delete_dir_if_exists(empty_dir)) goto error;
	if (!rt_file_path_get_type(empty_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;

	/* Attempt to delete non-existing directory. */
	if (rt_file_system_delete_dir(empty_dir)) goto error;
	if (!rt_file_system_delete_dir_if_exists(empty_dir)) goto error;

	/* Attempt to create the directory twice. */
	if (!rt_file_system_create_dir(empty_dir)) goto error;
	if (rt_file_system_create_dir(empty_dir)) goto error;

	/* Size of a directory should be zero. */
	if (!rt_file_system_get_file_size(empty_dir, &file_size)) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (file_size) goto error;
#else
	if (!file_size || file_size > 200) goto error;
#endif

	/* Delete files functions should not work. */
	if (rt_file_system_delete_file(empty_dir)) goto error;
	if (rt_file_system_delete_file_if_exists(empty_dir)) goto error;

	/* Remove directory with delete_dir. */
	if (!rt_file_path_get_type(empty_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;
	if (!rt_file_system_delete_dir(empty_dir)) goto error;
	if (!rt_file_path_get_type(empty_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;

	/* Remove directory with delete_dir_if_exists. */
	if (!rt_file_system_create_dir(empty_dir)) goto error;
	if (!rt_file_path_get_type(empty_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;
	if (!rt_file_system_delete_dir_if_exists(empty_dir)) goto error;
	if (!rt_file_path_get_type(empty_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;

	/* Create 2 directories at once. */
	if (!rt_file_system_create_dirs(sub_dir)) goto error;
	if (!rt_file_system_create_dirs(sub_dir)) goto error;
	if (!rt_file_path_get_type(sub_dir, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_empty_file(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char empty_file[RT_FILE_PATH_SIZE];
	rt_un empty_file_size;
	rt_un64 file_size;
	enum rt_file_path_type type;
	rt_s ret;

	/* Create an empty_file file path. */
	if (!rt_char_copy(tmp_dir, tmp_dir_size, empty_file, RT_FILE_PATH_SIZE)) goto error;
	empty_file_size = tmp_dir_size;
	if (!rt_file_path_append_separator(empty_file, RT_FILE_PATH_SIZE, &empty_file_size)) goto error;
	if (!rt_char_append(_R("empty_file.txt"), 14, empty_file, RT_FILE_PATH_SIZE, &empty_file_size)) goto error;

	/* Make sure the file does not exist. */
	if (!rt_file_system_delete_file_if_exists(empty_file)) goto error;
	if (!rt_file_path_get_type(empty_file, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;
	if (rt_file_system_get_file_size(empty_file, &file_size)) goto error;

	/* Create the file with truncate as false. */
	if (!rt_file_system_create_empty_file(empty_file, RT_FALSE)) goto error;
	if (!rt_file_path_get_type(empty_file, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;
	if (!rt_file_system_get_file_size(empty_file, &file_size)) goto error;
	if (file_size) goto error;

	/* Fail to re-create the file as truncate is false. */
	if (rt_file_system_create_empty_file(empty_file, RT_FALSE)) goto error;

	/* Re-create the file. */
	if (!rt_file_system_create_empty_file(empty_file, RT_TRUE)) goto error;

	/* Delete directories functions should not work. */
	if (rt_file_system_delete_dir(empty_file)) goto error;
	if (rt_file_system_delete_dir_if_exists(empty_file)) goto error;

	/* Delete the file. */
	if (!rt_file_system_delete_file(empty_file)) goto error;
	if (!rt_file_path_get_type(empty_file, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;

	/* Fail to delete non-existing file. */
	if (rt_file_system_delete_file(empty_file)) goto error;

	/* Should not fail even if the file does not exist. */
	if (!rt_file_system_delete_file_if_exists(empty_file)) goto error;

	/* Create the file with truncate as true. */
	if (!rt_file_system_create_empty_file(empty_file, RT_TRUE)) goto error;
	if (!rt_file_path_get_type(empty_file, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_file_system()
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret;

	if (!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size)) goto error;

	if (!zz_test_empty_dir(tmp_dir, tmp_dir_size)) goto error;

	if (!zz_test_empty_file(tmp_dir, tmp_dir_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
