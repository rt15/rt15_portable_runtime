#include "layer006/rt_file_system.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer004/rt_file_path.h"
#include "layer005/rt_file.h"

#ifndef RT_DEFINE_WINDOWS

/* rwx to user, rx to group and others. */
#define RT_FILE_SYSTEM_RIGHTS S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH

#endif

rt_s rt_file_system_create_dir(const rt_char *dir_path)
{
#ifdef RT_DEFINE_WINDOWS
	const rt_char *actual_path;
	rt_char namespaced_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (rt_file_path_is_namespaced(dir_path)) {
		actual_path = dir_path;
	} else {
		buffer_size = rt_char_get_size(dir_path);
		if (!rt_char_copy(dir_path, buffer_size, namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(namespaced_path, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
		actual_path = namespaced_path;
	}

	/* If CreateDirectory fails, the return value is zero and SetLastError is called. */
	if (!CreateDirectory(actual_path, NULL)) goto error;
#else
	/* mkdir returns zero on success, or -1 if an error occurred (in which case, errno is set appropriately). */
	if (mkdir(dir_path, RT_FILE_SYSTEM_RIGHTS)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_create_dirs(const rt_char *dir_path)
{
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	enum rt_file_path_type type;
	rt_s ret;

	if (!rt_file_path_get_type(dir_path, &type)) goto error;

	switch (type) {
	case RT_FILE_PATH_TYPE_DIR:
		/* It is already a directory, nothing to do. */
		break;
	case RT_FILE_PATH_TYPE_FILE:
		/* A file is in the way. */
		rt_error_set_last(RT_ERROR_FILE_ALREADY_EXISTS);
		goto error;
		break;
	case RT_FILE_PATH_TYPE_NONE:
		/* Create the parents, then this directory.*/
		buffer_size = rt_char_get_size(dir_path);
		if (!rt_char_copy(dir_path, buffer_size, buffer, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_get_parent(buffer, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
		if (!rt_file_system_create_dirs(buffer)) goto error;
		if (!rt_file_system_create_dir(dir_path)) goto error;
		break;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_delete_dir(const rt_char *dir_path)
{
#ifdef RT_DEFINE_WINDOWS
	const rt_char *actual_path;
	rt_char namespaced_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (rt_file_path_is_namespaced(dir_path)) {
		actual_path = dir_path;
	} else {
		buffer_size = rt_char_get_size(dir_path);
		if (!rt_char_copy(dir_path, buffer_size, namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(namespaced_path, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
		actual_path = namespaced_path;
	}

	/* RemoveDirectory returns 0 and call SetLastError in case of error. */
	if (!RemoveDirectory(actual_path)) goto error;
#else /* NOT RT_DEFINE_WINDOWS */
	/* On success, rmdir returns 0. On error, -1 is returned, and errno is set appropriately.	*/
	if (rmdir(dir_path)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_delete_dir_if_exists(const rt_char *dir_path)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD last_error;
#endif
	rt_s ret;

	/* Attempt to delete the directory then check the last error. */
	if (!rt_file_system_delete_dir(dir_path)) {
#ifdef RT_DEFINE_WINDOWS
		last_error = GetLastError();
		if (last_error != ERROR_PATH_NOT_FOUND && last_error != ERROR_FILE_NOT_FOUND) goto error;
#else
		if (errno != ENOENT) goto error;
#endif
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_delete_file(const rt_char *file_path)
{
#ifdef RT_DEFINE_WINDOWS
	const rt_char *actual_path;
	rt_char namespaced_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (rt_file_path_is_namespaced(file_path)) {
		actual_path = file_path;
	} else {
		buffer_size = rt_char_get_size(file_path);
		if (!rt_char_copy(file_path, buffer_size, namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(namespaced_path, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
		actual_path = namespaced_path;
	}

	/* DeleteFile returns 0 and call SetLastError in case of error. */
	if (!DeleteFile(actual_path)) goto error;
#else /* NOT RT_DEFINE_WINDOWS */
	/* On success, unlink returns 0. On error, -1 is returned, and errno is set appropriately.	*/
	if (unlink(file_path)) goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_delete_file_if_exists(const rt_char *file_path)
{
	rt_s ret;

	/* Attempt to delete the file then check the last error. */
	if (!rt_file_system_delete_file(file_path)) {
#ifdef RT_DEFINE_WINDOWS
		if (GetLastError() != ERROR_FILE_NOT_FOUND) goto error;
#else
		if (errno != ENOENT) goto error;
#endif
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_create_empty_file(const rt_char *file_path, rt_b truncate)
{
	struct rt_file file;
	rt_b file_created;
	rt_s ret;

	file_created = RT_FALSE;

	if (!rt_file_create(&file, file_path, truncate ? RT_FILE_MODE_TRUNCATE : RT_FILE_MODE_NEW)) goto error;
	file_created = RT_TRUE;

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
