#include "layer005/rt_file_system.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_char.h"
#include "layer003/rt_file.h"
#include "layer004/rt_file_path.h"

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
#else
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
#else
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
		if (!rt_io_device_free(&file.io_device) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

static rt_s rt_file_system_delete_dir_recursively_callback(const rt_char *path, enum rt_file_path_type type, RT_UNUSED void *context)
{
	rt_s ret;

	if (type == RT_FILE_PATH_TYPE_DIR) {
		if (!rt_file_system_delete_dir(path))
			goto error;
	} else {
		if (!rt_file_system_delete_file(path))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_delete_dir_recursively(const rt_char *dir_path)
{
	rt_s ret;

	/* If the directory does not exist or is empty, then rt_file_system_delete_dir_if_exists should do the job. */
	if (!rt_file_system_delete_dir_if_exists(dir_path)) {
		/* The directory should exist and we failed to delete it, probably because it is not empty. */
		if (!rt_file_path_browse(dir_path, rt_file_system_delete_dir_recursively_callback, RT_TRUE, RT_TRUE, RT_NULL)) goto error;

		/* Finally, delete the directory that should be now empty. */
		if (!rt_file_system_delete_dir(dir_path)) goto error;
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_get_file_size(const rt_char *file_path, rt_un64 *file_size)
{
#ifdef RT_DEFINE_WINDOWS
	const rt_char *actual_path;
	rt_char namespaced_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	WIN32_FILE_ATTRIBUTE_DATA file_info;
	LARGE_INTEGER large_integer;
#else
	struct stat file_info;
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

	/* GetFileAttributesEx returns 0 and use SetLastError in case of error. */
	if (!GetFileAttributesEx(actual_path, GetFileExInfoStandard, &file_info))
		goto error;

	large_integer.HighPart = file_info.nFileSizeHigh;
	large_integer.LowPart = file_info.nFileSizeLow;
	*file_size = large_integer.QuadPart;

#else /* RT_DEFINE_WINDOWS */

	/* stat returns zero in case of success, -1 in case of failure and set errno. */
	if (stat(file_path, &file_info))
		goto error;
	*file_size = file_info.st_size;

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_copy_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
#ifdef RT_DEFINE_WINDOWS
	rt_char source_namespaced_path[RT_FILE_PATH_SIZE];
	rt_un source_buffer_size;
	const rt_char *source_actual_path;
	rt_char destination_namespaced_path[RT_FILE_PATH_SIZE];
	rt_un destination_buffer_size;
	const rt_char *destination_actual_path;
#else
	rt_char8 buffer[BUFSIZ];
	ssize_t read_bytes;
	int source_file_descriptor;
	int destination_file_descriptor;
	rt_b source_open = RT_FALSE;
	rt_b destination_open = RT_FALSE;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (rt_file_path_is_namespaced(source_file_path)) {
		source_actual_path = source_file_path;
	} else {
		source_buffer_size = rt_char_get_size(source_file_path);
		if (!rt_char_copy(source_file_path, source_buffer_size, source_namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(source_namespaced_path, RT_FILE_PATH_SIZE, &source_buffer_size)) goto error;
		source_actual_path = source_namespaced_path;
	}

	if (rt_file_path_is_namespaced(destination_file_path)) {
		destination_actual_path = destination_file_path;
	} else {
		destination_buffer_size = rt_char_get_size(destination_file_path);
		if (!rt_char_copy(destination_file_path, destination_buffer_size, destination_namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(destination_namespaced_path, RT_FILE_PATH_SIZE, &destination_buffer_size)) goto error;
		destination_actual_path = destination_namespaced_path;
	}

	/* Returns 0 in case of error and set last error. */
	ret = CopyFile(source_actual_path, destination_actual_path, TRUE);

free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;

#else

	/* Open source. */
	source_file_descriptor = open(source_file_path, O_RDONLY | O_CLOEXEC);
	if (source_file_descriptor == -1)
		goto error;
	source_open = RT_TRUE;

	/* Open destination. */
	destination_file_descriptor = open(destination_file_path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, RT_FILE_SYSTEM_RIGHTS);
	if (destination_file_descriptor == -1)
		goto error;
	destination_open = RT_TRUE;

	while (RT_TRUE) {
		read_bytes = read(source_file_descriptor, buffer, BUFSIZ);
		if (!read_bytes) {
			break;
		} else if (read_bytes == -1) {
			goto error;
		} else {
			if (write(destination_file_descriptor, buffer, read_bytes) == -1) {
				goto error;
			}
		}
	}

	ret = RT_OK;
free:
	if (destination_open) {
		destination_open = RT_FALSE;
		if (close(destination_file_descriptor) && ret)
			goto error;
	}
	if (source_open) {
		source_open = RT_FALSE;
		if (close(source_file_descriptor) && ret)
			goto error;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}

static rt_s rt_file_system_move_or_rename_dir_or_file(const rt_char *source_file_path, const rt_char *destination_file_path, RT_WINDOWS_UNUSED rt_b rename_operation, RT_WINDOWS_UNUSED rt_b dir)
{
#ifdef RT_DEFINE_WINDOWS
	rt_char source_namespaced_path[RT_FILE_PATH_SIZE];
	rt_un source_buffer_size;
	const rt_char *source_actual_path;
	rt_char destination_namespaced_path[RT_FILE_PATH_SIZE];
	rt_un destination_buffer_size;
	const rt_char *destination_actual_path;
#else
	enum rt_file_path_type type;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (rt_file_path_is_namespaced(source_file_path)) {
		source_actual_path = source_file_path;
	} else {
		source_buffer_size = rt_char_get_size(source_file_path);
		if (!rt_char_copy(source_file_path, source_buffer_size, source_namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(source_namespaced_path, RT_FILE_PATH_SIZE, &source_buffer_size)) goto error;
		source_actual_path = source_namespaced_path;
	}

	if (rt_file_path_is_namespaced(destination_file_path)) {
		destination_actual_path = destination_file_path;
	} else {
		destination_buffer_size = rt_char_get_size(destination_file_path);
		if (!rt_char_copy(destination_file_path, destination_buffer_size, destination_namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(destination_namespaced_path, RT_FILE_PATH_SIZE, &destination_buffer_size)) goto error;
		destination_actual_path = destination_namespaced_path;
	}

	/* Returns 0 and set last error in case of error. */
	if (!MoveFileEx(source_actual_path, destination_actual_path, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH)) goto error;
#else

	/* Linux rename command overwrites existing file so we have to manually check. */
	if (!rt_file_path_get_type(destination_file_path, &type))
		goto error;
	if (type != RT_FILE_PATH_TYPE_NONE) {
		rt_error_set_last(RT_ERROR_FILE_ALREADY_EXISTS);
		goto error;
	}

	/* rename is faster under Linux but does not work across files systems/mount points. */
	if (rename(source_file_path, destination_file_path)) {
		if (rename_operation) {
			/* Rename has failed while it is a renaming (So same file system/mount point). */
			goto error;
		}
		/* The rename call has failed, try another approach to move: copy then delete. */
		if (!rt_file_system_copy_file(source_file_path, destination_file_path)) goto error;
		if (!rt_file_system_delete_file(source_file_path)) goto error;
	}
#endif

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_system_move_dir(const rt_char *source_dir_path, const rt_char *destination_dir_path)
{
	return rt_file_system_move_or_rename_dir_or_file(source_dir_path, destination_dir_path, RT_FALSE, RT_TRUE);
}

rt_s rt_file_system_rename_dir(const rt_char *source_dir_path, const rt_char *destination_dir_name)
{
	rt_char destination_dir_path[RT_FILE_PATH_SIZE];
	rt_un destination_dir_path_size;
	rt_s ret;

	/* Append destination directory name to source directory parent path. */
	destination_dir_path_size = rt_char_get_size(source_dir_path);
	if (!rt_char_copy(source_dir_path, destination_dir_path_size, destination_dir_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_get_parent(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size)) goto error;
	if (!rt_file_path_append_separator(destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size)) goto error;
	if (!rt_char_append(destination_dir_name, rt_char_get_size(destination_dir_name), destination_dir_path, RT_FILE_PATH_SIZE, &destination_dir_path_size)) goto error;

	if (!rt_file_system_move_or_rename_dir_or_file(source_dir_path, destination_dir_path, RT_TRUE, RT_TRUE)) goto error;

	ret = RT_OK;
	goto free;
error:
	ret = RT_FAILED;
free:
	return ret;
}

rt_s rt_file_system_move_file(const rt_char *source_file_path, const rt_char *destination_file_path)
{
	return rt_file_system_move_or_rename_dir_or_file(source_file_path, destination_file_path, RT_FALSE, RT_FALSE);
}

rt_s rt_file_system_rename_file(const rt_char *source_file_path, const rt_char *destination_file_name)
{
	rt_char destination_file_path[RT_FILE_PATH_SIZE];
	rt_un destination_file_path_size;
	rt_s ret;

	/* Append destination file name to source file parent path. */
	destination_file_path_size = rt_char_get_size(source_file_path);
	if (!rt_char_copy(source_file_path, destination_file_path_size, destination_file_path, RT_FILE_PATH_SIZE)) goto error;
	if (!rt_file_path_get_parent(destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size)) goto error;
	if (!rt_file_path_append_separator(destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size)) goto error;
	if (!rt_char_append(destination_file_name, rt_char_get_size(destination_file_name), destination_file_path, RT_FILE_PATH_SIZE, &destination_file_path_size)) goto error;

	if (!rt_file_system_move_or_rename_dir_or_file(source_file_path, destination_file_path, RT_TRUE, RT_FALSE)) goto error;

	ret = RT_OK;
	goto free;
error:
	ret = RT_FAILED;
free:
	return ret;
}
