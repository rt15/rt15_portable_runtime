#include "layer003/rt_file.h"

#include "layer001/rt_os_headers.h"
#include "layer002/rt_error.h"

#ifndef RT_DEFINE_WINDOWS

/* rwx to user, rx to group and others. */
#define RT_FILE_RIGHTS S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH

#endif

rt_s rt_file_create(struct rt_file *file, const rt_char *file_path, enum rt_file_mode mode)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD flags;
	DWORD creation_disposition;
	rt_h file_handle;
#else
	int flags;
	rt_n32 file_descriptor;
#endif
	rt_s ret = RT_FAILED;

	/* Flags determination. */
	switch (mode) {
	case RT_FILE_MODE_READ:
#ifdef RT_DEFINE_WINDOWS
		flags = GENERIC_READ;
		creation_disposition = OPEN_EXISTING;
#else
		flags = O_CLOEXEC | O_RDONLY;
#endif
		break;
	case RT_FILE_MODE_READ_WRITE:
#ifdef RT_DEFINE_WINDOWS
		flags = GENERIC_READ | GENERIC_WRITE;
		creation_disposition = OPEN_ALWAYS;
#else
		flags = O_CLOEXEC | O_CREAT | O_RDWR;
#endif
		break;
	case RT_FILE_MODE_TRUNCATE:
#ifdef RT_DEFINE_WINDOWS
		flags = GENERIC_READ | GENERIC_WRITE;
		creation_disposition = CREATE_ALWAYS;
#else
		flags = O_CLOEXEC | O_CREAT | O_RDWR | O_TRUNC;
#endif
		break;
	case RT_FILE_MODE_NEW:
#ifdef RT_DEFINE_WINDOWS
		flags = GENERIC_READ | GENERIC_WRITE;
		creation_disposition = CREATE_NEW;
#else
		flags = O_CLOEXEC | O_CREAT | O_EXCL | O_RDWR;
#endif
		break;
	default:
		rt_error_set_last(RT_ERROR_BAD_ARGUMENTS);
		goto end;
	}

#ifdef RT_DEFINE_WINDOWS

	file_handle = CreateFile(file_path,
				 flags,
				 FILE_SHARE_READ | FILE_SHARE_WRITE,
				 NULL,
				 creation_disposition,
				 FILE_ATTRIBUTE_NORMAL,
				 NULL);
	if (RT_UNLIKELY(file_handle == INVALID_HANDLE_VALUE))
		goto end;

#else
	file_descriptor = open(file_path, flags, RT_FILE_RIGHTS);
	if (RT_UNLIKELY(file_descriptor == -1))
		goto end;
#endif

#ifdef RT_DEFINE_WINDOWS
	rt_io_device_create_from_handle(&file->io_device, file_handle);
#else
	rt_io_device_create_from_file_descriptor(&file->io_device, file_descriptor);
#endif

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_file_get_size(struct rt_file *file, rt_un64 *file_size)
{
	rt_un64 old_position;
	rt_s ret = RT_FAILED;

	/* Backup the current position */
	if (RT_UNLIKELY(!rt_file_get_pointer(file, &old_position))) goto end;

	/* Go to end of file. */
	if (RT_UNLIKELY(!rt_file_set_pointer(file, 0, RT_FILE_POSITION_END))) goto end;

	/* Get the new position which is the file size. */
	if (RT_UNLIKELY(!rt_file_get_pointer(file, file_size))) goto end;

	/* Go back to original position. */
	if (RT_UNLIKELY(!rt_file_set_pointer(file, old_position, RT_FILE_POSITION_BEGIN))) goto end;

	ret = RT_OK;
end:
	return ret;
}

rt_s rt_file_set_pointer(struct rt_file *file, rt_n64 offset, enum rt_file_position position)
{
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER large_offset;
#else
	rt_n32 flag;
#endif
	rt_s ret;

	ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	large_offset.QuadPart = offset;
	/* If the function fails, the return value is zero and last error is set. */
	if (SetFilePointerEx(file->io_device.handle, large_offset, RT_NULL, (DWORD)position))
		ret = RT_OK;
#else
	switch (position) {
	case RT_FILE_POSITION_END:
		flag = SEEK_END;
		break;
	case RT_FILE_POSITION_CURRENT:
		flag = SEEK_CUR;
		break;
	default:
		flag = SEEK_SET;
		break;
	}
	/* In case of error, lseek returns -1 and sets errno. */
	if (lseek(file->io_device.file_descriptor, offset, flag) != -1)
		ret = RT_OK;
#endif

	return ret;
}

rt_s rt_file_get_pointer(struct rt_file *file, rt_un64 *offset)
{
#ifdef RT_DEFINE_WINDOWS
	LARGE_INTEGER distance_to_move;
	LARGE_INTEGER new_file_pointer;
#else
	off_t returned_value;
#endif
	rt_s ret = RT_FAILED;

#ifdef RT_DEFINE_WINDOWS
	distance_to_move.QuadPart = 0;
	/* If the function fails, the return value is zero and last error is set. */
	if (RT_UNLIKELY(!SetFilePointerEx(file->io_device.handle, distance_to_move, &new_file_pointer, FILE_CURRENT))) goto end;
	*offset = new_file_pointer.QuadPart;
#else
	returned_value = lseek(file->io_device.file_descriptor, 0, SEEK_CUR);
	if (RT_UNLIKELY(returned_value < 0))
		goto end;
	*offset = returned_value;
#endif

	ret = RT_OK;
end:
	return ret;
}
