#include "layer004/rt_file_path.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_env_var.h"

rt_s rt_file_path_get_current_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un written;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	written = GetCurrentDirectory((DWORD)buffer_capacity, buffer);
	if (!written)
		goto error;
	/* GetCurrentDirectory returns the required buffer size if the buffer is too small. */
	if (written > buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
#else /* NOT RT_DEFINE_WINDOWS */

	/* Returns NULL in case of error and set errno. Returns buffer in case of success. */
	if (!getcwd(buffer, buffer_capacity))
		goto error;
	written = strlen(buffer);
#endif

	*buffer_size = written;
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_append_separator(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_char last_char;
	rt_s ret;

	if (local_buffer_size) {
		last_char = path[local_buffer_size - 1];
		if (!RT_FILE_PATH_IS_SEPARATOR(last_char)) {
			if (!rt_char_append_char(RT_FILE_PATH_SEPARATOR, path, buffer_capacity, buffer_size))
				goto error;
		}
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_full(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD result;
	rt_char *file_part;
#endif
	rt_char buffer[RT_FILE_PATH_SIZE];
	rt_un input_path_size = *buffer_size;
	rt_un local_buffer_size;
	rt_s ret;

	if (input_path_size) {
#ifdef RT_DEFINE_WINDOWS
		result = GetFullPathName(path, RT_FILE_PATH_SIZE, buffer, &file_part);
		/* In case of error, GetFullPathName returns zero and set last error. */
		if (result == 0)
			goto error;

		/* If the buffer is too small, GetFullPathName returns the required buffer size. */
		if (result > RT_FILE_PATH_SIZE) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		local_buffer_size = result;
		if (!rt_char_copy(buffer, local_buffer_size, path, buffer_capacity)) goto error;
		*buffer_size = local_buffer_size;
#else
		if (path[0] != '/') {
			if (!rt_file_path_get_current_dir(buffer, RT_FILE_PATH_SIZE, &local_buffer_size)) goto error;
			if (!rt_file_path_append_separator(buffer, RT_FILE_PATH_SIZE, &local_buffer_size)) goto error;
			if (!rt_char_append(path, input_path_size, buffer, RT_FILE_PATH_SIZE, &local_buffer_size)) goto error;
			if (!rt_char_copy(buffer, local_buffer_size, path, buffer_capacity)) goto error;
			*buffer_size = local_buffer_size;
		}
#endif
	} else {
		/* Empty input path, return the current directory. */
		if (!rt_file_path_get_current_dir(path, buffer_capacity, buffer_size))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_b rt_file_path_is_namespaced(const rt_char *path)
{
#ifdef RT_DEFINE_WINDOWS
	/* If we hit the final null then we automatically stop iterating. */
	return (path[0] == _R('\\') &&
	        path[1] == _R('\\') &&
	        (path[2] == _R('?') || path[2] == _R('.')) &&
	        path[3] == _R('\\'));

#else
	/* All Linux paths are considered namespaced. We do not need to namespace them. */
	return RT_TRUE;
#endif
}

rt_s rt_file_path_namespace(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un local_buffer_size;
	rt_un i;
	rt_un skip_chars;
	rt_s ret;

	if (*buffer_size >= 4 &&
	    path[0] == _R('\\') &&
	    path[1] == _R('\\') &&
	    (path[2] == _R('?') || path[2] == _R('.')) &&
	    path[3] == _R('\\')) {
		/* The path is already prefixed by a namespace, nothing to do. */
		goto end;
	}

	if (!rt_file_path_full(path, buffer_capacity, buffer_size))
		goto error;

	/* After rt_file_path_full call, the path should start with either "x:" or "\\", but let's check it. */
	local_buffer_size = *buffer_size;
	if (local_buffer_size >= 2) {
		if (path[1] == _R(':')) {
			/* Check that there is room for the 4 characters prefix and the null terminating character. */
			if (local_buffer_size + 5 <= buffer_capacity) {
				/* Move the path plus the null character. */
				RT_MEMORY_MOVE(path, &path[4], (local_buffer_size + 1) * sizeof(rt_char));
				local_buffer_size += 4;
				/* Skip the 4 characters prefix plus the volume. */
				skip_chars = 6;

				path[0] = _R('\\');
				path[1] = _R('\\');
				path[2] = _R('?');
				path[3] = _R('\\');
			} else {
				rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
				goto error;
			}
		} else if (path[0] == _R('\\') && path[1] == _R('\\')) {
			/* We are replacing "\" by "\\?\UNC" so we need 6 characters and the null terminating character. */
			if (local_buffer_size + 7 <= buffer_capacity) {
				/* Move the path without the first backslash plus the null character. */
				RT_MEMORY_MOVE(&path[1], &path[7], local_buffer_size * sizeof(rt_char));
				local_buffer_size += 6;
				/* Skip "\\?\UNC\". */
				skip_chars = 8;

				/* First two backslash are there from the beginning, and we copied the last one. */
				path[2] = _R('?');
				path[3] = _R('\\');
				path[4] = _R('U');
				path[5] = _R('N');
				path[6] = _R('C');
			} else {
				rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
				goto error;
			}
		} else {
			rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
			goto error;
		}

		/* Namespaced paths support backslash only. */
		for (i = skip_chars; i < local_buffer_size; i++)
			if (path[i] == '/')
				path[i] = '\\';

		*buffer_size = local_buffer_size;
	} else {
		rt_error_set_last(RT_ERROR_FUNCTION_FAILED);
		goto error;
	}
end:
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
#else
	/* If someone calls this function from a portable code and does not want or cannot deal with conditional compilation. */
	return rt_file_path_full(path, buffer_capacity, buffer_size);
#endif
}

rt_b rt_file_path_get_type(const rt_char *path, enum rt_file_path_type *type)
{
#ifdef RT_DEFINE_WINDOWS
	const rt_char *actual_path;
	rt_char namespaced_path[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	DWORD attributes;
	DWORD last_error;
#else
	struct stat stats;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (rt_file_path_is_namespaced(path)) {
		actual_path = path;
	} else {
		buffer_size = rt_char_get_size(path);
		if (!rt_char_copy(path, buffer_size, namespaced_path, RT_FILE_PATH_SIZE)) goto error;
		if (!rt_file_path_namespace(namespaced_path, RT_FILE_PATH_SIZE, &buffer_size)) goto error;
		actual_path = namespaced_path;
	}

	attributes = GetFileAttributes(actual_path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		last_error = GetLastError();
		if (last_error == ERROR_PATH_NOT_FOUND || last_error == ERROR_FILE_NOT_FOUND) {
			*type = RT_FILE_PATH_TYPE_NONE;
		} else {
			goto error;
		}
	} else {
		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			*type = RT_FILE_PATH_TYPE_DIR;
		else
			*type = RT_FILE_PATH_TYPE_FILE;
	}

#else

	if (stat(path, &stats)) {
		if (errno == ENOENT) {
			*type = RT_FILE_PATH_TYPE_NONE;
		} else {
			goto error;
		}
	} else {
		if (S_ISDIR(stats.st_mode))
			*type = RT_FILE_PATH_TYPE_DIR;
		else
			*type = RT_FILE_PATH_TYPE_FILE;
	}

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_un rt_file_path_get_last_separator_index(const rt_char *path, rt_un path_size)
{
	rt_un i;

	/* Skip trailing separators. */
	i = path_size - 1;
	while ((i < RT_TYPE_MAX_UN) && RT_FILE_PATH_IS_SEPARATOR(path[i]))
		i--;

	/* Search next separator. */
	while ((i < RT_TYPE_MAX_UN) && !RT_FILE_PATH_IS_SEPARATOR(path[i]))
		i--;

#ifdef RT_DEFINE_WINDOWS
	/* If the path looks like \\XXX, then there is no interesting last separator. */
	if (i == 1 && path[0] == _R('\\') && path[1] == _R('\\')) {
		i = RT_TYPE_MAX_UN;
	}
#endif
	return i;
}

rt_s rt_file_path_get_parent(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un last_separator_index;
	rt_s ret;

	if (!rt_file_path_full(path, buffer_capacity, buffer_size)) goto error;

	last_separator_index = rt_file_path_get_last_separator_index(path, *buffer_size);
	if (last_separator_index != RT_TYPE_MAX_UN) {
		/* We cut the path at the right place. */
		path[last_separator_index] = 0;
		*buffer_size = last_separator_index;
	}
	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_get_executable_path(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un written;
	rt_s ret;

#ifdef RT_DEFINE_LINUX
	rt_un text_buffer_capacity;
#endif

#ifdef RT_DEFINE_WINDOWS
	/* Returns zero and set last error in case of issue (except if the buffer is too small). */
	written = GetModuleFileName(NULL, buffer, (DWORD)buffer_capacity);
	if (!written) goto error;
	/* GetModuleFileName does not fail if the buffer is too small. */
	if (written == buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
#else
	/* Keep space for NULL character because readlink does not write it. */
	text_buffer_capacity = buffer_capacity - 1;

	/* On error, -1 is returned and errno is set to indicate the error. */
	written = readlink("/proc/self/exe", buffer, text_buffer_capacity);
	if (written == -1) {
		written = 0;
		goto error;
	}
	/* Add null trailing character as readlink does not. */
	buffer[written] = 0;

	if (written == text_buffer_capacity) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
#endif

	ret = RT_OK;
free:
	*buffer_size = written;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
