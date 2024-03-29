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
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, tmp_dir_size, empty_dir, RT_FILE_PATH_SIZE))) goto error;
	empty_dir_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(empty_dir, RT_FILE_PATH_SIZE, &empty_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("empty_dir"), 9, empty_dir, RT_FILE_PATH_SIZE, &empty_dir_size))) goto error;

	/* Create a sub directory path. */
	if (RT_UNLIKELY(!rt_char_copy(empty_dir, empty_dir_size, sub_dir, RT_FILE_PATH_SIZE))) goto error;
	sub_dir_size = empty_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(sub_dir, RT_FILE_PATH_SIZE, &sub_dir_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub"), 3, sub_dir, RT_FILE_PATH_SIZE, &sub_dir_size))) goto error;

	/* Clean possible directory from previous test. */
	if (RT_UNLIKELY(!rt_file_system_delete_dir_if_exists(sub_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_dir_if_exists(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Attempt to delete non-existing directory. */
	if (RT_UNLIKELY(rt_file_system_delete_dir(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_dir_if_exists(empty_dir))) goto error;

	/* Attempt to create the directory twice. */
	if (RT_UNLIKELY(!rt_file_system_create_dir(empty_dir))) goto error;
	if (RT_UNLIKELY(rt_file_system_create_dir(empty_dir))) goto error;

	/* Check rt_file_system_create_dir_if_not_exists. */
	if (RT_UNLIKELY(!rt_file_system_delete_dir(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dir_if_not_exists(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dir_if_not_exists(empty_dir))) goto error;

	/* Size of a directory should be zero. */
	if (RT_UNLIKELY(!rt_file_system_get_file_size(empty_dir, &file_size))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (RT_UNLIKELY(file_size)) goto error;
#else
	if (RT_UNLIKELY(!file_size || file_size > 4096)) goto error;
#endif

	/* Delete files functions should not work. */
	if (RT_UNLIKELY(rt_file_system_delete_file(empty_dir))) goto error;
	if (RT_UNLIKELY(rt_file_system_delete_file_if_exists(empty_dir))) goto error;

	/* Remove directory with delete_dir. */
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_dir(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Remove directory with delete_dir_if_exists. */
	if (RT_UNLIKELY(!rt_file_system_create_dir(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_dir_if_exists(empty_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Create 2 directories at once. */
	if (RT_UNLIKELY(!rt_file_system_create_dirs(sub_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dirs(sub_dir))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(sub_dir, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;

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
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, tmp_dir_size, empty_file, RT_FILE_PATH_SIZE))) goto error;
	empty_file_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_file_path_append_separator(empty_file, RT_FILE_PATH_SIZE, &empty_file_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("empty_file.txt"), 14, empty_file, RT_FILE_PATH_SIZE, &empty_file_size))) goto error;

	/* Make sure the file does not exist. */
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(empty_file))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_file, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;
	if (RT_UNLIKELY(rt_file_system_get_file_size(empty_file, &file_size))) goto error;

	/* Create the file with truncate as false. */
	if (RT_UNLIKELY(!rt_file_system_create_empty_file(empty_file, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_file, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_FILE)) goto error;
	if (RT_UNLIKELY(!rt_file_system_get_file_size(empty_file, &file_size))) goto error;
	if (RT_UNLIKELY(file_size)) goto error;

	/* Fail to re-create the file as truncate is false. */
	if (RT_UNLIKELY(rt_file_system_create_empty_file(empty_file, RT_FALSE))) goto error;

	/* Re-create the file. */
	if (RT_UNLIKELY(!rt_file_system_create_empty_file(empty_file, RT_TRUE))) goto error;

	/* Delete directories functions should not work. */
	if (RT_UNLIKELY(rt_file_system_delete_dir(empty_file))) goto error;
	if (RT_UNLIKELY(rt_file_system_delete_dir_if_exists(empty_file))) goto error;

	/* Delete the file. */
	if (RT_UNLIKELY(!rt_file_system_delete_file(empty_file))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_file, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Fail to delete non-existing file. */
	if (RT_UNLIKELY(rt_file_system_delete_file(empty_file))) goto error;

	/* Should not fail even if the file does not exist. */
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(empty_file))) goto error;

	/* Create the file with truncate as true. */
	if (RT_UNLIKELY(!rt_file_system_create_empty_file(empty_file, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_get_type(empty_file, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_FILE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(source_file_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(destination_file_path))) goto error;

	/* Non-existing source file. */
	if (RT_UNLIKELY(rt_file_system_copy_file(source_file_path, destination_file_path, RT_FALSE))) goto error;

	if (RT_UNLIKELY(!zz_create_file(source_file_path, "Hello, world!"))) goto error;

	/* Copy and check the files. */
	if (RT_UNLIKELY(!rt_file_system_copy_file(source_file_path, destination_file_path, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(source_file_path, "Hello, world!"))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	/* Existing destination file, without overwrite. */
	if (RT_UNLIKELY(!rt_file_system_delete_file(source_file_path))) goto error;
	if (RT_UNLIKELY(!zz_create_file(source_file_path, "New content"))) goto error;
	if (RT_UNLIKELY(rt_file_system_copy_file(source_file_path, destination_file_path, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	/* Existing destination file, with overwrite. */
	if (RT_UNLIKELY(!rt_file_system_copy_file(source_file_path, destination_file_path, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "New content"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_move_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(source_file_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(destination_file_path))) goto error;

	/* Non-existing source file. */
	if (RT_UNLIKELY(rt_file_system_move_file(source_file_path, destination_file_path))) goto error;

	if (RT_UNLIKELY(!zz_create_file(source_file_path, "Hello, world!"))) goto error;

	/* Move and check the file. */
	if (RT_UNLIKELY(!rt_file_system_move_file(source_file_path, destination_file_path))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	/* Source file should no more exist. */
	if (RT_UNLIKELY(!rt_file_path_get_type(source_file_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Existing destination file. */
	if (RT_UNLIKELY(!zz_create_file(source_file_path, "New content"))) goto error;
	if (RT_UNLIKELY(rt_file_system_move_file(source_file_path, destination_file_path))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_rename_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(source_file_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_delete_file_if_exists(destination_file_path))) goto error;

	/* Non-existing source file. */
	if (RT_UNLIKELY(rt_file_system_rename_file(source_file_path, _R("destination.txt")))) goto error;

	if (RT_UNLIKELY(!zz_create_file(source_file_path, "Hello, world!"))) goto error;

	/* Rename and check the file. */
	if (RT_UNLIKELY(!rt_file_system_rename_file(source_file_path, _R("destination.txt")))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	/* Source file should no more exist. */
	if (RT_UNLIKELY(!rt_file_path_get_type(source_file_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_NONE)) goto error;

	/* Existing destination file. */
	if (RT_UNLIKELY(!zz_create_file(source_file_path, "New content"))) goto error;
	if (RT_UNLIKELY(rt_file_system_rename_file(source_file_path, _R("destination.txt")))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(destination_file_path, "Hello, world!"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_move_rename_file(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char source_file_path[RT_FILE_PATH_SIZE];
	rt_un source_file_path_size;
	rt_char destination_file_path[RT_FILE_PATH_SIZE];
	rt_un destination_file_path_size;
	rt_s ret;

	source_file_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, source_file_path_size, source_file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(source_file_path, RT_FILE_PATH_SIZE, &source_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("source.txt"), 10, source_file_path, RT_FILE_PATH_SIZE, &source_file_path_size))) goto error;

	destination_file_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, destination_file_path_size, destination_file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("destination.txt"), 15, destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_test_copy_file(source_file_path, destination_file_path))) goto error;
	if (RT_UNLIKELY(!zz_test_move_file(source_file_path, destination_file_path))) goto error;
	if (RT_UNLIKELY(!zz_test_rename_file(source_file_path, destination_file_path))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_prepare_dir(const rt_char *root_dir_path, rt_un root_dir_path_size)
{
	rt_char dir_path[RT_FILE_PATH_SIZE];
	rt_un dir_path_size;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_system_delete_dir_recursively(root_dir_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dir(root_dir_path))) goto error;

	dir_path_size = root_dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(root_dir_path, dir_path_size, dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub_dir1"), 8, dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_create_dir(dir_path))) goto error;

	file_path_size = dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(dir_path, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("file1.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_create_file(file_path, "Hello, world!1"))) goto error;

	if (RT_UNLIKELY(!rt_file_path_append_separator(dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub_dir2"), 8, dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_create_dir(dir_path))) goto error;

	file_path_size = dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(dir_path, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("file2.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_create_file(file_path, "Hello, world!2"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_check_dir(const rt_char *root_dir_path, rt_un root_dir_path_size)
{
	enum rt_file_path_type type;
	rt_char dir_path[RT_FILE_PATH_SIZE];
	rt_un dir_path_size;
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret;

	if (RT_UNLIKELY(!rt_file_path_get_type(root_dir_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;

	dir_path_size = root_dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(root_dir_path, dir_path_size, dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub_dir1"), 8, dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_path_get_type(dir_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;

	file_path_size = dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(dir_path, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("file1.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, world!1"))) goto error;

	if (RT_UNLIKELY(!rt_file_path_append_separator(dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub_dir2"), 8, dir_path, RT_FILE_PATH_SIZE, &dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_path_get_type(dir_path, &type))) goto error;
	if (RT_UNLIKELY(type != RT_FILE_PATH_TYPE_DIR)) goto error;

	file_path_size = dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(dir_path, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("file2.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, world!2"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_rename_dir(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char source_dir_path[RT_FILE_PATH_SIZE];
	rt_un source_dir_path_size;
	rt_char destination_dir_path[RT_FILE_PATH_SIZE];
	rt_un destination_dir_path_size;
	rt_s ret;

	source_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, source_dir_path_size, source_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("dir"), 3, source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;

	if (RT_UNLIKELY(!zz_prepare_dir(source_dir_path, source_dir_path_size))) goto error;

	destination_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, destination_dir_path_size, destination_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("new_name"), 8, destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_delete_dir_recursively(destination_dir_path))) goto error;

	if (RT_UNLIKELY(!rt_file_system_rename_dir(source_dir_path, _R("new_name")))) goto error;

	if (RT_UNLIKELY(!zz_check_dir(destination_dir_path, destination_dir_path_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_move_dir(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char source_dir_path[RT_FILE_PATH_SIZE];
	rt_un source_dir_path_size;
	rt_char destination_dir_path[RT_FILE_PATH_SIZE];
	rt_un destination_dir_path_size;
	rt_s ret;

	source_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, source_dir_path_size, source_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("dir1"), 4, source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;

	if (RT_UNLIKELY(!zz_prepare_dir(source_dir_path, source_dir_path_size))) goto error;

	destination_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, destination_dir_path_size, destination_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("dest_dir"), 8, destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_delete_dir_recursively(destination_dir_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dir(destination_dir_path))) goto error;

	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("dir2"), 4, destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;

	if (RT_UNLIKELY(rt_file_system_move_dir(source_dir_path, tmp_dir))) goto error;

	if (RT_UNLIKELY(!rt_file_system_move_dir(source_dir_path, destination_dir_path))) goto error;

	if (RT_UNLIKELY(!zz_check_dir(destination_dir_path, destination_dir_path_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_dir_with_source_and_destination(const rt_char *source_dir_path, rt_un source_dir_path_size, const rt_char *destination_dir_path, rt_un destination_dir_path_size)
{
	rt_char file_path[RT_FILE_PATH_SIZE];
	rt_un file_path_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_prepare_dir(source_dir_path, source_dir_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_delete_dir_recursively(destination_dir_path))) goto error;
	if (RT_UNLIKELY(!rt_file_system_create_dir(destination_dir_path))) goto error;

	if (RT_UNLIKELY(!rt_file_system_copy_dir(source_dir_path, destination_dir_path, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_check_dir(destination_dir_path, destination_dir_path_size))) goto error;

	file_path_size = destination_dir_path_size;
	if (RT_UNLIKELY(!rt_char_copy(destination_dir_path, file_path_size, file_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("sub_dir1"), 8, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("file1.txt"), 9, file_path, RT_FILE_PATH_SIZE, &file_path_size))) goto error;

	if (RT_UNLIKELY(!rt_file_system_delete_file(file_path))) goto error;
	if (RT_UNLIKELY(!zz_create_file(file_path, "new content"))) goto error;

	/* Copy again, should fail. */
	if (RT_UNLIKELY(rt_file_system_copy_dir(source_dir_path, destination_dir_path, RT_FALSE))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "new content"))) goto error;

	/* Copy with overwrite. */
	if (RT_UNLIKELY(!rt_file_system_copy_dir(source_dir_path, destination_dir_path, RT_TRUE))) goto error;
	if (RT_UNLIKELY(!zz_check_file_content(file_path, "Hello, world!1"))) goto error;

	if (RT_UNLIKELY(!zz_check_dir(destination_dir_path, destination_dir_path_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_copy_dir(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char source_dir_path[RT_FILE_PATH_SIZE];
	rt_un source_dir_path_size;
	rt_char destination_dir_path[RT_FILE_PATH_SIZE];
	rt_un destination_dir_path_size;
	rt_s ret;

	source_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, source_dir_path_size, source_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("copy_source"), 11, source_dir_path, RT_FILE_PATH_SIZE, &source_dir_path_size))) goto error;

	destination_dir_path_size = tmp_dir_size;
	if (RT_UNLIKELY(!rt_char_copy(tmp_dir, destination_dir_path_size, destination_dir_path, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(_R("copy_destination"), 16, destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size))) goto error;

	if (RT_UNLIKELY(!zz_test_copy_dir_with_source_and_destination(source_dir_path, source_dir_path_size, destination_dir_path, destination_dir_path_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_file_system(void)
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret;

	if (RT_UNLIKELY(!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size))) goto error;

	if (RT_UNLIKELY(!zz_test_empty_dir(tmp_dir, tmp_dir_size))) goto error;
	if (RT_UNLIKELY(!zz_test_empty_file(tmp_dir, tmp_dir_size))) goto error;
	if (RT_UNLIKELY(!zz_test_copy_move_rename_file(tmp_dir, tmp_dir_size))) goto error;
	if (RT_UNLIKELY(!zz_test_rename_dir(tmp_dir, tmp_dir_size))) goto error;
	if (RT_UNLIKELY(!zz_test_move_dir(tmp_dir, tmp_dir_size))) goto error;
	if (RT_UNLIKELY(!zz_test_copy_dir(tmp_dir, tmp_dir_size))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
