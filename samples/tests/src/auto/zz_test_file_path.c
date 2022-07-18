#include <rpr.h>

#include "zz_utils.h"

static rt_s zz_test_get_current_dir()
{
	rt_char first_buffer[RT_FILE_PATH_SIZE];
	rt_un first_buffer_size;
	rt_char second_buffer[RT_FILE_PATH_SIZE];
	rt_un second_buffer_size;
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_current_dir(first_buffer, RT_FILE_PATH_SIZE, &first_buffer_size)) goto error;
	if (!first_buffer_size) goto error;
	if (first_buffer_size != rt_char_get_size(first_buffer)) goto error;

	/* Trying a buffer too small. */
	if (rt_file_path_get_current_dir(second_buffer, first_buffer_size, &second_buffer_size)) goto error;

	/* Trying exact buffer size. */
	if (!rt_file_path_get_current_dir(second_buffer, first_buffer_size + 1, &second_buffer_size)) goto error;
	if (!rt_char_equals(first_buffer, first_buffer_size, second_buffer, second_buffer_size)) goto error;

	/* Checking the directory. */
	if (!rt_file_path_get_type(first_buffer, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_set_current_dir_do(const rt_char *tmp_dir)
{
	rt_char current_dir_path[RT_FILE_PATH_SIZE];
	rt_un current_dir_path_size;
	enum rt_file_path_type type;
	rt_s ret;

	/* Trying to change to an unexisting directory. */
	if (rt_file_path_set_current_dir(_R("unknown_dir"))) goto error;

	/* Switching to tmp directory. */
	if (!rt_file_path_set_current_dir(tmp_dir)) goto error;

	/* Checking the directory. */
	if (!rt_file_path_get_current_dir(current_dir_path, RT_FILE_PATH_SIZE, &current_dir_path_size)) goto error;
	if (!rt_file_path_get_type(current_dir_path, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_DIR) goto error;

	/* The empty file should be in the current directory (tmp). */
	if (!rt_file_path_get_type(_R("empty_file.txt"), &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_set_current_dir(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char current_dir_path[RT_FILE_PATH_SIZE];
	rt_un current_dir_path_size;
	rt_char empty_file_path[RT_FILE_PATH_SIZE];
	rt_un empty_file_path_size;

	rt_s ret;

	/* Create an empty file in tmp directory. */
	empty_file_path_size = tmp_dir_size;
	if (!rt_char_copy(tmp_dir, empty_file_path_size, empty_file_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(empty_file_path, RT_FILE_PATH_SIZE, &empty_file_path_size)) goto error;
	if (!rt_char_append(_R("empty_file.txt"), 14, empty_file_path, RT_FILE_PATH_SIZE, &empty_file_path_size)) goto error;
	if (!rt_file_system_create_empty_file(empty_file_path, RT_TRUE)) goto error;

	/* Backup the current directory. */
	if (!rt_file_path_get_current_dir(current_dir_path, RT_FILE_PATH_SIZE, &current_dir_path_size)) goto error;

	if (!zz_test_set_current_dir_do(tmp_dir)) goto error;

	/* Restore initial current directory. */
	if (!rt_file_path_set_current_dir(current_dir_path)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_executable_path()
{
	rt_char first_buffer[RT_FILE_PATH_SIZE];
	rt_un first_buffer_size;
	rt_char second_buffer[RT_FILE_PATH_SIZE];
	rt_un second_buffer_size;
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_executable_path(first_buffer, RT_FILE_PATH_SIZE, &first_buffer_size)) goto error;
	if (!first_buffer_size) goto error;
	if (first_buffer_size != rt_char_get_size(first_buffer)) goto error;

	/* Trying a buffer too small. */
	if (rt_file_path_get_executable_path(second_buffer, first_buffer_size, &second_buffer_size)) goto error;

	/* Trying exact buffer size. */
	if (!rt_file_path_get_executable_path(second_buffer, first_buffer_size + 1, &second_buffer_size)) goto error;
	if (!rt_char_equals(first_buffer, first_buffer_size, second_buffer, second_buffer_size)) goto error;

	/* Checking the file. */
	if (!rt_file_path_get_type(first_buffer, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_FILE) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_append_separator_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size = rt_char_get_size(path);
	rt_s ret;

	if (!rt_char_copy(path, buffer_size, buffer, RT_FILE_PATH_SIZE))
		goto error;

	if (!rt_file_path_append_separator(buffer, RT_FILE_PATH_SIZE, &buffer_size))
		goto error;

	if (buffer_size != rt_char_get_size(expected))
		goto error;

	if (!rt_char_equals(buffer, buffer_size, expected, buffer_size))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_append_separator()
{
	rt_s ret;

	if (!zz_test_append_separator_do(_R(""), _R(""))) goto error;
#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_append_separator_do(_R("c:"), _R("c:\\"))) goto error;
	if (!zz_test_append_separator_do(_R("foo"), _R("foo\\"))) goto error;
	if (!zz_test_append_separator_do(_R("c:\\"), _R("c:\\"))) goto error;
	if (!zz_test_append_separator_do(_R("c:\\foo\\"), _R("c:\\foo\\"))) goto error;
	if (!zz_test_append_separator_do(_R("c:\\foo/"), _R("c:\\foo/"))) goto error;
	if (!zz_test_append_separator_do(_R("c:\\foo\\"), _R("c:\\foo\\"))) goto error;
#else
	if (!zz_test_append_separator_do(_R("/"), _R("/"))) goto error;
	if (!zz_test_append_separator_do(_R("/foo"), _R("/foo/"))) goto error;
	if (!zz_test_append_separator_do(_R("/foo/"), _R("/foo/"))) goto error;
	if (!zz_test_append_separator_do(_R("foo"), _R("foo/"))) goto error;
	if (!zz_test_append_separator_do(_R("foo/"), _R("foo/"))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_full_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size = rt_char_get_size(path);
	rt_s ret;

	if (!rt_char_copy(path, buffer_size, buffer, RT_FILE_PATH_SIZE))
		goto error;

	if (!rt_file_path_full(buffer, RT_FILE_PATH_SIZE, &buffer_size))
		goto error;

	if (buffer_size != rt_char_get_size(expected))
		goto error;

	if (!rt_char_equals(buffer, buffer_size, expected, buffer_size))
		goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_full()
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;
#ifdef RT_DEFINE_WINDOWS
	rt_char variable_name[8];
	rt_char path[RT_FILE_PATH_SIZE];
#endif
	if (!rt_file_path_get_current_dir(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!zz_test_full_do(buffer, buffer)) goto error;
	if (!zz_test_full_do(_R(""), buffer)) goto error;

	/* This test should fail in some corner cases, like if the current directory is "/" on Linux. */
	if (!rt_char_append_char(RT_FILE_PATH_SEPARATOR, buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(_R("foo"), 3, buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!zz_test_full_do(_R("foo"), buffer)) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_full_do(_R("c:\\"), _R("c:\\"))) goto error;
	if (!zz_test_full_do(_R("c:\\test"), _R("c:\\test"))) goto error;
	if (!zz_test_full_do(_R("c:\\test\\"), _R("c:\\test\\"))) goto error;
	if (!zz_test_full_do(_R("c:\\test\\..\\sub"), _R("c:\\sub"))) goto error;
	if (!zz_test_full_do(_R("c:\\test\\.\\sub"), _R("c:\\test\\sub"))) goto error;
	if (!zz_test_full_do(_R("\\\\"), _R("\\\\"))) goto error;

	/* Test "\foo" path ( -> c:\foo where c is the current directory drive). */
	if (!rt_file_path_get_current_dir(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (buffer_size < 2 || buffer[1] != _R(':')) goto error;
	buffer[2] = _R('\\');
	buffer[3] = _R('f');
	buffer[4] = _R('o');
	buffer[5] = _R('o');
	buffer[6] = 0;
	if (!zz_test_full_do(_R("\\foo"), buffer)) goto error;

	/* Test "\" path ( -> c:\ where c is the current directory drive). */
	buffer[3] = 0;
	if (!zz_test_full_do(_R("\\"), buffer)) goto error;

	/* Retrieve the letter of current directory. */
	if (!rt_file_path_get_current_dir(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (buffer_size < 2|| buffer[1] != _R(':')) goto error;

	/* Build the variable like the command prompt would do. */
	variable_name[0] = _R('=');
	variable_name[1] = buffer[0];
	variable_name[2] = _R(':');
	variable_name[3] = 0;
	if (!rt_env_var_set(variable_name, buffer)) goto error;

	/* Build a path like "c:foo". */
	path[0] = buffer[0];
	path[1] = _R(':');
	path[2] = _R('f');
	path[3] = _R('o');
	path[4] = _R('o');
	path[5] = 0;

	if (!rt_char_append_char(RT_FILE_PATH_SEPARATOR, buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(_R("foo"), 3, buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!zz_test_full_do(path, buffer)) goto error;
#else
	if (!zz_test_full_do(_R("/"), _R("/"))) goto error;
	if (!zz_test_full_do(_R("/test"), _R("/test"))) goto error;
	if (!zz_test_full_do(_R("/.."), _R("/"))) goto error;
	if (!zz_test_full_do(_R("//test1//test2//.."), _R("/test1"))) goto error;
	if (!zz_test_full_do(_R("//test1//test2//."), _R("/test1/test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/../test2/"), _R("/test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/.../test2/"), _R("/test1/.../test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/..../test2/"), _R("/test1/..../test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/.hidden/test2"), _R("/test1/.hidden/test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/./test2/"), _R("/test1/test2"))) goto error;
	if (!zz_test_full_do(_R("/test1/..strange/"), _R("/test1/..strange"))) goto error;
#endif
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_is_namespaced()
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (rt_file_path_is_namespaced(_R("\\"))) goto error;
	if (rt_file_path_is_namespaced(_R("\\\\"))) goto error;
	if (rt_file_path_is_namespaced(_R("\\\\."))) goto error;
	if (rt_file_path_is_namespaced(_R("\\\\.a"))) goto error;
	if (rt_file_path_is_namespaced(_R("\\\\?a"))) goto error;
#else
	if (!rt_file_path_is_namespaced(_R("\\"))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\"))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\."))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\.a"))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\?a"))) goto error;
#endif

	if (!rt_file_path_is_namespaced(_R("\\\\?\\"))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\.\\C:\\Test"))) goto error;
	if (!rt_file_path_is_namespaced(_R("\\\\?\\C:\\Test"))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_namespace_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = rt_char_get_size(path);
	if (!rt_char_copy(path, buffer_size, buffer, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_namespace(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;

	if (buffer_size != rt_char_get_size(expected)) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, buffer_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_namespace()
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_namespace_do(_R("c:\\foo/bar"), _R("\\\\?\\c:\\foo\\bar"))) goto error;
	if (!zz_test_namespace_do(_R("c:\\"), _R("\\\\?\\c:\\"))) goto error;
	if (!zz_test_namespace_do(_R("\\\\server/directory"), _R("\\\\?\\UNC\\server\\directory"))) goto error;
	if (!zz_test_namespace_do(_R("\\\\?\\c:\\foo\\bar"), _R("\\\\?\\c:\\foo\\bar"))) goto error;
#else
	if (!zz_test_namespace_do(_R("/foo"), _R("/foo"))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_strip_namespace_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	buffer_size = rt_char_get_size(path);
	if (!rt_char_copy(path, buffer_size, buffer, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_strip_namespace(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;

	if (buffer_size != rt_char_get_size(expected)) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, buffer_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_strip_namespace()
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_strip_namespace_do(_R("\\\\?\\c:\\foo\\bar"),           _R("c:\\foo\\bar"))) goto error;
	if (!zz_test_strip_namespace_do(_R("\\\\?\\c:\\"),                   _R("c:\\"))) goto error;
	if (!zz_test_strip_namespace_do(_R("\\\\?\\UNC\\server\\directory"), _R("\\\\server\\directory"))) goto error;
	if (!zz_test_strip_namespace_do(_R("c:\\foo\\bar"),                  _R("c:\\foo\\bar"))) goto error;
#else
	if (!zz_test_strip_namespace_do(_R("/foo"), _R("/foo"))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_type_do(const rt_char *path, enum rt_file_path_type expected)
{
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_type(path, &type)) goto error;
	if (type != expected) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_type()
{
	rt_s ret;

	if (!zz_test_get_type_do(_R("../test_resources/bad"), RT_FILE_PATH_TYPE_NONE)) goto error;
	if (!zz_test_get_type_do(_R("../test_resources/bad.txt"), RT_FILE_PATH_TYPE_NONE)) goto error;
	if (!zz_test_get_type_do(_R("../test_resources/bad/bad"), RT_FILE_PATH_TYPE_NONE)) goto error;
	if (!zz_test_get_type_do(_R("../test_resources/bad/bad.txt"), RT_FILE_PATH_TYPE_NONE)) goto error;
	if (!zz_test_get_type_do(_R("../test_resources/dir"), RT_FILE_PATH_TYPE_DIR)) goto error;
	if (!zz_test_get_type_do(_R("../test_resources/dir/file.txt"), RT_FILE_PATH_TYPE_FILE)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_last_separator_index_do(const rt_char *path, rt_un expected)
{
	rt_un index;
	rt_un path_size = rt_char_get_size(path);

	index = rt_file_path_get_last_separator_index(path, path_size);
	return expected == index;
}

static rt_s zz_test_get_last_separator_index()
{
	rt_s ret;

	if (!zz_test_get_last_separator_index_do(_R("Foo"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("Foo/"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("/"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("//"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R(""), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("/Foo"), 0)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("/Foo/"), 0)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("bar/Foo/"), 3)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("bar//Foo//"), 4)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("/bar/Foo/"), 4)) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_get_last_separator_index_do(_R("c:\\foo\\bar"), 6)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("\\\\foo"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("\\\\foo\\"), RT_TYPE_MAX_UN)) goto error;
	if (!zz_test_get_last_separator_index_do(_R("\\\\foo\\bar"), 5)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_parent_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size = rt_char_get_size(path);
	rt_s ret;

	if (!rt_char_copy(path, buffer_size, buffer, RT_FILE_PATH_SIZE)) goto error;

	if (!rt_file_path_get_parent(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;

	if (buffer_size != rt_char_get_size(expected)) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, buffer_size)) goto error;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_parent()
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (!rt_file_path_get_current_dir(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!zz_test_get_parent_do(_R("foo.txt"), buffer)) goto error;
	if (!zz_test_get_parent_do(_R("./foo.txt"), buffer)) goto error;

	if (!rt_file_path_append_separator(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!rt_char_append(_R("data"), 4, buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (!zz_test_get_parent_do(_R("data/foo.txt"), buffer)) goto error;
	if (!zz_test_get_parent_do(_R("./data/foo.txt"), buffer)) goto error;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_get_parent_do(_R("\\\\"), _R("\\\\"))) goto error;
	if (!zz_test_get_parent_do(_R("c:\\"), _R("c:\\"))) goto error;
	if (!zz_test_get_parent_do(_R("c:\\foo"), _R("c:"))) goto error;
	if (!zz_test_get_parent_do(_R("c:\\foo\\"), _R("c:"))) goto error;
	if (!zz_test_get_parent_do(_R("c:\\foo\\bar"), _R("c:\\foo"))) goto error;
	if (!zz_test_get_parent_do(_R("c:\\foo\\\\bar"), _R("c:\\foo"))) goto error;
	if (!zz_test_get_parent_do(_R("\\\\foo"), _R("\\\\foo"))) goto error;
	if (!zz_test_get_parent_do(_R("\\\\foo\\"), _R("\\\\foo\\"))) goto error;
	if (!zz_test_get_parent_do(_R("\\\\foo\\bar"), _R("\\\\foo"))) goto error;
#else
	if (!zz_test_get_parent_do(_R("/"), _R("/"))) goto error;
	if (!zz_test_get_parent_do(_R("/data"), _R("/"))) goto error;
	if (!zz_test_get_parent_do(_R("/data/"), _R("/"))) goto error;
	if (!zz_test_get_parent_do(_R("/data/foo"), _R("/data"))) goto error;
	if (!zz_test_get_parent_do(_R("/data/foo/"), _R("/data"))) goto error;
	if (!zz_test_get_parent_do(_R("/data/foo/."), _R("/data"))) goto error;
	if (!zz_test_get_parent_do(_R("/data/foo/.."), _R("/"))) goto error;
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_name_do(const rt_char *path, const rt_char *expected)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

	if (!rt_file_path_get_name(path, rt_char_get_size(path), buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
	if (rt_char_get_size(buffer) != buffer_size) goto error;
	if (!rt_char_equals(buffer, buffer_size, expected, rt_char_get_size(expected))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_get_name()
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (!zz_test_get_name_do(_R("c:\\foo\\bar"), _R("bar"))) goto error;
	if (!zz_test_get_name_do(_R("c:\\bar"), _R("bar"))) goto error;
	if (!zz_test_get_name_do(_R("c:\\"), _R("c:"))) goto error;
	if (!zz_test_get_name_do(_R("\\"), _R("\\"))) goto error;
	if (!zz_test_get_name_do(_R("\\\\"), _R("\\"))) goto error;
#endif

	/* From basename testing. */
	if (!zz_test_get_name_do(_R("./data/foo.txt"), _R("foo.txt"))) goto error;
	if (!zz_test_get_name_do(_R("./foo"), _R("foo"))) goto error;
	if (!zz_test_get_name_do(_R("/foo"), _R("foo"))) goto error;
	if (!zz_test_get_name_do(_R("foo/"), _R("foo"))) goto error;
	if (!zz_test_get_name_do(_R("//foo"), _R("foo"))) goto error;
	if (!zz_test_get_name_do(_R("foo//"), _R("foo"))) goto error;
	if (!zz_test_get_name_do(_R("./"), _R("."))) goto error;
	if (!zz_test_get_name_do(_R("/."), _R("."))) goto error;
	if (!zz_test_get_name_do(_R("//"), _R("/"))) goto error;

	/* From basename specification. */
	if (!zz_test_get_name_do(_R(""), _R("."))) goto error;

	/* From basename examples. */
	if (!zz_test_get_name_do(_R("/usr/lib"), _R("lib"))) goto error;
	if (!zz_test_get_name_do(_R("/usr/"), _R("usr"))) goto error;
	if (!zz_test_get_name_do(_R("usr"), _R("usr"))) goto error;
	if (!zz_test_get_name_do(_R("/"), _R("/"))) goto error;
	if (!zz_test_get_name_do(_R("."), _R("."))) goto error;
	if (!zz_test_get_name_do(_R(".."), _R(".."))) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

static rt_s zz_test_browse_callback(const rt_char *path, rt_un type, void *context)
{
	rt_un *paths_count;

	paths_count = (rt_un*)context;
	(*paths_count)++;

	return RT_OK;
}

static rt_s zz_test_browse(const rt_char *tmp_dir, rt_un tmp_dir_size)
{
	rt_char path[RT_FILE_PATH_SIZE];
	rt_un path_size;
	rt_char sub_path[RT_FILE_PATH_SIZE];
	rt_un sub_path_size;
	rt_un paths_count;
	enum rt_file_path_type type;
	rt_s ret;

	/* Create empty directory named browse. */
	path_size = tmp_dir_size;
	if (!rt_char_copy(tmp_dir, path_size, path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(path, RT_FILE_PATH_SIZE, &path_size)) goto error;
	if (!rt_char_append(_R("browse"), 6, path, RT_FILE_PATH_SIZE, &path_size)) goto error;
	if (!rt_file_system_delete_dir_recursively(path)) goto error;
	if (!rt_file_system_create_dir(path)) goto error;

	/* Create empty file named file.txt. */
	sub_path_size = path_size;
	if (!rt_char_copy(path, sub_path_size, sub_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_char_append(_R("file.txt"), 8, sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_file_system_create_empty_file(sub_path, RT_FALSE)) goto error;

	/* Create empty directory named sub. */
	sub_path_size = path_size;
	if (!rt_char_copy(path, sub_path_size, sub_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_append_separator(sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_char_append(_R("sub"), 3, sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_file_system_create_dir(sub_path)) goto error;

	/* Create empty file named sub_file.txt. */
	if (!rt_file_path_append_separator(sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_char_append(_R("sub_file.txt"), 12, sub_path, RT_FILE_PATH_SIZE, &sub_path_size)) goto error;
	if (!rt_file_system_create_empty_file(sub_path, RT_FALSE)) goto error;

	paths_count = 0;
	if (!rt_file_path_browse(path, &zz_test_browse_callback, RT_TRUE, RT_TRUE, &paths_count)) goto error;
	if (paths_count != 3) goto error;

	paths_count = 0;
	if (!rt_file_path_browse(path, &zz_test_browse_callback, RT_FALSE, RT_TRUE, &paths_count)) goto error;
	if (paths_count != 2) goto error;

	if (!rt_file_system_delete_dir_recursively(path)) goto error;
	if (!rt_file_path_get_type(path, &type)) goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s zz_test_file_path()
{
	rt_char tmp_dir[RT_FILE_PATH_SIZE];
	rt_un tmp_dir_size;
	rt_s ret;

	if (!zz_get_tmp_dir(tmp_dir, RT_FILE_PATH_SIZE, &tmp_dir_size)) goto error;

	if (!zz_test_get_current_dir()) goto error;
	if (!zz_test_set_current_dir(tmp_dir, tmp_dir_size)) goto error;
	if (!zz_test_get_executable_path()) goto error;
	if (!zz_test_append_separator()) goto error;
	if (!zz_test_full()) goto error;
	if (!zz_test_is_namespaced()) goto error;
	if (!zz_test_namespace()) goto error;
	if (!zz_test_strip_namespace()) goto error;
	if (!zz_test_get_type()) goto error;
	if (!zz_test_get_last_separator_index()) goto error;
	if (!zz_test_get_parent()) goto error;
	if (!zz_test_get_name()) goto error;
	if (!zz_test_browse(tmp_dir, tmp_dir_size)) goto error;

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}
