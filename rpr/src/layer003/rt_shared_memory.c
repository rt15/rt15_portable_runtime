#include "layer003/rt_shared_memory.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

rt_s rt_shared_memory_create(struct rt_shared_memory *shared_memory, const rt_char *name, rt_un size, enum rt_shared_memory_right shared_memory_right)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD flProtect;
	DWORD dwDesiredAccess;
	HANDLE file_mapping_handle;
	rt_b file_mapping_created = RT_FALSE;
#else
	int shared_memory_file_descriptor;
	rt_b shared_memory_file_descriptor_created = RT_FALSE;
	rt_b shared_memory_created = RT_FALSE;
	int prot;
#endif
	void *area;
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	switch (shared_memory_right) {
	case RT_SHARED_MEMORY_RIGHT_READ:
		flProtect = PAGE_READONLY;
		dwDesiredAccess = FILE_MAP_READ;
		break;
	case RT_SHARED_MEMORY_RIGHT_READ_WRITE:
		flProtect = PAGE_READWRITE;
		dwDesiredAccess = FILE_MAP_ALL_ACCESS;
		break;
	case RT_SHARED_MEMORY_RIGHT_EXECUTE_READ:
		flProtect = PAGE_EXECUTE_READ;
		dwDesiredAccess = FILE_MAP_READ | FILE_MAP_EXECUTE;
		break;
	case RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE:
		flProtect = PAGE_EXECUTE_READWRITE;
		dwDesiredAccess = FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* CreateFileMapping returns null and set last error in case issue. */
	file_mapping_handle = CreateFileMapping(INVALID_HANDLE_VALUE, RT_NULL, flProtect, 0, (DWORD)size, name);
	if (RT_UNLIKELY(!file_mapping_handle))
		goto error;
	file_mapping_created = RT_TRUE;

	/* MapViewOfFile returns null and set last error in case issue. */
	area = MapViewOfFile(file_mapping_handle, dwDesiredAccess, 0, 0, size);
	if (RT_UNLIKELY(!area))
		goto error;

	shared_memory->file_mapping_handle = file_mapping_handle;
#else
	switch (shared_memory_right) {
	case RT_SHARED_MEMORY_RIGHT_READ:
		prot = PROT_READ;
		break;
	case RT_SHARED_MEMORY_RIGHT_READ_WRITE:
		prot = PROT_READ | PROT_WRITE;
		break;
	case RT_SHARED_MEMORY_RIGHT_EXECUTE_READ:
		prot = PROT_EXEC| PROT_READ;
		break;
	case RT_SHARED_MEMORY_RIGHT_EXECUTE_READ_WRITE:
		prot = PROT_EXEC | PROT_READ | PROT_WRITE;
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto error;
	}

	/* FD_CLOEXEC is set on the file descriptor. */
	/* shm_open returns -1 and set errno in case of error. */
	/* We need O_RDWR for ftruncate. */
	/* With O_CREAT but without O_EXCL, we either create a new shared memory or reuse the existing one. */
	shared_memory_file_descriptor = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (RT_UNLIKELY(shared_memory_file_descriptor == -1))
		goto error;
	shared_memory_created = RT_TRUE;
	shared_memory_file_descriptor_created = RT_TRUE;

	/* Set the size of the shared memory. */
	/* ftruncate returns -1 and set errno in case of error. */
	if (RT_UNLIKELY(ftruncate(shared_memory_file_descriptor, size) == -1))
		goto error;

	/* On error, mmap returns MAP_FAILED and set errno. */
	area = mmap(RT_NULL, size, prot, MAP_SHARED, shared_memory_file_descriptor, 0);
	if (RT_UNLIKELY(area == MAP_FAILED))
		goto error;
#endif
	shared_memory->area = area;
	shared_memory->size = size;

	ret = RT_OK;
free:
#ifdef RT_DEFINE_LINUX
	if (shared_memory_file_descriptor_created) {
		shared_memory_file_descriptor_created = RT_FALSE;
		/* close returns -1 and set errno in case of error. */
		if (RT_UNLIKELY(close(shared_memory_file_descriptor) && ret))
			goto error;
	}
#endif
	return ret;

error:
#ifdef RT_DEFINE_WINDOWS
	if (file_mapping_created) {
		file_mapping_created = RT_FALSE;
		if (RT_UNLIKELY(!CloseHandle(file_mapping_handle)))
			goto error;
	}
#else
	if (shared_memory_file_descriptor_created) {
		shared_memory_file_descriptor_created = RT_FALSE;
		close(shared_memory_file_descriptor);
	}
	if (shared_memory_created) {
		shared_memory_created = RT_FALSE;
		shm_unlink(name);
	}
#endif
	ret = RT_FAILED;
	goto free;
}

rt_s rt_shared_memory_free(struct rt_shared_memory *shared_memory)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_WINDOWS
	/* UnmapViewOfFile returns 0 and set last error in case of issue. */
	if (RT_UNLIKELY(!UnmapViewOfFile(shared_memory->area)))
		ret = RT_FAILED;
	if (RT_UNLIKELY(!CloseHandle(shared_memory->file_mapping_handle)))
		ret = RT_FAILED;
#else
	/* munmap returns -1 and set errno in case of error. */
	if (RT_UNLIKELY(munmap(shared_memory->area, shared_memory->size) == -1))
		ret = RT_FAILED;
#endif
	return ret;
}

rt_s rt_shared_memory_destroy(RT_WINDOWS_UNUSED const rt_char *name)
{
	rt_s ret = RT_OK;

#ifdef RT_DEFINE_LINUX
	/* shm_unlink returns -1 and set errno in case of error. */
	if (RT_UNLIKELY(shm_unlink(name) == -1))
		ret = RT_FAILED;
#endif
	return ret;
}
