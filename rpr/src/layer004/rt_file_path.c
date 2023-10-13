#include "layer004/rt_file_path.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"
#include "layer003/rt_env_var.h"

rt_s rt_file_path_browse(const rt_char *dir_path, rt_file_path_browse_callback_t callback, rt_b recursively, rt_b children_first, void *context)
{
#ifdef RT_DEFINE_WINDOWS
	WIN32_FIND_DATA find_file_data;
	HANDLE find_handle = INVALID_HANDLE_VALUE;
#else
	DIR *dir;
	struct dirent *dir_entry;
#endif
	rt_char child[RT_FILE_PATH_SIZE];
	rt_un buffer_size;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	buffer_size = rt_char_get_size(dir_path);
	if (RT_UNLIKELY(!rt_char_copy(dir_path, buffer_size, child, RT_FILE_PATH_SIZE))) goto error;
	if (RT_UNLIKELY(!rt_file_path_namespace(child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('*'), child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;

	find_handle = FindFirstFile(child, &find_file_data);
	if (find_handle == INVALID_HANDLE_VALUE) {
		if (RT_LIKELY(GetLastError() == ERROR_FILE_NOT_FOUND)) {
			ret = RT_OK;
			goto free;
		} else {
			goto error;
		}
	}

	do {
		if (lstrcmp(_R("."),  find_file_data.cFileName) &&
		    lstrcmp(_R(".."), find_file_data.cFileName)) {

			buffer_size = rt_char_get_size(dir_path);
			if (RT_UNLIKELY(!rt_char_copy(dir_path, buffer_size, child, RT_FILE_PATH_SIZE))) goto error;
			if (RT_UNLIKELY(!rt_file_path_append_separator(child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
			if (RT_UNLIKELY(!rt_char_append(find_file_data.cFileName, rt_char_get_size(find_file_data.cFileName), child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;

			if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (!children_first) {
					if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_DIR, context)))
						goto error;
				}

				if (recursively) {
					if (RT_UNLIKELY(!rt_file_path_browse(child, callback, recursively, children_first, context)))
						goto error;
				}

				if (children_first) {
					if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_DIR, context)))
						goto error;
				}
			} else {
				if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_FILE, context)))
					goto error;
			}
		}
	} while (FindNextFile(find_handle, &find_file_data));

	/* Check that we have exited loop because there are no more files. */
	if (RT_UNLIKELY(GetLastError() != ERROR_NO_MORE_FILES))
		goto error;

	ret = RT_OK;
free:
	if (find_handle != INVALID_HANDLE_VALUE) {
		if (RT_UNLIKELY(!FindClose(find_handle) && ret)) {
			find_handle = INVALID_HANDLE_VALUE;
			goto error;
		}
		find_handle = INVALID_HANDLE_VALUE;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;

#else

	dir = RT_NULL;

	dir = opendir(dir_path);
	if (RT_UNLIKELY(!dir))
		goto error;

	while (RT_TRUE) {
		errno = 0;
		dir_entry = readdir(dir);
		/* NULL is returned in case of error or if the the directory content has been read. */
		if (!dir_entry) {
			if (RT_UNLIKELY(errno != 0))
				goto error;
			break;
		} else {
			if (strcmp(_R("."),  dir_entry->d_name) &&
			    strcmp(_R(".."), dir_entry->d_name)) {

				buffer_size = rt_char_get_size(dir_path);
				if (RT_UNLIKELY(!rt_char_copy(dir_path, buffer_size, child, RT_FILE_PATH_SIZE))) goto error;
				if (RT_UNLIKELY(!rt_file_path_append_separator(child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
				if (RT_UNLIKELY(!rt_char_append(dir_entry->d_name, rt_char_get_size(dir_entry->d_name), child, RT_FILE_PATH_SIZE, &buffer_size))) goto error;

				if (dir_entry->d_type == DT_DIR) {
					if (!children_first) {
						if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_DIR, context)))
							goto error;
					}

					if (recursively) {
						if (RT_UNLIKELY(!rt_file_path_browse(child, callback, recursively, children_first, context)))
							goto error;
					}

					if (children_first) {
						if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_DIR, context)))
							goto error;
					}
				} else {
					if (RT_UNLIKELY(!callback(child, RT_FILE_PATH_TYPE_FILE, context)))
						goto error;
				}
			}
		}
	}

	ret = RT_OK;
free:
	if (dir) {
		/* The closedir() function returns 0 on success. */
		if (RT_UNLIKELY(closedir(dir) && ret)) {
			dir = RT_NULL;
			goto error;
		}
		dir = RT_NULL;
	}
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}

rt_s rt_file_path_get_current_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un written;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	written = GetCurrentDirectory((DWORD)buffer_capacity, buffer);
	if (RT_UNLIKELY(!written))
		goto error;
	/* GetCurrentDirectory returns the required buffer size if the buffer is too small. */
	if (RT_UNLIKELY(written > buffer_capacity)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
	if (RT_UNLIKELY(!rt_file_path_strip_namespace(buffer, buffer_capacity, &written)))
		goto error;

#else

	/* Returns NULL in case of error and set errno. Returns buffer in case of success. */
	if (RT_UNLIKELY(!getcwd(buffer, buffer_capacity)))
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

rt_s rt_file_path_set_current_dir(const rt_char *dir_path)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* Returns zero if failed, set last error. */
	/* Setting a namespaced path as current directory is kinda buggy. */
	if (RT_UNLIKELY(!SetCurrentDirectory(dir_path)))
		goto error;
#else
	/* Returns zero in case of success, set errno. */
	if (RT_UNLIKELY(chdir(dir_path)))
		goto error;
#endif
	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_append_separator(rt_char *dir_path, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un local_buffer_size = *buffer_size;
	rt_char last_char;
	rt_s ret;

	if (local_buffer_size) {
		last_char = dir_path[local_buffer_size - 1];
		if (!RT_FILE_PATH_IS_SEPARATOR(last_char)) {
			if (RT_UNLIKELY(!rt_char_append_char(RT_FILE_PATH_SEPARATOR, dir_path, buffer_capacity, buffer_size)))
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

#ifdef RT_DEFINE_LINUX

/**
 * Utility function for <tt>rt_file_path_realpath</tt>.
 *
 * <p>
 * The given <tt>dir_path</tt> must meet some prerequisites:
 * </p>
 * <ul>
 * <li>It must not be empty.</li>
 * <li>It must start and end with a slash.</li>
 * <li>It must not contain "." or "..".</li>
 * <li>It must not contain duplicated slashes.</li>
 * </ul>
 *
 * <p>
 * The resulting path meets the same prerequisites.
 * </p>
 *
 * <p>
 * By Linux convention, the parent of slash is slash.
 * </p>
 *
 * @param buffer_size The number of characters in the buffer (in/out parameter).
 */
static void rt_file_path_realpath_get_parent(rt_char *dir_path, rt_un *buffer_size)
{
	rt_un i;

	if (*buffer_size != 1) {
		/* buffer_size - 1 is slash, buffer_size - 2 is not slash. */
		i = *buffer_size - 3;
		while (RT_TRUE) {
			if (dir_path[i] == _R('/')) {
				/* Keep the slash. */
				i++;
				dir_path[i] = 0;
				*buffer_size = i;
				break;
			}
			i--;
		}
	}
}

#define RT_FILE_PATH_REALPATH_SLASH 0
#define RT_FILE_PATH_REALPATH_NAME 1
#define RT_FILE_PATH_REALPATH_DOT 2
#define RT_FILE_PATH_REALPATH_TWO_DOTS 3

/**
 * Similar to Linux <tt>realpath</tt> function, but works even if the directory or file does not exist.
 * <p>
 * Cleans up the path by resolving references to "." and "..", but does not resolve symbolic links.<br>
 * Removes duplicated slashes.<br>
 * No trailing slash, except if it is the root.
 * </p>
 *
 * @param path Read-only path to resolve. Must be an absolute path, with a slash as first character.
 * @param buffer_size The number of characters in the buffer (out parameter).
 */
static rt_s rt_file_path_realpath(const rt_char *path, rt_un path_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un state = RT_FILE_PATH_REALPATH_SLASH;
	rt_char path_char;
	rt_un i;
	rt_s ret;

	/* Make sure that given path looks absolute. */
	if (RT_UNLIKELY(!path_size || path[0] != _R('/'))) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* Prepare the result with just a single slash. */
	*buffer_size = 0;
	if (RT_UNLIKELY(!rt_char_append_char(_R('/'), buffer, buffer_capacity, buffer_size)))
		goto error;

	/* Go through all input path characters, except the first one which is slash. */
	for (i = 1; i < path_size; i++) {
		path_char = path[i];
		if (path_char == _R('/')) {
			switch (state) {
			case RT_FILE_PATH_REALPATH_SLASH:
				/* We ignore consecutive slashes. */
				break;
			case RT_FILE_PATH_REALPATH_NAME:
				/* End of a name, append the slash. */
				if (RT_UNLIKELY(!rt_char_append_char(_R('/'), buffer, buffer_capacity, buffer_size)))
					goto error;
				break;
			case RT_FILE_PATH_REALPATH_DOT:
				/* "/./", we ignore that. */
				break;
			case RT_FILE_PATH_REALPATH_TWO_DOTS:
				/* "/../", remove one level. */
				rt_file_path_realpath_get_parent(buffer, buffer_size);
				break;
			}
			state = RT_FILE_PATH_REALPATH_SLASH;
		} else if (path_char == _R('.')) {
			switch (state) {
			case RT_FILE_PATH_REALPATH_SLASH:
				/* "/.", one dot for now. */
				state = RT_FILE_PATH_REALPATH_DOT;
				break;
			case RT_FILE_PATH_REALPATH_NAME:
				/* "/x.", file name with a dot. */
				if (RT_UNLIKELY(!rt_char_append_char(_R('.'), buffer, buffer_capacity, buffer_size)))
					goto error;
				break;
			case RT_FILE_PATH_REALPATH_DOT:
				/* "/..", two dots for now. */
				state = RT_FILE_PATH_REALPATH_TWO_DOTS;
				break;
			case RT_FILE_PATH_REALPATH_TWO_DOTS:
				/* "/...", it is a name. */
				if (RT_UNLIKELY(!rt_char_append(_R("..."), 3, buffer, buffer_capacity, buffer_size)))
					goto error;
				state = RT_FILE_PATH_REALPATH_NAME;
				break;
			}
		} else {
			switch (state) {
			case RT_FILE_PATH_REALPATH_SLASH:
				/* "/x", beginning of file name. */
				if (RT_UNLIKELY(!rt_char_append_char(path_char, buffer, buffer_capacity, buffer_size)))
					goto error;
				state = RT_FILE_PATH_REALPATH_NAME;
				break;
			case RT_FILE_PATH_REALPATH_NAME:
				/* "/xx", file name. */
				if (RT_UNLIKELY(!rt_char_append_char(path_char, buffer, buffer_capacity, buffer_size)))
					goto error;
				break;
			case RT_FILE_PATH_REALPATH_DOT:
				/* "/.x", hidden file. */
				if (RT_UNLIKELY(!rt_char_append_char(_R('.'), buffer, buffer_capacity, buffer_size)))
					goto error;
				if (RT_UNLIKELY(!rt_char_append_char(path_char, buffer, buffer_capacity, buffer_size)))
					goto error;
				state = RT_FILE_PATH_REALPATH_NAME;
				break;
			case RT_FILE_PATH_REALPATH_TWO_DOTS:
				/* "/..x", strange file name. */
				if (RT_UNLIKELY(!rt_char_append(_R(".."), 2, buffer, buffer_capacity, buffer_size)))
					goto error;
				if (RT_UNLIKELY(!rt_char_append_char(path_char, buffer, buffer_capacity, buffer_size)))
					goto error;
				state = RT_FILE_PATH_REALPATH_NAME;
				break;
			}
		}
	}

	/* If the path ends with "..", we have to remove one level. */
	if (state == RT_FILE_PATH_REALPATH_TWO_DOTS) {
		rt_file_path_realpath_get_parent(buffer, buffer_size);
	}

	/* Remove trailing slash if it is not root. */
	if (*buffer_size > 1 && buffer[*buffer_size - 1] == _R('/')) {
		(*buffer_size)--;
		buffer[*buffer_size] = 0;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

#endif

rt_s rt_file_path_full(rt_char *path, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD result;
	rt_char *file_part;
#endif
	rt_char local_buffer[RT_FILE_PATH_SIZE];
	rt_un input_path_size = *buffer_size;
	rt_un local_buffer_size;
	rt_s ret;

	if (input_path_size) {
#ifdef RT_DEFINE_WINDOWS
		result = GetFullPathName(path, RT_FILE_PATH_SIZE, local_buffer, &file_part);
		/* In case of error, GetFullPathName returns zero and set last error. */
		if (RT_UNLIKELY(result == 0))
			goto error;

		/* If the buffer is too small, GetFullPathName returns the required buffer size. */
		if (RT_UNLIKELY(result > RT_FILE_PATH_SIZE)) {
			rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
			goto error;
		}
		local_buffer_size = result;
		if (RT_UNLIKELY(!rt_char_copy(local_buffer, local_buffer_size, path, buffer_capacity))) goto error;
		*buffer_size = local_buffer_size;
		/* GetFullPathName can return a namespaced path if current directory is namespaced. */
		if (RT_UNLIKELY(!rt_file_path_strip_namespace(path, buffer_capacity, buffer_size))) goto error;
#else
		if (path[0] != '/') {
			/* Append path to current directory into buffer. */
			if (RT_UNLIKELY(!rt_file_path_get_current_dir(local_buffer, RT_FILE_PATH_SIZE, &local_buffer_size))) goto error;
			if (RT_UNLIKELY(!rt_file_path_append_separator(local_buffer, RT_FILE_PATH_SIZE, &local_buffer_size))) goto error;
			if (RT_UNLIKELY(!rt_char_append(path, input_path_size, local_buffer, RT_FILE_PATH_SIZE, &local_buffer_size))) goto error;
		} else {
			if (RT_UNLIKELY(!rt_char_copy(path, input_path_size, local_buffer, RT_FILE_PATH_SIZE)))
				goto error;
			local_buffer_size = input_path_size;
		}
		if (RT_UNLIKELY(!rt_file_path_realpath(local_buffer, local_buffer_size, path, buffer_capacity, buffer_size)))
			goto error;
#endif
	} else {
		/* Empty input path, return the current directory. */
		if (RT_UNLIKELY(!rt_file_path_get_current_dir(path, buffer_capacity, buffer_size)))
			goto error;
	}

	ret = RT_OK;
free:
	return ret;
error:
	ret = RT_FAILED;
	goto free;
}

rt_b rt_file_path_is_namespaced(RT_LINUX_UNUSED const rt_char *path)
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

	if (RT_UNLIKELY(!rt_file_path_full(path, buffer_capacity, buffer_size)))
		goto error;

	/* After rt_file_path_full call, the path should start with either "x:" or "\\", but let's check it. */
	local_buffer_size = *buffer_size;
	if (RT_LIKELY(local_buffer_size >= 2)) {
		if (path[1] == _R(':')) {
			/* Check that there is room for the 4 characters prefix and the zero terminating character. */
			if (RT_LIKELY(local_buffer_size + 5 <= buffer_capacity)) {
				/* Move the path plus the zero character. */
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
			/* We are replacing "\" by "\\?\UNC" so we need 6 characters and the zero terminating character. */
			if (RT_LIKELY(local_buffer_size + 7 <= buffer_capacity)) {
				/* Move the path without the first backslash plus the zero character. */
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


rt_s rt_file_path_strip_namespace(RT_LINUX_UNUSED rt_char *path, RT_UNUSED rt_un buffer_capacity, RT_LINUX_UNUSED rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	rt_un local_buffer_size;
#endif

#ifdef RT_DEFINE_WINDOWS
	/* If we hit the final null then we automatically stop iterating. */
	if (path[0] == _R('\\') &&
	    path[1] == _R('\\') &&
	   (path[2] == _R('?') || path[2] == _R('.')) &&
	    path[3] == _R('\\')) {

		local_buffer_size = *buffer_size;

		if (path[4] == _R('U') &&
		    path[5] == _R('N') &&
		    path[6] == _R('C') &&
		    path[7] == _R('\\')) {
			/* 8 characters to remove minus the two backslashes at positions 0 and 1. */
			local_buffer_size -= 6;
			/* Result minus two backslashes plus the terminating zero. */
			RT_MEMORY_MOVE(&path[8], &path[2], (local_buffer_size - 1) * sizeof(rt_char));
		} else {
			local_buffer_size -= 4;
			/* Buffer size plus terminating zero. */
			RT_MEMORY_MOVE(&path[4], path, (local_buffer_size + 1) * sizeof(rt_char));
		}

		*buffer_size = local_buffer_size;
	}

	return RT_OK;
#else
	/* Nothing to strip. */
	return RT_OK;
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
		if (RT_UNLIKELY(!rt_char_copy(path, buffer_size, namespaced_path, RT_FILE_PATH_SIZE))) goto error;
		if (RT_UNLIKELY(!rt_file_path_namespace(namespaced_path, RT_FILE_PATH_SIZE, &buffer_size))) goto error;
		actual_path = namespaced_path;
	}

	attributes = GetFileAttributes(actual_path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		last_error = GetLastError();
		if (RT_LIKELY(last_error == ERROR_PATH_NOT_FOUND || last_error == ERROR_FILE_NOT_FOUND)) {
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
		if (RT_LIKELY(errno == ENOENT)) {
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

	if (RT_UNLIKELY(!rt_file_path_full(path, buffer_capacity, buffer_size))) goto error;

	last_separator_index = rt_file_path_get_last_separator_index(path, *buffer_size);
	if (last_separator_index != RT_TYPE_MAX_UN) {
#ifdef RT_DEFINE_WINDOWS
		/* We cut the path at the right place. */
		path[last_separator_index] = 0;
		*buffer_size = last_separator_index;
#else
		if (last_separator_index != 0) {
			/* We cut the path at the right place. */
			path[last_separator_index] = 0;
			*buffer_size = last_separator_index;
		} else {
			/* We only keep the slash. */
			path[1] = 0;
			*buffer_size = 1;
		}
#endif
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
#ifdef RT_DEFINE_WINDOWS
	rt_un written;
#else
	ssize_t written;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* Returns zero and set last error in case of issue (except if the buffer is too small). */
	written = GetModuleFileName(NULL, buffer, (DWORD)buffer_capacity);
	if (RT_UNLIKELY(!written)) goto error;
	/* GetModuleFileName does not fail if the buffer is too small. */
	if (RT_UNLIKELY(written == buffer_capacity)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
#else
	/* readlink does not add the zero character. */
	/* On error, -1 is returned and errno is set to indicate the error. */
	written = readlink("/proc/self/exe", buffer, buffer_capacity);
	if (RT_UNLIKELY(written == -1)) {
		written = 0;
		goto error;
	}

	if (RT_UNLIKELY((rt_un)written >= buffer_capacity)) {
		/* There is no room for the zero character. */
		buffer[buffer_capacity - 1] = 0;
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	/* Add zero terminating character. written is less than buffer_capacity at this point. */
	buffer[written] = 0;
#endif

	ret = RT_OK;
free:
	*buffer_size = written;
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_get_name(const rt_char *path, rt_un path_size, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	rt_un last_separator_index;
	rt_un name_last_character_index;
	rt_s ret;

	if (path_size == 0) {
		*buffer_size = 0;
		if (RT_UNLIKELY(!rt_char_append_char(_R('.'), buffer, buffer_capacity, buffer_size)))
			goto error;
	} else {
		last_separator_index = rt_file_path_get_last_separator_index(path, path_size);
		if (last_separator_index == RT_TYPE_MAX_UN) {
			/* No interesting last separator, copy everything. */
			*buffer_size = path_size;
			if (RT_UNLIKELY(!rt_char_copy(path, *buffer_size, buffer, buffer_capacity)))
				goto error;
		} else {
			/* Copy everything after the last interesting separator. */
			*buffer_size = path_size - (last_separator_index + 1);
			/* Because of rt_file_path_get_last_separator_index, there must be characters after the last interesting separator. */
			if (RT_UNLIKELY(!rt_char_copy(&path[last_separator_index + 1], *buffer_size, buffer, buffer_capacity)))
				goto error;
		}

		/* Removing trailing separators, but always keep the first character. */
		name_last_character_index = *buffer_size - 1;
		while (name_last_character_index != RT_TYPE_MAX_UN && RT_FILE_PATH_IS_SEPARATOR(buffer[name_last_character_index]))
			name_last_character_index--;
		if (name_last_character_index == RT_TYPE_MAX_UN) {
			/* Separators only, keep the first character. */
			buffer[1] = 0;
			*buffer_size = 1;
		} else {
			/* Cut after the last name character. */
			buffer[name_last_character_index + 1] = 0;
			*buffer_size = name_last_character_index + 1;
		}
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_path_get_temp_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD returned_value;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	/* Returns the characters copied to buffer, not including the zero terminating character. */
	returned_value = GetTempPath((DWORD)buffer_capacity, buffer);
	/* GetTempPath returns zero and set last error in case of error. */
	if (RT_UNLIKELY(!returned_value))
		goto error;

	/* If the buffer is too small GetTempPath returns the required buffer size. */
	if (RT_UNLIKELY(returned_value > buffer_capacity)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}
	*buffer_size = returned_value;

#else
	if (!rt_env_var_get(_R("TMPDIR"), buffer, buffer_capacity, buffer_size)) {
		if (RT_UNLIKELY(errno != EINVAL))
			goto error;
#ifdef P_tmpdir
		/* The variable has not be found. Use P_tmpdir macro instead. */
		*buffer_size = rt_char_get_size(P_tmpdir);
		if (RT_UNLIKELY(!rt_char_copy(P_tmpdir, *buffer_size, buffer, buffer_capacity)))
			goto error;
#else
		/* The variable has not be found and P_tmpdir macro is not defined. Use /tmp. */
		*buffer_size = 4;
		if (RT_UNLIKELY(!rt_char_copy(_R("/tmp"), *buffer_size, buffer, buffer_capacity)))
			goto error;
#endif
	}
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#ifdef RT_DEFINE_WINDOWS

static rt_s rt_file_path_sh_get_folder_path(int csidl, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
	HRESULT result_handle;
	rt_s ret;

	/* SHGetFolderPath expect buffer length to be at least MAX_PATH. */
	if (RT_UNLIKELY(buffer_capacity < MAX_PATH)) {
		rt_error_set_last(RT_ERROR_INSUFFICIENT_BUFFER);
		goto error;
	}

	result_handle = SHGetFolderPath(RT_NULL, csidl, RT_NULL, 0, buffer);
	if (RT_UNLIKELY(FAILED(result_handle))) {
		SetLastError(HRESULT_CODE(result_handle));
		goto error;
	}
	*buffer_size = rt_char_get_size(buffer);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

#endif

rt_s rt_file_path_get_home_dir(rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_WINDOWS
	return rt_file_path_sh_get_folder_path(CSIDL_PROFILE, buffer, buffer_capacity, buffer_size);
#else

	uid_t user_id;
	long int password_buffer_size;
	char *password_buffer;
	int returned_value;
	struct passwd password_info_struct;
	struct passwd *password_info;
	rt_s ret;

	/* Use folder pointer by HOME environment variable if available. */
	if (!rt_env_var_get(_R("HOME"), buffer, buffer_capacity, buffer_size)) {
		/* The getuid function is always successful. */
		user_id = getuid();

		/* Compute size for getpwuid_r buffer. */
		password_buffer_size = sysconf(_SC_GETPW_R_SIZE_MAX);
		if (password_buffer_size == -1) {
			 /* Should be more than enough */
			password_buffer_size = 16384;
		}

		/* Allocate buffer. */
		password_buffer = malloc(password_buffer_size);
		if (RT_UNLIKELY(!password_buffer))
			goto error;

		/* The getpwuid_r function must be used in place of getpwuid in a multithreaded environment. */
		returned_value = getpwuid_r(user_id, &password_info_struct, password_buffer, password_buffer_size, &password_info);
		/* In case of issue (Error or no matching password), password_info is set to NULL. */
		if (RT_UNLIKELY(!password_info)) {
			if (returned_value) {
				/* In case of error getpwuid_r return an error code. */
				errno = returned_value;
			} else {
				/* No matching password found. */
				rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
			}
			free(password_buffer);
			goto error;
		}

		*buffer_size = rt_char_get_size(password_info_struct.pw_dir);
		if (RT_UNLIKELY(!rt_char_copy(password_info_struct.pw_dir, *buffer_size, buffer, buffer_capacity))) {
			free(password_buffer);
			goto error;
		}
		free(password_buffer);
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#endif
}

rt_s rt_file_path_get_application_data_dir(const rt_char *application_name, rt_char *buffer, rt_un buffer_capacity, rt_un *buffer_size)
{
#ifdef RT_DEFINE_LINUX
	rt_char local_application_name[RT_FILE_PATH_NAME_SIZE];
	rt_un application_name_size;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (RT_UNLIKELY(!rt_file_path_sh_get_folder_path(CSIDL_APPDATA, buffer, buffer_capacity, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append(application_name, rt_char_get_size(application_name), buffer, buffer_capacity, buffer_size))) goto error;

#else

	if (RT_UNLIKELY(!rt_file_path_get_home_dir(buffer, buffer_capacity, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_file_path_append_separator(buffer, buffer_capacity, buffer_size))) goto error;
	if (RT_UNLIKELY(!rt_char_append_char(_R('.'), buffer, buffer_capacity, buffer_size))) goto error;

	application_name_size = rt_char_get_size(application_name);
	if (RT_UNLIKELY(!rt_char_copy(application_name, application_name_size, local_application_name, RT_FILE_PATH_NAME_SIZE))) goto error;
	rt_char_fast_lower(local_application_name);
	if (RT_UNLIKELY(!rt_char_append(local_application_name, application_name_size, buffer, buffer_capacity, buffer_size))) goto error;

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
