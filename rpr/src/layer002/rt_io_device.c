#include "layer002/rt_io_device.h"

#include "layer001/rt_memory.h"
#include "layer001/rt_os_headers.h"

#ifdef RT_DEFINE_WINDOWS
void rt_io_device_create_from_handle(struct rt_io_device *io_device, rt_h handle)
{
	io_device->handle = handle;
	io_device->input_stream.read = &rt_io_device_read;
	io_device->output_stream.write = &rt_io_device_write;
}
#else
void rt_io_device_create_from_file_descriptor(struct rt_io_device *io_device, rt_n32 file_descriptor)
{
	io_device->file_descriptor = file_descriptor;
	io_device->input_stream.read = &rt_io_device_read;
	io_device->output_stream.write = &rt_io_device_write;
}
#endif

rt_s rt_io_device_create_from_std_input(struct rt_io_device *io_device)
{
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
	rt_s ret;
#endif

#ifdef RT_DEFINE_WINDOWS
	handle = GetStdHandle(STD_INPUT_HANDLE);
	/* GetStdHandle can return NULL but we do not consider it an error. */
	if (handle == INVALID_HANDLE_VALUE)
		goto error;

	rt_io_device_create_from_handle(io_device, handle);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#else
	rt_io_device_create_from_file_descriptor(io_device, 0);
	return RT_OK;
#endif
}

rt_s rt_io_device_create_from_std_output(struct rt_io_device *io_device)
{
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
	rt_s ret;
#endif

#ifdef RT_DEFINE_WINDOWS
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	/* GetStdHandle can return NULL but we do not consider it an error. */
	if (handle == INVALID_HANDLE_VALUE)
		goto error;

	rt_io_device_create_from_handle(io_device, handle);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#else
	rt_io_device_create_from_file_descriptor(io_device, 1);
	return RT_OK;
#endif
}

rt_s rt_io_device_create_from_std_error(struct rt_io_device *io_device)
{
#ifdef RT_DEFINE_WINDOWS
	rt_h handle;
	rt_s ret;
#endif

#ifdef RT_DEFINE_WINDOWS
	handle = GetStdHandle(STD_ERROR_HANDLE);
	/* GetStdHandle can return NULL but we do not consider it an error. */
	if (handle == INVALID_HANDLE_VALUE)
		goto error;

	rt_io_device_create_from_handle(io_device, handle);

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
#else
	rt_io_device_create_from_file_descriptor(io_device, 2);
	return RT_OK;
#endif
}

rt_s rt_io_device_read(struct rt_input_stream *input_stream, rt_char8 *buffer, rt_un bytes_to_read, rt_un *bytes_read)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD local_bytes_read;
#else
	ssize_t local_bytes_read;
#endif
	struct rt_io_device *io_device;
	rt_s ret;

	ret = RT_FAILED;

	io_device = RT_MEMORY_CONTAINER_OF(input_stream, struct rt_io_device, input_stream);

#ifdef RT_DEFINE_WINDOWS
	/* TODO: Manage more than 4Go? */
	if (!ReadFile(io_device->handle, buffer, (DWORD)bytes_to_read, &local_bytes_read, NULL))
		goto error;
	*bytes_read = local_bytes_read;
#else
	/* read returns -1 and set errno in case of issue. */
	local_bytes_read = read(io_device->file_descriptor, buffer, bytes_to_read);
	if (local_bytes_read == -1)
		goto error;
	*bytes_read = local_bytes_read;
#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_io_device_write(struct rt_output_stream *output_stream, const rt_char8 *data, rt_un bytes_to_write)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD bytes_written;
#endif
	struct rt_io_device *io_device;
	rt_s ret;

	ret = RT_FAILED;

	io_device = RT_MEMORY_CONTAINER_OF(output_stream, struct rt_io_device, output_stream);

#ifdef RT_DEFINE_WINDOWS
	if (WriteFile(io_device->handle,
		      data,
		      (DWORD)bytes_to_write,
		      &bytes_written,
		      NULL))
		if (bytes_written == bytes_to_write)
			ret = RT_OK;
#else
	if (write(io_device->file_descriptor, data, bytes_to_write) != -1)
		ret = RT_OK;
#endif

	return ret;
}

rt_s rt_io_device_is_inheritable(struct rt_io_device *io_device, rt_b *inheritable)
{
#ifdef RT_DEFINE_WINDOWS
	DWORD flags;
#else
	int flags;
#endif
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS

	if (!GetHandleInformation(io_device->handle, &flags)) goto error;
	if (flags & HANDLE_FLAG_INHERIT)
		*inheritable = RT_TRUE;
	else
		*inheritable = RT_FALSE;

#else

	flags = fcntl(io_device->file_descriptor, F_GETFD, 0);
	if (flags == -1) goto error;

	if (flags & FD_CLOEXEC)
		*inheritable = RT_FALSE;
	else
		*inheritable = RT_TRUE;

#endif

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_io_device_set_inheritable(struct rt_io_device *io_device, rt_b inheritable)
{
	rt_b current_value;
#ifdef RT_DEFINE_LINUX
	int flags;
#endif
	rt_s ret;

	if (!rt_io_device_is_inheritable(io_device, &current_value))
		goto error;

#ifdef RT_DEFINE_LINUX
	flags = fcntl(io_device->file_descriptor, F_GETFD, 0);
	if (flags == -1)
		goto error;
#endif

	if (inheritable && !current_value) {
#ifdef RT_DEFINE_WINDOWS
		/* Add inheritance. */
		if (!SetHandleInformation(io_device->handle, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
			goto error;
#else
		/* Remove FD_CLOEXEC flag so that the descriptor will be valid in children. */
		if (fcntl(io_device->file_descriptor, F_SETFD, flags & ~FD_CLOEXEC) == -1)
			goto error;
#endif
	} else if (!inheritable && current_value) {
#ifdef RT_DEFINE_WINDOWS
		/* Remove inheritance. */
		if (!SetHandleInformation(io_device->handle, HANDLE_FLAG_INHERIT, 0))
			goto error;
#else
		/* Add FD_CLOEXEC flag so that the descriptor will not be available in children. */
		if (fcntl(io_device->file_descriptor, F_SETFD, flags | FD_CLOEXEC) == -1)
			goto error;
#endif
	}

	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}

rt_s rt_io_device_free(struct rt_io_device *io_device)
{
	rt_s ret;

#ifdef RT_DEFINE_WINDOWS
	/* If the function succeeds, the return value is nonzero. Otherwise, zero is returned and last error is set. */
	if (!CloseHandle(io_device->handle))
		goto error;
#else
	/* Return zero on success. On error, -1 is returned, and errno is set appropriately. */
	if (close(io_device->file_descriptor))
		goto error;
#endif
	ret = RT_OK;
free:
	return ret;

error:
	ret = RT_FAILED;
	goto free;
}
