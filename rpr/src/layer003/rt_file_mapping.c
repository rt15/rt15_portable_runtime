#include "layer003/rt_file_mapping.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_file_mapping_create(struct rt_file_mapping *file_mapping, const rt_char *file_path, enum rt_file_mapping_right file_mapping_right, RT_LINUX_UNUSED rt_un max_size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD dwDesiredAccess;
	DWORD flProtect;
	HANDLE file_handle;
	rt_b file_handle_created = RT_FALSE;
	DWORD file_size;
	HANDLE file_mapping_handle;
	rt_b file_mapping_handle_created = RT_FALSE;
#else
	int open_flags;
	int file_descriptor;
	rt_b file_descriptor_created = RT_FALSE;
	struct stat file_stat;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	switch (file_mapping_right) {
	case RT_FILE_MAPPING_RIGHT_READ:
		dwDesiredAccess = GENERIC_READ;
		flProtect = PAGE_READONLY;
		break;
	case RT_FILE_MAPPING_RIGHT_READ_WRITE:
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		flProtect = PAGE_READWRITE;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ:
		dwDesiredAccess = GENERIC_EXECUTE | GENERIC_READ;
		flProtect = PAGE_EXECUTE_READ;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ_WRITE:
		dwDesiredAccess = GENERIC_EXECUTE | GENERIC_READ | GENERIC_WRITE;
		flProtect = PAGE_EXECUTE_READWRITE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* Returns INVALID_HANDLE_VALUE and sets last error in case of issue. */
	file_handle = CreateFile(file_path, dwDesiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, RT_NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, RT_NULL);
	if (RT_UNLIKELY(file_handle == INVALID_HANDLE_VALUE))
		goto error;
	file_handle_created = RT_TRUE;

	file_size = GetFileSize(file_handle, RT_NULL);
	if (RT_UNLIKELY(file_size == INVALID_FILE_SIZE))
		goto error;
	file_mapping->file_size = file_size;

	/* Returns null and sets last error in case of issue. */
	file_mapping_handle = CreateFileMapping(file_handle, RT_NULL, flProtect, 0, (DWORD)max_size, RT_NULL);
	if (RT_UNLIKELY(!file_mapping_handle))
		goto error;
	file_mapping_handle_created = RT_TRUE;

	file_mapping->file_handle = file_handle;
	file_mapping->file_mapping_handle = file_mapping_handle;
	file_mapping->initial_file_size = file_size;
	file_mapping->max_size = max_size;
#else
	open_flags = O_CLOEXEC;
	switch (file_mapping_right) {
	case RT_FILE_MAPPING_RIGHT_READ:
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ:
		open_flags |= O_RDONLY;
		break;
	case RT_FILE_MAPPING_RIGHT_READ_WRITE:
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ_WRITE:
		open_flags |= O_RDWR;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* open returns -1 and sets errno in case of issue. */
	file_descriptor = open(file_path, open_flags);
	if (file_descriptor == -1)
		goto error;
	file_descriptor_created = RT_TRUE;

	/* fstat returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(fstat(file_descriptor, &file_stat)))
		goto error;
	file_mapping->file_size = file_stat.st_size;

	file_mapping->file_descriptor = file_descriptor;
#endif

	ret = RT_OK;
free:
	return ret;

error:
#ifdef RT_DEFINE_WINDOWS
	if (file_mapping_handle_created) {
		file_mapping_handle_created = RT_FALSE;
		CloseHandle(file_mapping_handle);
	}
	if (file_handle_created) {
		file_handle_created = RT_FALSE;
		CloseHandle(file_handle);
	}
#else
	if (file_descriptor_created) {
		file_descriptor_created = RT_FALSE;
		close(file_descriptor);
	}
#endif
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_mapping_set_file_size(struct rt_file_mapping *file_mapping, rt_un size)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	if (size > file_mapping->max_size) {
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}
#else
	/* ftruncate returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(ftruncate(file_mapping->file_descriptor, size) == -1))
		goto error;
#endif
	file_mapping->file_size = size;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_mapping_free(struct rt_file_mapping *file_mapping)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	/* UnmapViewOfFile returns 0 and sets last error in case of issue. */
	if (RT_UNLIKELY(!CloseHandle(file_mapping->file_mapping_handle)))
		ret = RT_FAILED;

	if (file_mapping->file_size != file_mapping->initial_file_size) {

		/* SetFilePointer returns INVALID_SET_FILE_POINTER and sets last error in case of issue. */
		if (RT_UNLIKELY(SetFilePointer(file_mapping->file_handle, (LONG)file_mapping->file_size, RT_NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER))
			ret = RT_FAILED;

		if (ret) {
			/* SetEndOfFile returns -1 and sets last error in case of issue. */
			if (RT_UNLIKELY(!SetEndOfFile(file_mapping->file_handle)))
				ret = RT_FAILED;
		}
	}

	if (RT_UNLIKELY(!CloseHandle(file_mapping->file_handle)))
		ret = RT_FAILED;
#else
	/* close returns -1 and sets errno in case of error. */
	if (RT_UNLIKELY(close(file_mapping->file_descriptor) == -1))
		ret = RT_FAILED;
#endif

	return ret;
}

rt_s rt_file_mapping_view_create(struct rt_file_mapping_view *file_mapping_view, struct rt_file_mapping *file_mapping, enum rt_file_mapping_right file_mapping_right, rt_un offset, rt_un size)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD dwDesiredAccess;
#else
	int prot;
#endif
	rt_un view_size;
	void *area;
	rt_s ret;

	if (size)
		view_size = size;
	else
		view_size = file_mapping->file_size - offset;

#ifdef RT_DEFINE_WINDOWS

	switch (file_mapping_right) {
	case RT_FILE_MAPPING_RIGHT_READ:
		dwDesiredAccess = FILE_MAP_READ;
		break;
	case RT_FILE_MAPPING_RIGHT_READ_WRITE:
		dwDesiredAccess = FILE_MAP_ALL_ACCESS;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ:
		dwDesiredAccess = FILE_MAP_READ | FILE_MAP_EXECUTE;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ_WRITE:
		dwDesiredAccess = FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* Returns null and sets last error in case of issue. */
	area = MapViewOfFile(file_mapping->file_mapping_handle, dwDesiredAccess, 0, (DWORD)offset, view_size);
	if (RT_UNLIKELY(!area))
		goto error;

#else
	switch (file_mapping_right) {
	case RT_FILE_MAPPING_RIGHT_READ:
		prot = PROT_READ;
		break;
	case RT_FILE_MAPPING_RIGHT_READ_WRITE:
		prot = PROT_READ | PROT_WRITE;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ:
		prot = PROT_EXEC | PROT_READ;
		break;
	case RT_FILE_MAPPING_RIGHT_EXECUTE_READ_WRITE:
		prot = PROT_EXEC | PROT_READ | PROT_WRITE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* mmap returns MAP_FAILED and sets errno in case of issue. */
	area = mmap(RT_NULL, view_size, prot, MAP_SHARED, file_mapping->file_descriptor, offset);
	if (RT_UNLIKELY(area == MAP_FAILED))
		goto error;
#endif

	file_mapping_view->file_mapping = file_mapping;
	file_mapping_view->area = area;
	file_mapping_view->view_size = view_size;

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_mapping_view_flush(RT_UNUSED struct rt_file_mapping_view *file_mapping_view, void *area, rt_un size)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* FlushViewOfFile returns 0 and sets last error in case of issue. */
	if (RT_UNLIKELY(!FlushViewOfFile(area, size)))
		goto error;
#else
	/* msync returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(msync(area, size, MS_SYNC) == -1))
		goto error;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_file_mapping_view_free(struct rt_file_mapping_view *file_mapping_view)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	/* UnmapViewOfFile returns 0 and sets last error in case of issue. */
	if (RT_UNLIKELY(!UnmapViewOfFile(file_mapping_view->area)))
		ret = RT_FAILED;
#else
	/* munmap returns -1 and sets errno in case of issue. */
	if (RT_UNLIKELY(munmap(file_mapping_view->area, file_mapping_view->view_size) == -1))
		ret = RT_FAILED;
#endif

	return ret;
}
